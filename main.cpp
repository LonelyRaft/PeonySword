
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <functional>
#include "actor.h"
#include "timer.h"
#include <csignal>
#include <ctime>

static void sayHello(void *) {
    using namespace std::chrono;
    duration curr = system_clock::now().time_since_epoch();
    milliseconds stamp = duration_cast<milliseconds>(curr);
    std::cout << "Hello: " << stamp.count() << std::endl;
}

static void sayHello1(sigval_t arg) {
//    arg.sival_ptr;
    using namespace std::chrono;
    duration curr = system_clock::now().time_since_epoch();
    milliseconds stamp = duration_cast<milliseconds>(curr);
    std::cout << "Hello: " << stamp.count() << std::endl;
}

int main(int argc, char *argv[]) {
    sigevent se = {0};
    se.sigev_notify = SIGEV_THREAD;
    se.sigev_notify_function = sayHello1;
    se.sigev_value.sival_ptr = nullptr;
    timer_t timer1;
    timer_create(CLOCK_REALTIME, &se, &timer1);
    itimerspec its;
    its.it_interval.tv_sec = 5;
    its.it_interval.tv_nsec = 0;
    its.it_value.tv_sec = 5;
    its.it_value.tv_nsec = 0;
    timer_settime(timer1, TIMER_ABSTIME, &its, nullptr);


    using namespace PeonySword;
    setKeepEnable(KeyValueLeftShift, false);
    setKeepEnable(KeyValueLeftCtrl, false);
    setKeepEnable(KeyValueLeftAlt, false);
    setKeepEnable(KeyValueRightShift, false);
    setKeepEnable(KeyValueRightCtrl, false);
    setKeepEnable(KeyValueRightAlt, false);
//    Timer *timer = new Timer(5000);
//    timer->addRoutine("sayHello", sayHello, nullptr);
//    timer->start();
    Actor *actor = new Actor;
    actor->changeName("DemoName");
    int idx = 10000;
    while (idx-- > 0) {
        std::this_thread::sleep_for(
                std::chrono::milliseconds(100));
    }
//    timer->stop();
//    delete timer;
//    timer = nullptr;
    return 0;
}
