#ifndef PEONYSWORD_TIMER_DATA_H
#define PEONYSWORD_TIMER_DATA_H

#include <functional>
#include <utility>

namespace PeonySword {
    class TimeoutRoutine {
    public:

        std::function<void(void *)> mRoutine;

        void *mParam = nullptr;

        TimeoutRoutine() = default;

        TimeoutRoutine(const std::function<void(void *)> &_routine, void *_param) :
                mRoutine(_routine), mParam(_param) {}
    };

    class TimerData {
    public:
        TimerData() = default;

        TimerData(const TimerData &) = delete;

        TimerData(TimerData &&) = delete;

        TimerData &operator=(const TimerData &) = delete;

        TimerData &operator=(TimerData &&) = delete;

        virtual ~TimerData() = default;

        virtual int start() = 0;

        virtual int stop() = 0;

        virtual bool isActive() = 0;

        virtual void setPeriod(unsigned int _milli_secs) = 0;

        virtual void addRoutine(const std::function<void(void *)> &, void *arg) = 0;
    };

    extern TimerData *createTimerData();
}

#endif //PEONYSWORD_TIMER_DATA_H
