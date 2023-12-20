#ifdef __linux__

#include "keyboard.h"
#include "keybaord_data.h"
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>

namespace PeonySword {

    class KeyboardLinux : public Keyboard {
    public:
        static bool readline(int _fd, std::string &_line);

        static int scan_keyboard(std::string &_event);
    };

    bool KeyboardLinux::readline(int _fd, std::string &_line) {
        std::string line;
        char buf[4] = {0};
        bool ret = true;
        while (buf[0] != '\n') {
            line.append(buf);
            if (read(_fd, buf, 1) <= 0) {
                ret = false;
                break;
            }
        }
        _line = std::move(line);
        return ret;
    }

    int KeyboardLinux::scan_keyboard(std::string &_event) {
        static const char *dev_path =
                "/proc/bus/input/devices";
        int result = -1;
        int input_dev = open(dev_path, O_RDONLY);
        if (input_dev < 0) {
            perror(dev_path);
            return result;
        }
        char flag = 'N';
        std::string line;
        while (readline(input_dev, line)) {
            if (line.empty() ||
                line.at(0) != flag) {
                continue;
            }
            if ('N' == flag) {
                if (line.find("keyboard") < line.length()) {
                    flag = 'H';
                }
            } else {
                size_t start = line.find("event");
                if (start >= line.length()) {
                    continue;
                }
                size_t end = line.find(' ', start);
                if (end > line.length()) {
                    end = line.length();
                }
                _event = std::move(line.substr(start, end - start));
                result = 0;
                break;
            }
        }
        close(input_dev);
        return result;
    }

    void Keyboard::listenThreadEntry() {
        static const char *kb_event_dir =
                "/dev/input/";
        // find keyboard event path
        std::string kb_event_name;
        if (KeyboardLinux::scan_keyboard(kb_event_name)) {
            return;
        }
        std::string kb_event(kb_event_dir);
        kb_event.append(kb_event_name);
        // open kb event
        int fd = open(kb_event.c_str(), O_RDONLY);
        if (fd < 0) {
            perror(kb_event.c_str());
            return;
        }
        // read event
        while (sRunning) {
            input_event event = {0};
            if (sizeof(input_event) ==
                read(fd, &event, sizeof(input_event)) &&
                event.type == EV_KEY) {
                KeyEvent keyEvent;
                keyEvent.mKeyValue = toKeyValue(event.code);
                if (event.value == 0) {
                    keyEvent.mKeyState = KeyStateUp;
                    std::lock_guard lock(sDataLock);
                    sData[keyEvent.mKeyValue].mKeeping = false;
                } else {
                    keyEvent.mKeyState = KeyStateDown;
                    std::lock_guard lock(sDataLock);
                    if (!sData[keyEvent.mKeyValue].mKeepEnable &&
                        sData[keyEvent.mKeyValue].mKeeping) {
                        continue;
                    }
                    sData[keyEvent.mKeyValue].mKeeping = true;
                }
                if (keyEvent.mKeyState != KeyStateInvalid) {
                    sEventQueue.emplace(keyEvent);
                    sEventCond.notify_one();
                }
            }
        }
        // close kb event
        close(fd);
    }

    const KeyValuePair Keyboard::KeyValueTable[] = {
            {1,   KeyValueEscape},
            {2,   KeyValue1},
            {3,   KeyValue2},
            {4,   KeyValue3},
            {5,   KeyValue4},
            {6,   KeyValue5},
            {7,   KeyValue6},
            {8,   KeyValue7},
            {9,   KeyValue8},
            {10,  KeyValue9},
            {11,  KeyValue0},
            {12,  KeyValueSub},
            {13,  KeyValueEqual},
            {14,  KeyValueBack},
            {15,  KeyValueTab},
            {16,  KeyValueQ},
            {17,  KeyValueW},
            {18,  KeyValueE},
            {19,  KeyValueR},
            {20,  KeyValueT},
            {21,  KeyValueY},
            {22,  KeyValueU},
            {23,  KeyValueI},
            {24,  KeyValueO},
            {25,  KeyValueP},
            {26,  KeyValueLeftBracket},
            {27,  KeyValueRightBracket},
            {28,  KeyValueEnter},
            {29,  KeyValueLeftCtrl},
            {30,  KeyValueA},
            {31,  KeyValueS},
            {32,  KeyValueD},
            {33,  KeyValueF},
            {34,  KeyValueG},
            {35,  KeyValueH},
            {36,  KeyValueJ},
            {37,  KeyValueK},
            {38,  KeyValueL},
            {39,  KeyValueSemiconlon},
            {40,  KeyValueQuote},
            {41,  KeyValueTilde},
            {42,  KeyValueLeftShift},
            {43,  KeyValueBackslash},
            {44,  KeyValueZ},
            {45,  KeyValueX},
            {46,  KeyValueC},
            {47,  KeyValueV},
            {48,  KeyValueB},
            {49,  KeyValueN},
            {50,  KeyValueM},
            {51,  KeyValueComma},
            {52,  KeyValuePeriod},
            {53,  KeyValueSlash},
            {54,  KeyValueRightShift},
            {56,  KeyValueLeftAlt},
            {57,  KeyValueSpace},
            {58,  KeyValueCaps},
            {59,  KeyValueF1},
            {60,  KeyValueF2},
            {61,  KeyValueF3},
            {62,  KeyValueF4},
            {63,  KeyValueF5},
            {64,  KeyValueF6},
            {65,  KeyValueF7},
            {66,  KeyValueF8},
            {67,  KeyValueF9},
            {68,  KeyValueF10},
            {70,  KeyValueScrollLock},
            {87,  KeyValueF11},
            {88,  KeyValueF12},
            {97,  KeyValueRightCtrl},
            {99,  KeyValuePrtScreen},
            {100, KeyValueRightAlt},
            {102, KeyValueHome},
            {103, KeyValueUp},
            {104, KeyValuePgUp},
            {105, KeyValueLeft},
            {106, KeyValueRight},
            {107, KeyValueEnd},
            {108, KeyValueDown},
            {109, KeyValuePgDown},
            {110, KeyValueInsert},
            {111, KeyValueDelete},
            {119, KeyValuePause},
            {125, KeyValueStart},
            {127, KeyValueMenu},
    };
    const int Keyboard::KeyValueTableSize =
            sizeof(KeyValueTable) / sizeof(KeyValuePair);
}

#endif // __linux__
