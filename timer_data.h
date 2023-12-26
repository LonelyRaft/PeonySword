#ifndef PEONYSWORD_TIMER_DATA_H
#define PEONYSWORD_TIMER_DATA_H

#include <functional>
#include <utility>
#include <string>
#include <list>

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
    protected:
        static const unsigned int DefaultPeriod = 1000;
        bool mOnce = false;
        size_t mPeriod = 1000;
        std::list<TimeoutRoutine> mRoutines;
    public:
        TimerData() = default;

        TimerData(const TimerData &) = delete;

        TimerData(TimerData &&) = delete;

        TimerData &operator=(const TimerData &) = delete;

        TimerData &operator=(TimerData &&) = delete;

        virtual ~TimerData() = default;

        void setOnceFlag(bool _flag);

        [[nodiscard]] bool onceFlag() const;

        void setPeriod(unsigned int _milli_secs);

        virtual int start() = 0;

        virtual int stop() = 0;

        [[nodiscard]] virtual bool isActive() const = 0;

        void addRoutine(
                const std::string &_name,
                const std::function<void(void *)> &_func,
                void *arg);

        void rmRoutine(const std::string &_name);
    };

    extern TimerData *createTimerData();
}

#endif //PEONYSWORD_TIMER_DATA_H
