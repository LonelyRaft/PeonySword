
#ifdef _WIN32

#include "timer_data.h"
#include <list>
#include <windows.h>

namespace PeonySword {
    class TimerQueueWin {
    public:
        HANDLE mQueue = nullptr;

        TimerQueueWin() {
            mQueue = CreateTimerQueue();
        }

        ~TimerQueueWin() {
            if (mQueue != nullptr) {
                DeleteTimerQueue(mQueue);
                mQueue = nullptr;
            }
        }
    };

    class TimerDataWin : public TimerData {
    protected:
        static const unsigned int DefaultPeriod = 1000;
        static TimerQueueWin TimerQueue;

        static VOID CALLBACK TimeoutRoutineExec(PVOID, BOOLEAN);

        HANDLE mTimer = nullptr;
        unsigned int mPeriod = DefaultPeriod;
        std::list<TimeoutRoutine> mRoutines;
    public:
        TimerDataWin() = default;

        ~TimerDataWin() override;

        TimerDataWin(const TimerDataWin &) = delete;

        TimerDataWin(TimerDataWin &&) = delete;

        TimerDataWin &operator=(const TimerDataWin &) = delete;

        TimerDataWin &operator=(TimerDataWin &&) = delete;

        int start() override;

        int stop() override;

        bool isActive() override;

        void setPeriod(unsigned int _period) override;

        void rmRoutine(const std::string &_name) override;

        void addRoutine(
                const std::string &_name,
                const std::function<void(void *)> &_routine,
                void *_arg) override;
    };

    int TimerDataWin::start() {
        if (mTimer != nullptr) {
            return 0;
        }
        WINBOOL ret = CreateTimerQueueTimer(
                &mTimer, TimerQueue.mQueue, TimeoutRoutineExec,
                this, mPeriod, mPeriod, WT_EXECUTEDEFAULT);
        if (ret == FALSE) {
            mTimer = nullptr;
            return (int) GetLastError();
        }
        return 0;
    }

    int TimerDataWin::stop() {
        if (mTimer == nullptr) {
            return 0;
        }
        HANDLE waitEvent = CreateEvent(
                nullptr, false, false, nullptr);
        if (!DeleteTimerQueueTimer(
                TimerQueue.mQueue, mTimer, waitEvent)) {
            CloseHandle(waitEvent);
            return (int) GetLastError();
        }
        if (WaitForSingleObject(waitEvent, INFINITE)) {
            CloseHandle(waitEvent);
            return (int) GetLastError();
        }
        CloseHandle(waitEvent);
        mTimer = nullptr;
        return 0;
    }

    bool TimerDataWin::isActive() {
        return mTimer != nullptr;
    }

    void TimerDataWin::setPeriod(unsigned int _period) {
        mPeriod = _period;
    }

    void TimerDataWin::addRoutine(
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

    TimerQueueWin TimerDataWin::TimerQueue;

    VOID CALLBACK TimerDataWin::TimeoutRoutineExec(
            PVOID _param, BOOLEAN) {
        TimerDataWin *timer =
                static_cast<TimerDataWin *>(_param);
        if (timer == nullptr) {
            return;
        }
        auto it = timer->mRoutines.cbegin();
        while (it != timer->mRoutines.cend()) {
            it->mRoutine(it->mParam);
            ++it;
        }
    }

    TimerDataWin::~TimerDataWin() {
        if (mTimer == nullptr) {
            return;
        }
        HANDLE waitEvent = CreateEvent(
                nullptr, false, false, nullptr);
        if (!DeleteTimerQueueTimer(
                TimerQueue.mQueue, mTimer, waitEvent)) {
            CloseHandle(waitEvent);
            return;
        }
        if (WaitForSingleObject(waitEvent, INFINITE)) {
            CloseHandle(waitEvent);
            return;
        }
        CloseHandle(waitEvent);
        mTimer = nullptr;
    }

    void TimerDataWin::rmRoutine(const std::string &_name) {
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

    TimerData *createTimerData() {
        return new TimerDataWin;
    }
}

#endif // _WIN32
