#ifndef PEONYSWORD_TIMER_DATA_H
#define PEONYSWORD_TIMER_DATA_H

namespace PeonySword {
    class TimeoutRoutine {
    public:
        void (*mRoutine)(void *) = nullptr;

        void *mParam = nullptr;

        TimeoutRoutine() = default;

        TimeoutRoutine(void (*_routine)(void *), void *_param) :
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

        virtual void addRoutine(void(*_timeout)(void *), void *_arg) = 0;
    };

    extern TimerData *createTimerData();
}

#endif //PEONYSWORD_TIMER_DATA_H
