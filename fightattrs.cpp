
#include "fightattrs.h"
#include "errdefs.h"

namespace PeonySword
{
	FightAttrs::~FightAttrs()
	{
	}

	int FightAttrs::setMaxHealth(int _max_health)
	{
		if (_max_health < 0) {
			return ERR_DEF_INVALID_ARGV;
		}
		MH = _max_health;
		if (CH > MH) {
			CH = MH;
		}
		return ERR_DEF_OKAY;
	}

	int FightAttrs::incMaxHealth(int _health)
	{
		if (_health < 0) {
			return ERR_DEF_INVALID_ARGV;
		}
		MH += _health;
		if (_health = (MH - CH)) {
			CH = MH;
		}
		return ERR_DEF_OKAY;
	}

	int FightAttrs::redMaxHealth(int _health)
	{
		if (_health < 0) {
			return ERR_DEF_INVALID_ARGV;
		}
		MH -= _health;
		if (MH < 0) {
			MH = 0;
		}
		if (CH > MH) {
			CH = MH;
		}
		return ERR_DEF_OKAY;
	}

	int FightAttrs::getMaxHealth() const
	{
		return MH;
	}

	int FightAttrs::setHealth(int _health)
	{
		if (_health > MH) {
			CS = MH;
		}
		CS = _health;
		return ERR_DEF_OKAY;
	}

	int FightAttrs::incHealth(int _health)
	{
		if (_health < 0) {
			return ERR_DEF_INVALID_ARGV;
		}
		CH += _health;
		if (CH > MH) {
			CH = MH;
		}
		return ERR_DEF_OKAY;
	}

	int FightAttrs::redHealth(int _health)
	{
		if (_health < 0) {
			return ERR_DEF_INVALID_ARGV;
		}
		CH -= _health;
		return ERR_DEF_OKAY;
	}

	int FightAttrs::getHealth() const
	{
		return CH;
	}

	int FightAttrs::setMaxStrength(int _max_strength)
	{
		return 0;
	}

	int FightAttrs::incMaxStrength(int _strength)
	{
		return 0;
	}

	int FightAttrs::redMaxStrength(int _strength)
	{
		return 0;
	}

	int FightAttrs::getMaxStrength() const
	{
		return 0;
	}

	int FightAttrs::setStrength(int _strength)
	{
		if (_strength < 0) {
			return ERR_DEF_INVALID_ARGV;
		}
		CS = _strength;
		if (CS > MS) {
			CS = MS;
		}
		return ERR_DEF_OKAY;
	}

	int FightAttrs::incStrength(int _strength)
	{
		if (_strength < 0) {
			return ERR_DEF_INVALID_ARGV;
		}
		CS += _strength;
		if (CS > MS) {
			CS = MS;
		}
		return ERR_DEF_OKAY;
	}

	int FightAttrs::redStrength(int _strength)
	{
		if (_strength < 0) {
			return ERR_DEF_INVALID_ARGV;
		}
		if (_strength > CS) {
			return ERR_DEF_NOT_ENOUGH;
		}
		CS -= _strength;
		return ERR_DEF_OKAY;
	}

	int FightAttrs::getStrength() const
	{
		return CS;
	}

	int FightAttrs::setAttack(int _attack)
	{
		if (_attack < 0) {
			return ERR_DEF_INVALID_ARGV;
		}
		A = _attack;
		return ERR_DEF_OKAY;
	}

	int FightAttrs::incAttack(int _attack)
	{
		if (_attack < 0) {
			return ERR_DEF_INVALID_ARGV;
		}
		A += _attack;
		return ERR_DEF_OKAY;
	}

	int FightAttrs::redAttack(int _attack)
	{
		if (_attack < 0) {
			return ERR_DEF_INVALID_ARGV;
		}
		A -= _attack;
		if (A < 0) {
			A = 0;
		}
		return ERR_DEF_OKAY;
	}

	int FightAttrs::getAttack() const
	{
		return A;
	}

	int FightAttrs::setPower(int _power)
	{
		if (_power < 0) {
			return ERR_DEF_INVALID_ARGV;
		}
		P = _power;
		return ERR_DEF_OKAY;
	}

	int FightAttrs::incPower(int _power)
	{
		if (_power < 0) {
			return ERR_DEF_INVALID_ARGV;
		}
		P += _power;
		return ERR_DEF_OKAY;
	}

	int FightAttrs::redPower(int _power)
	{
		if (_power < 0) {
			return ERR_DEF_INVALID_ARGV;
		}
		P -= _power;
		if (P < 0) {
			P = 0;
		}
		return ERR_DEF_OKAY;
	}

	int FightAttrs::getPower() const
	{
		return P;
	}

	std::ostream& operator<<(
		std::ostream& _os, const FightAttrs& _attrs)
	{
		_os << "ActorAttrs{"
			<< "maxHealth:" << _attrs.MH << ", currHealth:" << _attrs.CH
			<< ", maxStrength:" << _attrs.MS << ", currStrength: " << _attrs.CS
			<< ", attack:" << _attrs.A << ", power:" << _attrs.P
			<< ", attResistance: " << _attrs.AR << ", powResistance:" << _attrs.PR
			<< ", attrPenetration:" << _attrs.AP << ", powPenetration:" << _attrs.PP
			<< ", critHitRate:" << _attrs.CHR << ", critHitMagni:" << _attrs.CHM
			<< ", mvSpeed:" << _attrs.MVS << ", attSpeed:" << _attrs.AS
			<< "}";
		return _os;
	}

	FightAttrs operator+(
		const FightAttrs& _attrs1,
		const FightAttrs& _attrs2)
	{
		FightAttrs attrs;
		attrs.A = _attrs1.A + _attrs2.A;
		attrs.AP = _attrs1.AP + _attrs2.AP;
		attrs.AR = _attrs1.AR + _attrs2.AR;
		attrs.AS = _attrs1.AS + _attrs2.AS;
		attrs.P = _attrs1.P + _attrs2.P;
		attrs.PP = _attrs1.PP + _attrs2.PP;
		attrs.PR = _attrs1.PR + _attrs2.PR;
		attrs.MH = _attrs1.MH + _attrs2.MH;
		attrs.MS = _attrs1.MS + _attrs2.MS;
		attrs.CH = _attrs1.CH + _attrs2.CH;
		attrs.CHR = _attrs1.CHR + _attrs2.CHR;
		attrs.CHM = _attrs1.CHM + _attrs2.CHM;
		attrs.MVS = _attrs1.MVS + _attrs2.MVS;
		return attrs;
	}

}
