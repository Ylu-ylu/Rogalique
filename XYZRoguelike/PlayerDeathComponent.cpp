#include "PlayerDeathComponent.h"
#include "DeveloperLevel.h"
#include "../Engine/GameObject.h"
#include "../Engine/GameWorld.h"
#include "../Engine/SpriteMovementAnimationComponent.h"
#include "../Engine/Logger.h"

namespace XYZRoguelike
{
PlayerDeathComponent::PlayerDeathComponent(XYZEngine::GameObject *gameObject, DeveloperLevel *level)
    : XYZEngine::Component(gameObject), level(level)
{
    stats = gameObject->GetComponent<XYZEngine::StatsComponent>();
}

void PlayerDeathComponent::Update(float deltaTime)
{
    if (stats == nullptr || level == nullptr)
    {
        return;
    }

    if (stats->GetCurrentHealth() <= 0.f)
    {
        if (!deathStarted)
        {
            deathStarted = true;
            deathTimer = 0.8f;

            auto animation = gameObject->GetComponent<XYZEngine::SpriteMovementAnimationComponent>();
            if (animation != nullptr)
            {
                animation->Play("death");
            }

            LOG_INFO("Player died - restarting level");
        }

        deathTimer -= deltaTime;

        if (deathTimer <= 0.f && !deathQueued)
        {
            deathQueued = true;
            XYZEngine::GameWorld::Instance()->EnqueueLateAction([level = level]() { level->Restart(); });
        }
    }
}

void PlayerDeathComponent::Render()
{
}
} // namespace XYZRoguelike
