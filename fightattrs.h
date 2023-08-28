
#ifndef FIGHT_ATTRS_H
#define FIGHT_ATTRS_H

#include <iostream>

namespace PeonySword
{
	class FightAttrs
	{
		int MH = { 0 }; // max health
		int CH = { 0 }; // current health
		int  MS = { 0 }; // max strength
		int CS = { 0 }; // current strenth

		int A = { 0 }; // attack
		int AR = { 0 }; // attack resistance
		int AS = { 0 }; // attack speed
		int AP = { 0 }; // attack penetration

		int P = { 0 }; // power
		int PR = { 0 }; // power resistance
		int PP = { 0 }; // power penetration

		int CHR = { 0 }; // critical hit rate
		int CHM = { 0 }; // critical hit magnification

		int MVS = { 0 }; // move speed 
	public:
		FightAttrs() = default;
		~FightAttrs();

		int setMaxHealth(int _max_health);
		int incMaxHealth(int _health);
		int redMaxHealth(int _health);
		int getMaxHealth() const;

		int setHealth(int _health);
		int incHealth(int _health);
		int redHealth(int _health);
		int getHealth() const;

		int setMaxStrength(int _max_strength);
		int incMaxStrength(int _strength);
		int redMaxStrength(int _strength);
		int getMaxStrength() const;

		int setStrength(int _strength);
		int incStrength(int _strength);
		int redStrength(int _strength);
		int getStrength() const;

		int setAttack(int _attack);
		int incAttack(int _attack);
		int redAttack(int _attack);
		int getAttack() const;



		int setPower(int _power);
		int incPower(int _power);
		int redPower(int _power);
		int getPower() const;



		friend std::ostream& operator<<(
			std::ostream& _os,
			const FightAttrs& _attrs);

		friend FightAttrs operator+(
			const FightAttrs& _attrs1,
			const FightAttrs& _attrs2);
	};
}

#endif // !FIGHT_ATTRS_H

