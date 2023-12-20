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
                switch (event.value) {
                    case 0: {
                        keyEvent.mKeyState = KeyStateUp;
                        break;
                    }
                    case 1: {
                        keyEvent.mKeyState = KeyStateDown;
                        break;
                    }
                    case 2: {
                        keyEvent.mKeyState = KeyStateDown;
                        {

                        }
                        break;
                    }
                    default: {
                        break;
                    }
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
            {8, KeyValueBack},
    };
    const int Keyboard::KeyValueTableSize =
            sizeof(KeyValueTable) / sizeof(KeyValuePair);
}

#endif // __linux__
