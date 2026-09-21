#include "EnemyDeathComponent.h"
#include "../Engine/GameObject.h"
#include "../Engine/TransformComponent.h"
#include "../Engine/SpriteColliderComponent.h"
#include "../Engine/Logger.h"

namespace XYZRoguelike
{
EnemyDeathComponent::EnemyDeathComponent(XYZEngine::GameObject *gameObject) : XYZEngine::Component(gameObject)
{
    stats = gameObject->GetComponent<XYZEngine::StatsComponent>();
}

void EnemyDeathComponent::Update(float deltaTime)
{
    if (stats == nullptr)
    {
        return;
    }

    if (!dying && stats->GetCurrentHealth() <= 0.f)
    {
        dying = true;
        deathTimer = 2.f;

        auto collider = gameObject->GetComponent<XYZEngine::SpriteColliderComponent>();
        if (collider != nullptr)
        {
            collider->SetEnabled(false);
        }

        LOG_INFO(gameObject->GetName() + " died");
    }

    if (dying)
    {
        deathTimer -= deltaTime;

        if (deathTimer <= 0.f)
        {
            auto transform = gameObject->GetComponent<XYZEngine::TransformComponent>();
            if (transform != nullptr)
            {
                transform->SetWorldPosition({-2000.f, -2000.f});
            }
        }
    }
}

void EnemyDeathComponent::Render()
{
}
} // namespace XYZRoguelike
