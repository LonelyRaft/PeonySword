
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
        static TimerQueueWin TimerQueue;

        static VOID CALLBACK TimeoutRoutineExec(PVOID, BOOLEAN);

        HANDLE mTimer = nullptr;
    public:
        TimerDataWin() = default;

        ~TimerDataWin() override;

        TimerDataWin(const TimerDataWin &) = delete;

        TimerDataWin(TimerDataWin &&) = delete;

        TimerDataWin &operator=(const TimerDataWin &) = delete;

        TimerDataWin &operator=(TimerDataWin &&) = delete;

        int start() override;

        int stop() override;

        [[nodiscard]] bool isActive() const override;
    };

    int TimerDataWin::start() {
        if (mTimer != nullptr) {
            return -1;
        }
        DWORD DueTime = mPeriod;
        DWORD Period = mPeriod;
        if (mOnce) {
            Period = 0;
        }
        WINBOOL ret = CreateTimerQueueTimer(
                &mTimer, TimerQueue.mQueue, TimeoutRoutineExec,
                this, DueTime, Period, WT_EXECUTEDEFAULT);
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

    bool TimerDataWin::isActive() const {
        return mTimer != nullptr;
    }

    TimerQueueWin TimerDataWin::TimerQueue;

    VOID CALLBACK TimerDataWin::TimeoutRoutineExec(
            PVOID _param, BOOLEAN) {
        auto *timer =
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

    TimerData *createTimerData() {
        return new TimerDataWin;
    }
}

#endif // _WIN32
