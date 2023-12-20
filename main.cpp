
#include <string>
#include <thread>
#include <chrono>
#include "actor.h"

int main(int argc, char *argv[]) {
    using namespace PeonySword;
    setKeepEnable(KeyValueLeftShift, false);
    setKeepEnable(KeyValueLeftCtrl, false);
    setKeepEnable(KeyValueLeftAlt, false);
    setKeepEnable(KeyValueRightShift, false);
    setKeepEnable(KeyValueRightCtrl, false);
    setKeepEnable(KeyValueRightAlt, false);
    Actor *actor = new Actor;
    actor->changeName("DemoName");
    while (true) {
        std::this_thread::sleep_for(
                std::chrono::milliseconds(100));
    }
    return 0;
}
