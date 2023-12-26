
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

    bool Timer::isActive() const {
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

    void Timer::setOnceFlag(bool _flag) {
        pd->setOnceFlag(_flag);
    }

    bool Timer::onceFlag() const {
        return pd->onceFlag();
    }

    void TimerData::setOnceFlag(bool _flag) {
        mOnce = _flag;
    }

    bool TimerData::onceFlag() const {
        return mOnce;
    }

    void TimerData::setPeriod(
            unsigned int _milli_secs) {
        if (_milli_secs == 0) {
            mPeriod = DefaultPeriod;
            return;
        }
        mPeriod = _milli_secs;
    }

    void TimerData::addRoutine(
            const std::string &_name,
            const std::function<void(void *)> &_routine,
            void *_arg) {
        if (_name.empty() ||
            !_routine.operator bool()) {
            return;
        }
        TimeoutRoutine r(_name, _routine, _arg);
        mRoutines.emplace_back(std::move(r));
    }

    void TimerData::rmRoutine(const std::string &_name) {
        if (_name.empty()) {
            return;
        }
        auto it = mRoutines.cbegin();
        while (it != mRoutines.cend()) {
            if (it->mName == _name) {
                mRoutines.erase(it);
                continue;
            }
            ++it;
        }
    }
}
