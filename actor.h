#ifndef PEONYSWORD_ACTOR_H
#define PEONYSWORD_ACTOR_H

#include <string>
#include "event.h"


namespace PeonySword {
    class ActorData;

    class Actor {
        ActorData *pd;
    public:

        explicit Actor(const std::string &_name);

        explicit Actor();

        virtual ~Actor();

        void moveTo(double _x, double _y, double _z = 0);

        void move(double _x, double _y, double _z = 0);

        virtual void showInfo();

        void changeName(const std::string &_newName);

        [[nodiscard]] std::string actorName() const;

        virtual void keyReleaseEvent(const KeyEvent &);

        virtual void keyPressEvent(const KeyEvent &);
    };
}


#endif //PEONYSWORD_ACTOR_H
