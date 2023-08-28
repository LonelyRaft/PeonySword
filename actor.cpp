
#include "actor.h"

namespace PeonySword
{
	Point::Point(double _x, double _y, double _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	Point::~Point()
	{
	}

	Point& Point::operator+=(const Point& _point)
	{
		x += _point.x;
		y += _point.y;
		z += _point.z;
		return (*this);
	}

	Point operator+(
		const Point& _point1,
		const Point& _point2)
	{
		Point p;
		p.x = _point1.x + _point1.x;
		p.y = _point1.y + _point2.y;
		p.z = _point1.z + _point2.z;
		return p;
	}

	Actor::Actor(
		const Point& _pos,
		const std::string& _name)
	{
		pos = _pos;
		name = _name;
	}

	Actor::~Actor()
	{
		for (auto wp : wps) {
			delete wp;
		}
		for (auto skill : skills) {
			delete skill;
		}
	}

	const FightAttrs& PeonySword::Actor::readAttrs() const
	{
		return attrs;
	}

	void Actor::attack(Actor* _enemy)
	{
		if (_enemy == nullptr) {
			return;
		}
		FightAttrs& _attrs = _enemy->attrs;
		_attrs.CH -= attrs.A;
		// ÎüÑª
		// attrs.CH += (attrs.A * 10%);
	}

	void Actor::attack(std::vector<Actor*> _enemies)
	{
		for (auto enemy : _enemies) {
			attack(enemy);
		}
	}

	void Actor::useSkill(int _idx, Actor* _actor)
	{
		if (_actor == nullptr) {
			return;
		}
		skills[_idx]->function(_actor);
	}

	void Actor::useSkill(int _idx, std::vector<Actor*> _actors)
	{
	}

	void Actor::move(const Point& _offset)
	{
		pos += _offset;
	}

	void Actor::moveTo(const Point& _dest)
	{
		pos = _dest;
	}

	void Actor::fly(const Point& _offset)
	{
	}

	void Actor::flyTo(const Point& _dest)
	{
	}

	std::ostream& operator<<(
		std::ostream& _os, const Point& _point)
	{
		_os << "Point(" << _point.x
			<< ", " << _point.y
			<< ", " << _point.z << ");";
		return _os;
	}

	std::ostream& operator<<(
		std::ostream& _os, const Actor& _actor)
	{
		_os << _actor.name
			<< "{" << _actor.pos
			<< ", " << _actor.attrs << "}";
		return _os;
	}
}

