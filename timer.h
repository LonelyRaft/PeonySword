#ifndef PEONYSWORD_TIMER_H
#define PEONYSWORD_TIMER_H

#include <functional>
#include <string>

namespace PeonySword {
    class TimerData;

    class Timer {
        TimerData *pd = nullptr;
    public:
        Timer();

        explicit Timer(unsigned int _milli_secs);

        ~Timer();

        Timer(const Timer &) = delete;

        Timer(Timer &&) = delete;

        Timer &operator=(const Timer &) = delete;

        Timer &operator=(Timer &&) = delete;

        void setOnceFlag(bool _flag);

        [[nodiscard]] bool onceFlag() const;

        void setPeriod(unsigned int _milli_secs);

        void addRoutine(
                const std::string &_name,
                const std::function<void(void *)> &_func,
                void *_arg = nullptr);

        void rmRoutine(const std::string &_name);

        int start();

        int stop();

        [[nodiscard]] bool isActive() const;
    };
}

#endif //PEONYSWORD_TIMER_H
