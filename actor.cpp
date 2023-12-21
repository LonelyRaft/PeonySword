
#include "actor.h"
#include "actor_data.h"
#include "keyboard.h"
#include <iostream>

namespace PeonySword {


    Actor::Actor() {
        subscribeKeyEvent(this);
        pd = new ActorData;
    }

    Actor::Actor(const std::string &_name) {
        subscribeKeyEvent(this);
        pd = new ActorData(_name);
    }

    Actor::~Actor() {
        unsubscribeKeyEvent(this);
        if (pd != nullptr) {
            delete pd;
            pd = nullptr;
        }
    }

    void Actor::moveTo(double _x, double _y, double _z) {
        pd->mPos.moveTo(_x, _y, _z);
    }

    void Actor::move(double _x, double _y, double _z) {
        pd->mPos.move(_x, _y, _z);
    }

    void Actor::showInfo() {
        std::cout << "Actor:" << *pd << std::endl;
    }

    void Actor::changeName(const std::string &_newName) {
        pd->mName = _newName;
    }

    std::string Actor::actorName() const {
        return pd->mName;
    }

    void Actor::keyReleaseEvent(const KeyEvent &e) {
        pd->keyReleaseEvent(e);
    }

    void Actor::keyPressEvent(const KeyEvent &e) {
        pd->keyPressEvent(e);
    }
}
