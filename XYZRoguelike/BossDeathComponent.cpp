#include "BossDeathComponent.h"
#include "DeveloperLevel.h"
#include "../Engine/GameObject.h"
#include "../Engine/GameWorld.h"
#include "../Engine/Logger.h"

namespace XYZRoguelike
{
BossDeathComponent::BossDeathComponent(XYZEngine::GameObject *gameObject, DeveloperLevel *level)
    : XYZEngine::Component(gameObject), level(level)
{
    stats = gameObject->GetComponent<XYZEngine::StatsComponent>();
}

void BossDeathComponent::Update(float deltaTime)
{
    if (fired || stats == nullptr || level == nullptr)
    {
        return;
    }

    if (stats->GetCurrentHealth() <= 0.f)
    {
        fired = true;
        LOG_INFO("Boss defeated");

        XYZEngine::GameWorld::Instance()->EnqueueLateAction([level = level]() { level->OnBossDied(); });
    }
}

void BossDeathComponent::Render()
{
}
} // namespace XYZRoguelike
