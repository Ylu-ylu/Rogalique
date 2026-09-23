#include "BossAbilitiesComponent.h"
#include "../Engine/GameObject.h"
#include "../Engine/RenderSystem.h"
#include "../Engine/Logger.h"

#include <cmath>

namespace XYZRoguelike
{
BossAbilitiesComponent::BossAbilitiesComponent(XYZEngine::GameObject *gameObject, XYZEngine::GameObject *player)
    : XYZEngine::Component(gameObject)
{
    transform = gameObject->GetComponent<XYZEngine::TransformComponent>();
    stats = gameObject->GetComponent<XYZEngine::StatsComponent>();
    follower = gameObject->GetComponent<XYZEngine::FollowComponent>();
    renderer = gameObject->GetComponent<XYZEngine::SpriteRendererComponent>();

    if (player != nullptr)
    {
        playerTransform = player->GetComponent<XYZEngine::TransformComponent>();
        playerStats = player->GetComponent<XYZEngine::StatsComponent>();
    }

    if (follower != nullptr)
    {
        baseSpeed = follower->GetSpeed();
    }

    telegraphRing.setFillColor(sf::Color(255, 40, 40, 60));
    telegraphRing.setOutlineColor(sf::Color(255, 60, 60, 200));
    telegraphRing.setOutlineThickness(4.f);
    telegraphRing.setOrigin(aoeRadius, aoeRadius);
}

void BossAbilitiesComponent::Update(float deltaTime)
{
    if (stats == nullptr || stats->GetCurrentHealth() <= 0.f)
    {
        aoeTelegraphActive = false;
        return;
    }

    // --- AOE with telegraph ---
    if (aoeTelegraphActive)
    {
        telegraphTimer += deltaTime;

        if (transform != nullptr)
        {
            auto position = transform->GetWorldPosition();
            telegraphRing.setPosition(position.x, position.y);

            // the ring grows from the boss to the full strike radius
            float progress = telegraphTimer / telegraphDuration;
            if (progress > 1.f)
            {
                progress = 1.f;
            }

            float currentRadius = aoeRadius * progress;
            telegraphRing.setRadius(currentRadius);
            telegraphRing.setOrigin(currentRadius, currentRadius);
        }

        if (telegraphTimer >= telegraphDuration)
        {
            aoeTelegraphActive = false;

            // strike: everyone inside the radius takes damage (the player)
            if (playerTransform != nullptr && playerStats != nullptr && transform != nullptr)
            {
                auto playerPosition = playerTransform->GetWorldPosition();
                auto bossPosition = transform->GetWorldPosition();

                float dx = playerPosition.x - bossPosition.x;
                float dy = playerPosition.y - bossPosition.y;

                if (std::sqrt(dx * dx + dy * dy) < aoeRadius && playerStats->GetCurrentHealth() > 0.f)
                {
                    playerStats->TakeDamage(aoeDamage);
                    LOG_INFO("Boss AOE hit the player");
                }
            }
        }

        return;
    }

    aoeTimer += deltaTime;

    if (aoeTimer >= aoeCooldown)
    {
        aoeTimer = 0.f;
        aoeTelegraphActive = true;
        telegraphTimer = 0.f;
        LOG_INFO("Boss AOE telegraph started");
    }

    // --- Rage below 50% HP ---
    if (!rageActive && stats != nullptr && stats->GetMaxHealth() > 0.f)
    {
        if (stats->GetCurrentHealth() <= stats->GetMaxHealth() * 0.5f)
        {
            rageActive = true;

            if (follower != nullptr)
            {
                follower->SetSpeed(baseSpeed * 1.5f);
                follower->SetAttackCooldown(baseAttackCooldown * 0.5f);
            }

            if (renderer != nullptr)
            {
                renderer->SetColor(sf::Color(255, 0, 0, 255));
            }

            LOG_INFO("Boss enters rage");
        }
    }
}

void BossAbilitiesComponent::Render()
{
    if (!aoeTelegraphActive)
    {
        return;
    }

    XYZEngine::RenderSystem *renderSystem = XYZEngine::RenderSystem::Instance();
    renderSystem->Render(telegraphRing);
}
} // namespace XYZRoguelike
