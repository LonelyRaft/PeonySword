
#ifndef PEONYSWORD_KEYBOARD_H
#define PEONYSWORD_KEYBOARD_H

#include "actor.h"

namespace PeonySword {
#ifdef __cplusplus
    extern "C" {
#endif

    void subscribeKeyEvent(Actor *_actor);

    void unsubscribeKeyEvent(Actor *_actor);

#ifdef __cplusplus
    }
#endif
}

#endif //PEONYSWORD_KEYBOARD_H
