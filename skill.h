
#ifndef SKILL_H
#define SKILL_H

#include <iostream>
#include <string>

namespace PeonySword
{
	class Actor;

	class SkillAttrs {
	public:
		SkillAttrs() = default;
		~SkillAttrs();
		friend std::ostream& operator<<(
			std::ostream& _os,
			const SkillAttrs& _attrs);
	};

	class Skill {
	protected:
		std::string name;
	public:
		Skill() = default;
		virtual ~Skill();
		friend std::ostream& operator<<(
			std::ostream& _os,
			const Skill& _skill);
		virtual void function(Actor* _actor) = 0;
	};
}

#endif // !SKILL_H

