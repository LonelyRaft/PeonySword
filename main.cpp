
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <functional>
#include "actor.h"
#include "timer.h"

void callFunction(void(*_func)(void *), void *_obj, void *arg) {

}

class Ademo {
    PeonySword::Timer *timer = nullptr;
    int x = 0;
    int y = 0;
public:
    Ademo();

    ~Ademo();

    void callback(void *);
};

void Ademo::callback(void *) {
    std::cout << x << " " << y << std::endl;
}

Ademo::Ademo() {
    timer = new PeonySword::Timer;
    timer->setPeriod(10000);
    timer->addRoutine(
            "callback",
            [this](void *arg) {
                this->callback(arg);
            });
    timer->start();
}

Ademo::~Ademo() {
    delete timer;
}


static void sayHello(void *) {
    using namespace std::chrono;
    duration curr = system_clock::now().time_since_epoch();
    milliseconds stamp = duration_cast<milliseconds>(curr);
    std::cout << "Hello: " << stamp.count() << std::endl;
}

int main(int argc, char *argv[]) {
    Ademo obj;
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
