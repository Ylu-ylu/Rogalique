#include "pch.h"
#include "AttackComponen.h"

namespace XYZEngine
{
void AttackComponent::Attack(GameObject *target)
{
    if (!target)
    {
        return;
    }

    auto targetStatsComponent = target->GetComponent<StatsComponent>();
    if (targetStatsComponent)
    {
        float damage = attackPower;
        targetStatsComponent->TakeDamage(damage);
    }
}
void AttackComponent::Update(float /*deltaTime*/)
{
    // No periodic logic for now
}

void AttackComponent::Render()
{
    // Nothing to render for AttackComponent
}

} // namespace XYZEngine
