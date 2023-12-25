
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

int main(int argc, char *argv[]) {
    using namespace PeonySword;
    setKeepEnable(KeyValueLeftShift, false);
    setKeepEnable(KeyValueLeftCtrl, false);
    setKeepEnable(KeyValueLeftAlt, false);
    setKeepEnable(KeyValueRightShift, false);
    setKeepEnable(KeyValueRightCtrl, false);
    setKeepEnable(KeyValueRightAlt, false);
    Timer *timer = new Timer(5000);
    timer->addRoutine("sayHello", sayHello, nullptr);
    timer->start();
    Actor *actor = new Actor;
    actor->changeName("DemoName");
    int idx = 100;
    while (idx-- > 0) {
        std::this_thread::sleep_for(
                std::chrono::milliseconds(100));
    }
    timer->stop();
    delete timer;
    timer = nullptr;
    return 0;
}
