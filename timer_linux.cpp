
#ifdef __linux__

#include "timer.h"
#include "timer_data.h"
#include <list>
#include <csignal>
#include <ctime>


namespace PeonySword {
    class TimerSignalLinux {
    public:
        sigevent mSigEvent;
        TimerSignalLinux() {

        }

        ~TimerSignalLinux() = default;
    };
    class TimerDataLinux
            : public TimerData {
        static TimerSignalLinux mSigEvent;
        static void processRoutines(sigval);

        bool mRunning = false;
        timer_t mTimer = nullptr;

    public:

        TimerDataLinux();

        ~TimerDataLinux() override;

        int start() override;

        int stop() override;

        [[nodiscard]] bool isActive() const override;
    };

    int TimerDataLinux::start() {
        if(mOnce) {
            mPeriod.it_interval.tv_sec = 0;
            mPeriod.it_interval.tv_nsec = 0;
        }
        int ret = timer_settime(
                mTimer, TIMER_ABSTIME, &mPeriod, nullptr);
        if(ret < 0) {
            ret = errno;
        }
        return ret;
    }

    int TimerDataLinux::stop() {
        if(mTimer == nullptr) {
            return 0;
        }
        itimespec stop_its;
        int ret = timer_settime(
                mTimer, TIMER_ABSTIME, &stop_its, nullptr);
        if(ret < 0){
            ret = errno;
        }
        return ret;
    }

    bool TimerDataLinux::isActive() const {
        return mRunning;
    }

    void TimerDataLinux::setPeriod(
            unsigned int _milli_secs) {
        mPeriod.it_value.tv_sec = _milli_secs / 1000;
        _milli_secs %= 1000;
        mPeriod.it_value.tv_nsec = _milli_secs * 1000;
        mPeriod.it_interval = mPeriod.it_value;
    }

    TimerDataLinux::TimerDataLinux() {
        mSigEvent.sigev_notify = SIGEV_THREAD;
        mSigEvent.sigev_notify_function =
                TimerDataLinux::processRoutines;
        mSigEvent.sigev_value.sival_ptr = this;
        if (timer_create(CLOCK_REALTIME, &mSigEvent, &mTimer)) {
            // log errno
        }
    }

    TimerDataLinux::~TimerDataLinux() {
        if (mTimer == nullptr) {
            return;
        }
        if (timer_delete(mTimer)) {
            // log errno
        }
        mTimer = nullptr;
        mRunning = false;
    }

    void TimerDataLinux::processRoutines(sigval arg) {
        auto *timer =
                static_cast<TimerDataLinux *>(arg.sival_ptr);
        if (timer == nullptr) {
            return;
        }
        auto it = timer->mRoutines.cbegin();
        while (it != timer->mRoutines.cend()) {
            it->mRoutine(it->mParam);
            ++it;
        }
    }

    TimerSignalLinux TimerDataLinux::mSigEvent;

    TimerData *createTimerData() {
        return new TimerDataLinux;
    }
}

#endif // __linux__
