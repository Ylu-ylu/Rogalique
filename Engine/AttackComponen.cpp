#include "pch.h"
#include "AttackComponen.h"

namespace XYZEngine
{
	void AttackComponent::Attack(GameObject* target)
	{
		if (!target)
		{
			return;
		}

		auto targetStatsComponent=target->AddComponent<StatsComponent>();
		if(targetStatsComponent)
		{
			float damage = attackPower;
			targetStatsComponent->TakeDamage(damage);
		}

	}

}
