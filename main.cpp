
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <windows.h>
#include "event.h"
#include "actor.h"

int main(int argc, char *argv[]) {
    using namespace PeonySword;
    Actor *actor = new Actor;
    actor->changeName("DemoName");
    while (true) {
        std::this_thread::sleep_for(
                std::chrono::milliseconds(100));
    }
    return 0;
}
