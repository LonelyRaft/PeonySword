
#include "timer.h"
#include "timer_data.h"

namespace PeonySword {
    Timer::Timer() {
        pd = createTimerData();
    }

    Timer::Timer(unsigned int _milli_secs) : Timer() {
        pd->setPeriod(_milli_secs);
    }

    Timer::~Timer() {
        delete pd;
        pd = nullptr;
    }

    int Timer::start() {
        return pd->start();
    }

    int Timer::stop() {
        return pd->stop();
    }

    bool Timer::isActive() {
        return pd->isActive();
    }

    void Timer::setPeriod(unsigned int _milli_secs) {
        pd->setPeriod(_milli_secs);
    }

    void Timer::addRoutine(
            const std::string &_name,
            const std::function<void(void *)> &_func, void *_arg) {
        pd->addRoutine(_name, _func, _arg);
    }

    void Timer::rmRoutine(const std::string &_name) {
        pd->rmRoutine(_name);
    }
}
