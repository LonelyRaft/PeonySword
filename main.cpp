
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <functional>
#include "actor.h"
#include "timer.h"
#include "clog.h"
#include <csignal>
#include <ctime>

clog_t *g_log = nullptr;

static void sayHello(void *) {
    using namespace std::chrono;
    duration curr = system_clock::now().time_since_epoch();
    milliseconds stamp = duration_cast<milliseconds>(curr);
    std::cout << "Hello: " << stamp.count() << std::endl;
    if (g_log == nullptr) {
        g_log = clog_create(2048);
        clog_use_datetime(g_log, 1);
        clog_use_level(g_log, 1);
        clog_use_name(g_log, 1);
        clog_use_function(g_log, 1);
        clog_use_position(g_log, 1);
        clog_use_stdout(g_log, 1);
        clog_set_dir(g_log, "./");
        clog_set_name(g_log, "PeonySword");
    }
    clog_debug_(g_log, __FILE__, __LINE__, __FUNCTION__, "Log Test Message: %d", stamp.count());
    clog_info_(g_log, __FILE__, __LINE__, __FUNCTION__, "Log Test Message: %d", stamp.count());
    clog_warn_(g_log, __FILE__, __LINE__, __FUNCTION__, "Log Test Message: %d", stamp.count());
    clog_error_(g_log, __FILE__, __LINE__, __FUNCTION__, "Log Test Message: %d", stamp.count());
}

int main(int argc, char *argv[]) {
    using namespace PeonySword;
    setKeepEnable(KeyValueLeftShift, false);
    setKeepEnable(KeyValueLeftCtrl, false);
    setKeepEnable(KeyValueLeftAlt, false);
    setKeepEnable(KeyValueRightShift, false);
    setKeepEnable(KeyValueRightCtrl, false);
    setKeepEnable(KeyValueRightAlt, false);
    Timer *timer = new Timer(1000);
    timer->addRoutine("sayHello", sayHello, nullptr);
    timer->start();
    Actor *actor = new Actor;
    actor->changeName("DemoName");
    int idx = 1000;
    while (idx-- > 0) {
        std::this_thread::sleep_for(
                std::chrono::milliseconds(100));
    }
    timer->stop();
    delete timer;
    timer = nullptr;
    return 0;
}
