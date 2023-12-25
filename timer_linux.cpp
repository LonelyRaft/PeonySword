
#ifdef __linux__

#include "timer.h"
#include "timer_data.h"
#include <list>
#include <csignal>
#include <ctime>


namespace PeonySword {
    class TimerDataLinux
            : public TimerData {
        timer_t mTimer = nullptr;
        sigevent mSigEvent;
        itimerspec mPeriod =
                {{0, 0},
                 {0, 0}
                };
        std::list<TimeoutRoutine> mRoutines;

        static void processRoutines(sigval);

    public:

        TimerDataLinux();

        ~TimerDataLinux() override;

        int start() override;

        int stop() override;

        bool isActive() override;

        void setPeriod(unsigned int _milli_secs) override;

        void addRoutine(
                const std::string &_name,
                const std::function<void(void *)> &_func,
                void *arg) override;

        void rmRoutine(const std::string &_name) override;
    };

    int TimerDataLinux::start() {

        return 0;
    }

    int TimerDataLinux::stop() {
        return 0;
    }

    bool TimerDataLinux::isActive() {

        return false;
    }

    void TimerDataLinux::setPeriod(
            unsigned int _milli_secs) {
        mPeriod.it_value.tv_sec = _milli_secs / 1000;
        _milli_secs %= 1000;
        mPeriod.it_value.tv_nsec = _milli_secs * 1000;
        mPeriod.it_interval = mPeriod.it_value;
    }

    void TimerDataLinux::addRoutine(
            const std::string &_name,
            const std::function<void(void *)> &_func,
            void *arg) {
        if (_name.empty() ||
            !_func.operator bool()) {
            return;
        }
        TimeoutRoutine r(_name, _func, arg);
        mRoutines.emplace_back(std::move(r));
    }

    void TimerDataLinux::rmRoutine(const std::string &_name) {
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
    }

    void TimerDataLinux::processRoutines(sigval arg) {
        TimerDataLinux *timer =
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

    TimerData *createTimerData() {
        return new TimerDataLinux;
    }
}

#endif // __linux__
