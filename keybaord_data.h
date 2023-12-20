#ifndef PEONYSWORD_KEYBAORD_DATA_H
#define PEONYSWORD_KEYBAORD_DATA_H

#include <climits>
#include <list>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>

namespace PeonySword {
    class KeyData {
    public:
        bool mKeepEnable = true;
        bool mKeeping = false;
    };

    class KeyValuePair {
    public:
        unsigned char key = 0;
        KeyValue value = KeyValueInvalid;

        KeyValuePair() = default;
    };

    class Keyboard {
    protected:
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
}

#endif //PEONYSWORD_KEYBAORD_DATA_H
