#include "PlayerAttackComponent.h"
#include "CreeperSpawner.h"
#include "../Engine/GameObject.h"
#include "../Engine/ResourceSystem.h"
#include "../Engine/Logger.h"

#include <SFML/Window/Keyboard.hpp>

namespace XYZRoguelike
{
PlayerAttackComponent::PlayerAttackComponent(XYZEngine::GameObject *gameObject) : XYZEngine::Component(gameObject)
{
    transform = gameObject->GetComponent<XYZEngine::TransformComponent>();
    attack = gameObject->GetComponent<XYZEngine::AttackComponent>();
    stats = gameObject->GetComponent<XYZEngine::StatsComponent>();
    animation = gameObject->GetComponent<XYZEngine::SpriteMovementAnimationComponent>();

    attackSound = gameObject->AddComponent<XYZEngine::AudioComponent>();
    const sf::SoundBuffer *buffer = XYZEngine::ResourceSystem::Instance()->GetSound("Attack");
    if (buffer != nullptr)
    {
        attackSound->SetAudio(*buffer);
        attackSound->SetLoop(false);
        attackSound->SetVolume(100.f);
    }
}

void PlayerAttackComponent::SetEnemySpawner(CreeperSpawner *spawner)
{
    enemySpawner = spawner;
}

void PlayerAttackComponent::TryDealDamage()
{
    if (enemySpawner == nullptr || transform == nullptr || attack == nullptr)
    {
        return;
    }

    // the melee swing hits every enemy in range, not just the closest one
    std::vector<XYZEngine::GameObject *> targets;
    enemySpawner->FindEnemiesInRange(transform->GetWorldPosition(), attackRange, targets);

    if (targets.empty())
    {
        LOG_INFO("Player attack missed: no enemy in range");
        return;
    }

    for (XYZEngine::GameObject *target : targets)
    {
        attack->Attack(target);
    }

    LOG_INFO("Player attack hit " + std::to_string(targets.size()) + " enemies");
}

void PlayerAttackComponent::Update(float deltaTime)
{
    if (transform == nullptr || attack == nullptr)
    {
        return;
    }

    if (stats != nullptr && stats->GetCurrentHealth() <= 0.f)
    {
        return;
    }

    if (currentCooldown > 0.f)
    {
        currentCooldown -= deltaTime;
    }

    if (isAttackActive)
    {
        attackTimer += deltaTime;

        if (!damageApplied && attackTimer >= damageMoment)
        {
            TryDealDamage();
            damageApplied = true;
        }

        if (attackTimer >= attackDuration)
        {
            isAttackActive = false;
        }

        return;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && currentCooldown <= 0.f)
    {
        isAttackActive = true;
        damageApplied = false;
        attackTimer = 0.f;
        currentCooldown = attackCooldown;

        if (animation != nullptr)
        {
            animation->Play("attack");
        }

        if (attackSound != nullptr)
        {
            attackSound->Stop();
            attackSound->Play();
        }

        LOG_INFO("Player attack started");
    }
}

void PlayerAttackComponent::Render()
{
}
} // namespace XYZRoguelike
