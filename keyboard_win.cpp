#ifdef _WIN32

#include "keyboard.h"
#include "keybaord_data.h"
#include "actor.h"
#include <list>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <climits>
#include <windows.h>

namespace PeonySword {
    class Keyboard {
        // key data
        static const int sDataSize = CHAR_MAX + 1;
        static bool sRunning;
        static std::mutex sDataLock;
        static KeyData sData[sDataSize];
        // key event queue
        // there is no race condition on the queue
        static std::mutex sEventLock;
        static std::condition_variable sEventCond;
        static std::queue<KeyEvent> sEventQueue;
        // receiver list
        static std::mutex sRecverLock;
        static std::list<Actor *> sRecverList;
        // event listen thread
        static std::thread *sListenThread;
        // event process thread
        static std::thread *sProcessThread;
        // system key code table
        static const KeyValuePair KeyValueTable[];
        static const int KeyValueTableSize;

        static KeyValue toKeyValue(int _key);

        static void dispatchEvent(const KeyEvent &);

        static LRESULT CALLBACK cbKeyboard(
                int, WPARAM, LPARAM);

        static void listenThreadEntry();

        static void processThreadEntry();

        static std::once_flag sOnceFlag;

    public:

        Keyboard();

        ~Keyboard();

        Keyboard(const Keyboard &) = delete;

        Keyboard(Keyboard &&) = delete;

        Keyboard &operator=(const Keyboard &) = delete;

        Keyboard &operator=(Keyboard &&) = delete;

        static void subscribeKeyEvent(Actor *);

        static void unsubscribeKeyEvent(Actor *);

        static bool getKeepEnable(KeyValue _kval);

        static void setKeepEnable(KeyValue _kval, bool _enable);

        static void setKeepEnableAll(bool _enable);
    };

    std::once_flag Keyboard::sOnceFlag;
    bool Keyboard::sRunning = true;
    std::mutex Keyboard::sDataLock;
    std::mutex Keyboard::sRecverLock;
    std::mutex Keyboard::sEventLock;
    std::condition_variable Keyboard::sEventCond;

    KeyData Keyboard::sData[sDataSize];
    std::queue<KeyEvent> Keyboard::sEventQueue;
    std::list<Actor *> Keyboard::sRecverList;
    std::thread *Keyboard::sListenThread = nullptr;
    std::thread *Keyboard::sProcessThread = nullptr;

    KeyValue Keyboard::toKeyValue(int _key) {
        int start = 0;
        int end = KeyValueTableSize;
        while (start < end) {
            int idx = start + (end - start) / 2;
            const KeyValuePair *pair =
                    KeyValueTable + idx;
            if (pair->key > _key) {
                end = idx;
            } else if (pair->key < _key) {
                start = idx + 1;
            } else {
                return pair->value;
            }
        }
        return KeyValueInvalid;
    }

    void Keyboard::dispatchEvent(const KeyEvent &e) {
        std::lock_guard lk(sRecverLock);
        for (Actor *actor: sRecverList) {
            switch (e.mKeyState) {
                case KeyStateDown: {
                    actor->keyPressEvent(e);
                    break;
                }
                case KeyStateUp: {
                    actor->keyReleaseEvent(e);
                    break;
                }
                default: {
                    break;
                }
            }
        }
    }

