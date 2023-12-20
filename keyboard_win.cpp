#ifdef _WIN32

#include "keyboard.h"
#include "keybaord_data.h"
#include <windows.h>

namespace PeonySword {
    class KeyboardWin : public Keyboard {
    public:
        static LRESULT CALLBACK cbKeyboard(
                int, WPARAM, LPARAM);
    };

    LRESULT CALLBACK KeyboardWin::cbKeyboard(
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

    void Keyboard::listenThreadEntry() {
        HHOOK hook = SetWindowsHookEx(
                WH_KEYBOARD_LL, KeyboardWin::cbKeyboard, nullptr, 0);
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
