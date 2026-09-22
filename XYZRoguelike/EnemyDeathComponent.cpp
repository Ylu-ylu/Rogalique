#include "EnemyDeathComponent.h"
#include "../Engine/GameObject.h"
#include "../Engine/TransformComponent.h"
#include "../Engine/SpriteColliderComponent.h"
#include "../Engine/SpriteMovementAnimationComponent.h"
#include "../Engine/ResourceSystem.h"
#include "../Engine/Logger.h"

namespace XYZRoguelike
{
EnemyDeathComponent::EnemyDeathComponent(XYZEngine::GameObject *gameObject) : XYZEngine::Component(gameObject)
{
    stats = gameObject->GetComponent<XYZEngine::StatsComponent>();

    deathSound = gameObject->AddComponent<XYZEngine::AudioComponent>();
    const sf::SoundBuffer *buffer = XYZEngine::ResourceSystem::Instance()->GetSound("Creeper-death");
    if (buffer != nullptr)
    {
        deathSound->SetAudio(*buffer);
        deathSound->SetLoop(false);
    }
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

        auto animation = gameObject->GetComponent<XYZEngine::SpriteMovementAnimationComponent>();
        if (animation != nullptr)
        {
            animation->Play("death");
        }

        if (deathSound != nullptr)
        {
            deathSound->Stop();
            deathSound->Play();
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
