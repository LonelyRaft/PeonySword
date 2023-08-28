
#ifndef WEAPON_H
#define WEAPON_H

#include "fightattrs.h"
#include <string>

namespace PeonySword
{
	class Actor;

	class Weapon {
	protected:
		std::string name;
		FightAttrs attrs;
	public:
		Weapon() = default;
		virtual ~Weapon();
	};
}

#endif // !WEAPON_H

