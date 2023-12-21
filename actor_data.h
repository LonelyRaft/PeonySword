#ifndef PEONYSWORD_ACTOR_DATA_H
#define PEONYSWORD_ACTOR_DATA_H

#include "position.h"
#include "keyboard.h"
#include <string>
#include <mutex>
#include <iostream>

namespace PeonySword
{
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

        virtual void keyReleaseEvent(const KeyEvent &);

        virtual void keyPressEvent(const KeyEvent &);

        friend std::ostream &operator<<(std::ostream &, const ActorData &);
    };
}

#endif //PEONYSWORD_ACTOR_DATA_H
