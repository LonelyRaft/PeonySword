
#include "skill.h"

namespace PeonySword 
{
	SkillAttrs::~SkillAttrs()
	{
	}

	Skill::~Skill()
	{
	}

	std::ostream& operator<<(
		std::ostream& _os, const SkillAttrs& _attrs)
	{
		return _os;
	}

	std::ostream& operator<<(std::ostream& _os, const Skill& _skill)
	{
		return _os;
	}
}
