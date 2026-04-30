#include "AI.h"
#include <stdexcept>
#include <ResourceSystem.h>
#include "RigidbodyComponent.h"
#include "SpriteColliderComponent.h"
#include "FollowComponent.h"
#include <StatsComponent.h>
#include <AttackComponen.h>
#include "SpriteRendererComponent.h"
#include "GameWorld.h"
#include "TransformComponent.h"

namespace XYZRoguelike
{
AI::AI(const XYZEngine::Vector2Df &position, XYZEngine::GameObject *player) : AI(player, "ai", 0)
{
    SetPosition(position);
}

AI::AI(XYZEngine::GameObject *player, const std::string &enemyName, int id) : fallowTarget(player)
{
    const std::string name = enemyName + "_" + std::to_string(id);
    gameObject = XYZEngine::GameWorld::Instance()->CreateGameObject(name);

    auto renderer = gameObject->AddComponent<XYZEngine::SpriteRendererComponent>();
    const sf::Texture *texture = XYZEngine::ResourceSystem::Instance()->GetTextureMapElementShared("ai", 0);
    if (texture == nullptr)
    {
        throw std::runtime_error("AI texture 'ai' is not loaded");
    }
    renderer->SetTexture(*texture);

    // keep texture aspect ratio (no squash)
    const float desiredHeight = 128.0f;
    const auto texSize = texture->getSize();
    const int desiredWidth = static_cast<int>(desiredHeight * static_cast<float>(texSize.x) / static_cast<float>(texSize.y));
    renderer->SetPixelSize(desiredWidth, static_cast<int>(desiredHeight));

    auto follower = gameObject->AddComponent<XYZEngine::FollowComponent>();
    follower->SetTarget(player);
    follower->SetSpeed(120.f);

    auto rigidbody = gameObject->AddComponent<XYZEngine::RigidbodyComponent>();
    rigidbody->SetKinematic(false);

    gameObject->AddComponent<XYZEngine::SpriteColliderComponent>();
    gameObject->AddComponent<XYZEngine::StatsComponent>(maxHealth, maxArmor);
    gameObject->AddComponent<XYZEngine::AttackComponent>(attackPower);
}

XYZEngine::GameObject *AI::GetGameObject() const
{
    return gameObject;
}

std::unique_ptr<AI> AI::Clone(XYZEngine::Vector2Df spawnPosition, const std::string &enemyName, int id) const
{
    auto clonedAI = std::make_unique<AI>(fallowTarget, enemyName, id);
    clonedAI->SetPosition(spawnPosition);
    clonedAI->SetColor(sf::Color::Blue);
    return clonedAI;
}

void AI::SetPosition(const XYZEngine::Vector2Df &spawnPosition)
{
    auto transform = gameObject->GetComponent<XYZEngine::TransformComponent>();
    if (transform != nullptr)
    {
        transform->SetWorldPosition(spawnPosition);
    }
}

void AI::SetColor(const sf::Color &color)
{
    auto renderer = gameObject->GetComponent<XYZEngine::SpriteRendererComponent>();
    if (renderer != nullptr)
    {
        renderer->SetColor(color);
    }
}

XYZEngine::Vector2Df AI::GetPosition() const
{
    auto transform = gameObject->GetComponent<XYZEngine::TransformComponent>();
    if (transform != nullptr)
    {
        return transform->GetWorldPosition();
    }

    return {};
}

void AI::SetHealthMultiplier(float multiplier)
{
    maxHealth = baseHealth * multiplier;
    // TODO: Обновить StatsComponent если есть
    auto stats = gameObject->GetComponent<XYZEngine::StatsComponent>();
    if (stats)
    {
        stats->SetMaxHealth(maxHealth);
        stats->SetCurrentHealth(maxHealth);
    }
}

void AI::SetArmorMultiplier(float multiplier)
{
    maxArmor = baseArmor * multiplier;
    auto stats = gameObject->GetComponent<XYZEngine::StatsComponent>();
    if (stats)
    {
        stats->SetMaxArmor(maxArmor);
        stats->SetCurrentArmor(maxArmor);
    }
}

void AI::SetAttackMultiplier(float multiplier)
{
    attackPower = baseAttack * multiplier;
    auto attack = gameObject->GetComponent<XYZEngine::AttackComponent>();
    if (attack)
    {
        attack->SetAttackPower(attackPower);
    }
}

void AI::SetSpeedMultiplier(float multiplier)
{
    // TODO: Получить FollowComponent и изменить скорость
    auto follower = gameObject->GetComponent<XYZEngine::FollowComponent>();
    if (follower)
    {
        follower->SetSpeed(follower->GetSpeed() * multiplier);
    }
}
} // namespace XYZRoguelike