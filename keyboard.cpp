
#include "keyboard.h"
#include "keybaord_data.h"
#include "actor.h"

namespace PeonySword {
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
}