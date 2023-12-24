#ifndef PEONYSWORD_TIMER_H
#define PEONYSWORD_TIMER_H

#include <functional>

namespace PeonySword {
    class TimerData;

    class Timer {
        TimerData *pd;
    public:
        Timer();

        explicit Timer(unsigned int _milli_secs);

        ~Timer();

        Timer(const Timer &) = delete;

        Timer(Timer &&) = delete;

        Timer &operator=(const Timer &) = delete;

        Timer &operator=(Timer &&) = delete;

        int start();

        int stop();

        bool isActive();

        void setPeriod(unsigned int _milli_secs);

        void addRoutine(const std::function<void(void *)>&, void *);
    };
}

#endif //PEONYSWORD_TIMER_H
