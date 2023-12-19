#ifndef PEONYSWORD_POSITION_H
#define PEONYSWORD_POSITION_H

namespace PeonySword {
    class Position {
    public:
        double mPosX = 0;
        double mPosY = 0;
        double mPosZ = 0;

        bool operator==(const Position &) const;

        bool operator>(const Position &) const;

        bool operator<(const Position &) const;

        bool operator!=(const Position &) const;

        bool operator>=(const Position &) const;

        bool operator<=(const Position &) const;

        void move(double _x, double _y, double _z = 0);

        void moveTo(double _x, double _y, double _z = 0);
    };
}

#endif //PEONYSWORD_POSITION_H
