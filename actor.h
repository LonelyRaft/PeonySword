
#ifndef ACTOR_H
#define ACTOR_H

#include "skill.h"
#include "weapon.h"
#include "fightattrs.h"
#include <iostream>
#include <string>
#include <array>
#include <vector>

namespace PeonySword
{
	struct Point
	{
		double x = { 0 };
		double y = { 0 };
		double z = { 0 };

		Point() = default;
		explicit Point(
			double _x,
			double _y = 0,
			double _z = 0);
		virtual ~Point();
		friend std::ostream& operator<<(
			std::ostream& _os, const Point& _point);
		friend Point operator+(
			const Point& _point1,
			const Point& _point2);
		Point& operator+=(const Point& _point);
	};

	class ActorStatus
	{

	};

	class Actor
	{
		FightAttrs attrs;
		Point pos;
		std::string name;
		std::array<Weapon*, 6> wps =
		{ nullptr, nullptr, nullptr,
			nullptr, nullptr, nullptr };
		std::array<Skill*, 4> skills =
		{ nullptr, nullptr, nullptr, nullptr };
	public:
		Actor() = default;
		Actor(const Point& _pos,
			const std::string& _name);
		virtual ~Actor();

		const FightAttrs& readAttrs() const;
		friend std::ostream& operator<<(
			std::ostream& _os,
			const Actor& _actor);
		
		void attack(Actor* _enemy);
		void attack(std::vector<Actor*> _enemies);
		
		void useSkill(int _idx, Actor* _actor);
		void useSkill(int _idx, std::vector<Actor*> _actors);
		
		void move(const Point& _offset);
		void moveTo(const Point& _dest);
		void fly(const Point& _offset);
		void flyTo(const Point& _dest);
	};
}

#endif // ACTOR_H


