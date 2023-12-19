
#include "actor.h"
#include "position.h"
#include "keyboard.h"
#include <iostream>
#include <string>
#include <mutex>

namespace PeonySword {
    class ActorData {
        friend class Actor;

        int mID = 0;
        Position mPos;
        std::string mName = "default";
        static std::mutex sMutID;
        static int sActorCounter;

        static int allocID();

    public:
        explicit ActorData();

        explicit ActorData(const std::string &_name);

        virtual ~ActorData() = default;

        friend std::ostream &operator<<(std::ostream &, const ActorData &);
    };

    ActorData::ActorData() {
        mID = allocID();
    }

    ActorData::ActorData(const std::string &_name) :
            mName(_name) {
        mID = allocID();
    }

    std::ostream &operator<<(std::ostream &out, const ActorData &_data) {
        out << R"({"id":)" << _data.mID
            << R"(, "name":")" << _data.mName
            << R"(", position:[)"
            << _data.mPos.mPosX << ", "
            << _data.mPos.mPosY << ", "
            << _data.mPos.mPosZ << "]}";
        return out;
    }

    std::mutex ActorData::sMutID;
    int ActorData::sActorCounter;

    int ActorData::allocID() {
        std::lock_guard lock(sMutID);
        ++sActorCounter;
        return sActorCounter;
    }

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
        std::cout << __FUNCTION__ << std::endl;
    }

    void Actor::keyPressEvent(const KeyEvent &e) {
        std::cout << __FUNCTION__ << std::endl;
    }
}
