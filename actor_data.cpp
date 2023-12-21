
#include "actor_data.h"

namespace PeonySword {
    ActorData::ActorData() {
        mID = allocID();
    }

    ActorData::ActorData(const std::string &_name) :
            mName(_name) {
        mID = allocID();
    }

    void ActorData::keyReleaseEvent(const KeyEvent &e) {
        if (e.mKeyState == KeyStateKeep) {
            return;
        }
        std::cout << __FUNCTION__ << "\t"
                  << (int) e.mKeyValue << std::endl;
    }

    void ActorData::keyPressEvent(const KeyEvent &e) {
        std::cout << __FUNCTION__ << "\t"
                  << (int) e.mKeyValue << std::endl;
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
}
