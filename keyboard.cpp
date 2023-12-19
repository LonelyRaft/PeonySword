
#include "keyboard.h"
#include "event.h"
#include "actor.h"
#include <list>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>
#include <climits>

#include <windows.h>

#include <iostream>

namespace PeonySword {
    class Keyboard {
        // key data
        static const int sDataSize = CHAR_MAX + 1;
        static bool sRunning;
        static std::mutex sDataLock;
        static bool sData[sDataSize];
        // key event queue
        // there is no race condition on the queue
        static std::queue<KeyEvent> sEventQueue;
        // receiver list
        static std::mutex sRecverLock;
        static std::list<Actor *> sRecverList;
        // event listen thread
        static std::thread *sListenThread;
        // event process thread
        static std::thread *sProcessThread;

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
    };

    std::once_flag Keyboard::sOnceFlag;
    bool Keyboard::sRunning = true;
    std::mutex Keyboard::sDataLock;
    std::mutex Keyboard::sRecverLock;

    bool Keyboard::sData[CHAR_MAX + 1];
    std::queue<KeyEvent> Keyboard::sEventQueue;
    std::list<Actor *> Keyboard::sRecverList;
    std::thread *Keyboard::sListenThread = nullptr;
    std::thread *Keyboard::sProcessThread = nullptr;

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
                sData[keyEvent.mKeyValue] = true;
            }
        } else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
            keyEvent.mKeyState = KeyStateUp;
            {
                std::lock_guard lock(sDataLock);
                sData[keyEvent.mKeyValue] = false;
            }
        }
        if (keyEvent.mKeyState != KeyStateInvalid) {
            sEventQueue.emplace(keyEvent);
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
        using namespace std::chrono;
        using namespace std::this_thread;
        while (sRunning) {
            // process press and release events
            while (!sEventQueue.empty()) {
                const KeyEvent &e = sEventQueue.front();
                dispatchEvent(e);
                sEventQueue.pop();
            }
            sleep_for(milliseconds(10));
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

    static Keyboard sKBData;

    void subscribeKeyEvent(Actor *_actor) {
        Keyboard::subscribeKeyEvent(_actor);
    }

    void unsubscribeKeyEvent(Actor *_actor) {
        Keyboard::unsubscribeKeyEvent(_actor);
    }
}