    LRESULT CALLBACK Keyboard::cbKeyboard(
            int code, WPARAM wParam, LPARAM lParam) {
        using namespace std::chrono;
        if (code < 0) {
            return CallNextHookEx(nullptr, code, wParam, lParam);
        }
        KBDLLHOOKSTRUCT *event = (KBDLLHOOKSTRUCT *) lParam;
        KeyEvent keyEvent;
        duration currStamp = system_clock::now().time_since_epoch();
        keyEvent.mStamp = duration_cast<milliseconds>(currStamp);
        keyEvent.mKeyValue = toKeyValue((int) event->vkCode);
        keyEvent.mKeyState = KeyStateInvalid;
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            keyEvent.mKeyState = KeyStateDown;
            {
                std::lock_guard lock(sDataLock);
                if (!sData[keyEvent.mKeyValue].mKeepEnable &&
                    sData[keyEvent.mKeyValue].mKeeping) {
                    return CallNextHookEx(nullptr, code, wParam, lParam);
                }
                sData[keyEvent.mKeyValue].mKeeping = true;
            }
        } else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
            keyEvent.mKeyState = KeyStateUp;
            {
                std::lock_guard lock(sDataLock);
                sData[keyEvent.mKeyValue].mKeeping = false;
            }
        }
        if (keyEvent.mKeyState != KeyStateInvalid) {
            sEventQueue.emplace(keyEvent);
            sEventCond.notify_one();
        }
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }

    void Keyboard::subscribeKeyEvent(Actor *_actor) {
        if (_actor == nullptr) {
            return;
        }
        {
            std::lock_guard lk(sRecverLock);
            sRecverList.emplace_back(_actor);
        }
    }

    void Keyboard::unsubscribeKeyEvent(Actor *_actor) {
        if (_actor == nullptr) {
            return;
        }
        std::lock_guard lk(sRecverLock);
        auto it = sRecverList.begin();
        while (it != sRecverList.end()) {
            if ((*it) == _actor) {
                sRecverList.erase(it);
                break;
            }
            ++it;
        }
    }

    void Keyboard::listenThreadEntry() {
        HHOOK hook = SetWindowsHookEx(
                WH_KEYBOARD_LL, cbKeyboard, nullptr, 0);
        if (hook == nullptr) {
            return;
        }
        while (sRunning) {
            MSG msg;
            if (GetMessage(&msg, nullptr, WM_KEYFIRST, WM_KEYLAST)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        UnhookWindowsHookEx(hook);
    }

    void Keyboard::processThreadEntry() {
        while (sRunning) {
            // process press and release events
            std::unique_lock lk(sEventLock);
            sEventCond.wait(lk, [] { return !sEventQueue.empty(); });
            while (!sEventQueue.empty()) {
                const KeyEvent &e = sEventQueue.front();
                dispatchEvent(e);
                sEventQueue.pop();
            }
            lk.unlock();
        }
    }

    Keyboard::Keyboard() {
        std::call_once(sOnceFlag, [] {
            if (sListenThread == nullptr) {
                sListenThread = new std::thread(
                        Keyboard::listenThreadEntry);
            }
            if (sProcessThread == nullptr) {
                sProcessThread = new std::thread(
                        Keyboard::processThreadEntry);
            }
        });
    }

    Keyboard::~Keyboard() {
        std::lock_guard lk(sRecverLock);
        if (!sRecverList.empty()) {
            return;
        }
        sRunning = false;
        if (sListenThread != nullptr &&
            sListenThread->joinable()) {
            sListenThread->join();
        }
        if (sProcessThread != nullptr &&
            sProcessThread->joinable()) {
            sProcessThread->join();
        }
    }

    bool Keyboard::getKeepEnable(KeyValue _kval) {
        if (_kval <= KeyValueInvalid ||
            _kval >= KeyValueUpperLimit) {
            return false;
        }
        {
            std::lock_guard lk(sDataLock);
            return sData[_kval].mKeepEnable;
        }
    }

    void Keyboard::setKeepEnable(KeyValue _kval, bool _enable) {
        if (_kval <= KeyValueInvalid ||
            _kval >= KeyValueUpperLimit) {
            return;
        }
        {
            std::lock_guard lk(sDataLock);
            sData[_kval].mKeepEnable = _enable;

        }
    }

    void Keyboard::setKeepEnableAll(bool _enable) {
        int idx = 0;
        while (idx < sDataSize) {
            sData[idx].mKeepEnable = _enable;
            ++idx;
        }
    }

    static Keyboard sKBData;

    void subscribeKeyEvent(Actor *_actor) {
        Keyboard::subscribeKeyEvent(_actor);
    }

    void unsubscribeKeyEvent(Actor *_actor) {
        Keyboard::unsubscribeKeyEvent(_actor);
    }

    bool getKeepEnable(KeyValue _kval) {
        return Keyboard::getKeepEnable(_kval);
    }

    void setKeepEnable(KeyValue _kval, bool _enable) {
        Keyboard::setKeepEnable(_kval, _enable);
    }

    void setKeepEnableAll(bool _enable) {
        Keyboard::setKeepEnableAll(_enable);
    }

    bool KeyEvent::operator==(
            const KeyEvent &_event) const {
        return mKeyValue == _event.mKeyValue &&
               mKeyState == _event.mKeyState;
    }

    bool KeyEvent::operator!=(
            const KeyEvent &_event) const {
        return mKeyValue != _event.mKeyValue ||
               mKeyState != _event.mKeyState;
    }

    const KeyValuePair Keyboard::KeyValueTable[] = {
            {8,   KeyValueBack},
            {9,   KeyValueTab},
            {13,  KeyValueEnter},
            {19,  KeyValuePause},
            {20,  KeyValueCaps},
            {27,  KeyValueEscape},
            {32,  KeyValueSpace},
            {33,  KeyValuePgUp},
            {34,  KeyValuePgDown},
            {35,  KeyValueEnd},
            {36,  KeyValueHome},
            {37,  KeyValueLeft},
            {38,  KeyValueUp},
            {39,  KeyValueRight},
            {40,  KeyValueDown},
            {44,  KeyValuePrtScreen},
            {45,  KeyValueInsert},
            {46,  KeyValueDelete},
            {48,  KeyValue0},
            {49,  KeyValue1},
            {50,  KeyValue2},
            {51,  KeyValue3},
            {52,  KeyValue4},
            {53,  KeyValue5},
            {54,  KeyValue6},
            {55,  KeyValue7},
            {56,  KeyValue8},
            {57,  KeyValue9},
            {65,  KeyValueA},
            {66,  KeyValueB},
            {67,  KeyValueC},
            {68,  KeyValueD},
            {69,  KeyValueE},
            {70,  KeyValueF},
            {71,  KeyValueG},
            {72,  KeyValueH},
            {73,  KeyValueI},
            {74,  KeyValueJ},
            {75,  KeyValueK},
            {76,  KeyValueL},
            {77,  KeyValueM},
            {78,  KeyValueN},
            {79,  KeyValueO},
            {80,  KeyValueP},
            {81,  KeyValueQ},
            {82,  KeyValueR},
            {83,  KeyValueS},
            {84,  KeyValueT},
            {85,  KeyValueU},
            {86,  KeyValueV},
            {87,  KeyValueW},
            {88,  KeyValueX},
            {89,  KeyValueY},
            {90,  KeyValueZ},
            {91,  KeyValueStart},
            {93,  KeyValueMenu},
            {112, KeyValueF1},
            {113, KeyValueF2},
            {114, KeyValueF3},
            {115, KeyValueF4},
            {116, KeyValueF5},
            {117, KeyValueF6},
            {118, KeyValueF7},
            {119, KeyValueF8},
            {120, KeyValueF9},
            {121, KeyValueF10},
            {122, KeyValueF11},
            {123, KeyValueF12},
            {145, KeyValueScrollLock},
            {160, KeyValueLeftShift},
            {161, KeyValueRightShift},
            {162, KeyValueLeftCtrl},
            {163, KeyValueRightCtrl},
            {164, KeyValueLeftAlt},
            {165, KeyValueRightAlt},
            {186, KeyValueSemiconlon},
            {187, KeyValueEqual},
            {188, KeyValueComma},
            {189, KeyValueSub},
            {190, KeyValuePeriod},
            {191, KeyValueSlash},
            {192, KeyValueTilde},
            {219, KeyValueLeftBracket},
            {220, KeyValueBackslash},
            {221, KeyValueRightBracket},
            {222, KeyValueQuote},
    };
    const int Keyboard::KeyValueTableSize =
            sizeof(KeyValueTable) / sizeof(KeyValuePair);
}
#endif // _WIN32
