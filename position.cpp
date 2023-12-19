#include "position.h"

namespace PeonySword {
    bool Position::operator==(
            const Position &_another) const {
        return mPosX == _another.mPosX &&
               mPosY == _another.mPosY &&
               mPosZ == _another.mPosZ;
    }

    bool Position::operator>(
            const Position &_another) const {
        if (mPosX > _another.mPosX) {
            return true;
        }
        if (mPosX < _another.mPosX) {
            return false;
        }
        if (mPosY > _another.mPosY) {
            return true;
        }
        if (mPosY < _another.mPosY) {
            return false;
        }
        if (mPosZ > _another.mPosZ) {
            return true;
        }
        return false;
    }

    bool Position::operator<(
            const Position &_another) const {
        if (mPosX < _another.mPosX) {
            return true;
        }
        if (mPosX > _another.mPosX) {
            return false;
        }
        if (mPosY < _another.mPosY) {
            return true;
        }
        if (mPosY > _another.mPosY) {
            return false;
        }
        if (mPosZ < _another.mPosZ) {
            return true;
        }
        return false;
    }

    bool Position::operator!=(
            const Position &_another) const {
        return !(*this == _another);
    }

    bool Position::operator>=(
            const Position &_another) const {
        return !(*this < _another);
    }

    bool Position::operator<=(
            const Position &_another) const {
        return !(*this > _another);
    }

    void Position::move(double _x, double _y, double _z) {
        mPosX += _x;
        mPosY += _y;
        mPosZ += _z;
    }

    void Position::moveTo(double _x, double _y, double _z) {
        mPosX = _x;
        mPosY = _y;
        mPosZ = _z;
    }
}


