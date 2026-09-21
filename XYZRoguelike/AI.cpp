#include "AI.h"
#include "GameConstants.h"
#include "HealthBarComponent.h"
#include "EnemyDeathComponent.h"
#include <stdexcept>
#include "../Engine/ResourceSystem.h"
#include "../Engine/RigidbodyComponent.h"
#include "../Engine/SpriteColliderComponent.h"
#include "../Engine/FollowComponent.h"
#include "../Engine/SpriteDirectionComponent.h"
#include "../Engine/SpriteMovementAnimationComponent.h"
#include "../Engine/StatsComponent.h"
#include "../Engine/AttackComponen.h"
#include "../Engine/SpriteRendererComponent.h"
#include "../Engine/GameWorld.h"
#include "../Engine/TransformComponent.h"

namespace XYZRoguelike
{
// Constructs an AI enemy by delegating to the full constructor with position parameter
AI::AI(const XYZEngine::Vector2Df &position, XYZEngine::GameObject *player) : AI(player, "ai", 0)
{
    SetPosition(position);
}

// Constructs an AI enemy with sprite renderer, follow behavior, physics, collider, and combat stats
AI::AI(XYZEngine::GameObject *player, const std::string &enemyName, int id) : fallowTarget(player)
{
    const std::string name = enemyName + "_" + std::to_string(id);
    gameObject = XYZEngine::GameWorld::Instance()->CreateGameObject(name);

    auto renderer = gameObject->AddComponent<XYZEngine::SpriteRendererComponent>();
    const sf::Texture *texture = XYZEngine::ResourceSystem::Instance()->GetTextureMapElementShared(AI_TEXTURE_KEY, DEFAULT_TEXTURE_INDEX);
    if (texture == nullptr)
    {
        throw std::runtime_error("AI texture 'ai' is not loaded");
    }
    renderer->SetTexture(*texture);

    // keep texture aspect ratio (no squash)
    const auto texSize = texture->getSize();
    const int desiredWidth = static_cast<int>(AI_TEXTURE_HEIGHT * static_cast<float>(texSize.x) / static_cast<float>(texSize.y));
    renderer->SetPixelSize(desiredWidth, static_cast<int>(AI_TEXTURE_HEIGHT));

    auto follower = gameObject->AddComponent<XYZEngine::FollowComponent>();
    follower->SetTarget(player);
    follower->SetSpeed(AI_SPEED);

    gameObject->AddComponent<XYZEngine::SpriteDirectionComponent>();

    auto animation = gameObject->AddComponent<XYZEngine::SpriteMovementAnimationComponent>();
    animation->AddAnimation("walk", AI_TEXTURE_KEY, {0, 1, 2, 3, 4, 5, 6}, 0.1f, true);
    animation->AddAnimation("idle", AI_TEXTURE_KEY, {DEFAULT_TEXTURE_INDEX}, 0.1f, true);
    animation->Play("idle");

    auto rigidbody = gameObject->AddComponent<XYZEngine::RigidbodyComponent>();
    rigidbody->SetKinematic(false);

    gameObject->AddComponent<XYZEngine::SpriteColliderComponent>();
    gameObject->AddComponent<XYZEngine::StatsComponent>(AI_HEALTH, AI_ARMOR);
    gameObject->AddComponent<XYZEngine::AttackComponent>(AI_ATTACK_POWER);

    auto healthBar = gameObject->AddComponent<HealthBarComponent>();
    healthBar->SetOffset(0.f, 80.f);
    healthBar->SetSize(70.f, 8.f);

    gameObject->AddComponent<EnemyDeathComponent>();
}

// Gets the AI's game object
XYZEngine::GameObject *AI::GetGameObject() const
{
    return gameObject;
}

// Creates a copy of this AI at the specified spawn position with given name and ID
std::unique_ptr<AI> AI::Clone(XYZEngine::Vector2Df spawnPosition, const std::string &enemyName, int id) const
{
    auto clonedAI = std::make_unique<AI>(fallowTarget, enemyName, id);
    clonedAI->SetPosition(spawnPosition);
    clonedAI->SetColor(sf::Color::Blue);
    return clonedAI;
}

// Sets the world position of the AI's game object
void AI::SetPosition(const XYZEngine::Vector2Df &spawnPosition)
{
    auto transform = gameObject->GetComponent<XYZEngine::TransformComponent>();
    if (transform != nullptr)
    {
        transform->SetWorldPosition(spawnPosition);
    }
}

// Sets the color tint of the AI's sprite renderer
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
    // TODO: �������� StatsComponent ���� ����
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
    // TODO: �������� FollowComponent � �������� ��������
    auto follower = gameObject->GetComponent<XYZEngine::FollowComponent>();
    if (follower)
    {
        follower->SetSpeed(follower->GetSpeed() * multiplier);
    }
}
} // namespace XYZRoguelike