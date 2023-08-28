
#ifndef ACTOR_4_H
#define ACTOR_4_H

#include "skill.h"

namespace PeonySword
{
	// ÷Œ¡∆–Õ
	namespace Actor4
	{
		class Skill1 : public Skill
		{
		public:
			virtual void function(Actor* _actor) override;
		};

		class Skill2 : public Skill
		{
		public:
			virtual void function(Actor* _actor) override;
		};

		class Skill3 : public Skill
		{
		public:
			virtual void function(Actor* _actor) override;
		};

		class Skill4 : public Skill
		{
		public:
			virtual void function(Actor* _actor) override;
		};
	}
}

#endif // !ACTOR_4_H

