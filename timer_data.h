#ifndef PEONYSWORD_TIMER_DATA_H
#define PEONYSWORD_TIMER_DATA_H

#include <functional>
#include <utility>
#include <string>

namespace PeonySword {
    class TimeoutRoutine {
    public:
        std::string mName;

        std::function<void(void *)> mRoutine;

        void *mParam = nullptr;

        TimeoutRoutine() = default;

        TimeoutRoutine(
                const std::string &_name,
                const std::function<void(void *)> &_routine,
                void *_param) :
                mName(_name), mRoutine(_routine), mParam(_param) {}
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

        virtual void addRoutine(
                const std::string &_name,
                const std::function<void(void *)> &_func,
                void *arg) = 0;

        virtual void rmRoutine(const std::string &_name) = 0;
    };

    extern TimerData *createTimerData();
}

#endif //PEONYSWORD_TIMER_DATA_H
