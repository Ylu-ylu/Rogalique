#include "pch.h"
#include "FollowComponent.h"
#include <iostream>

namespace XYZEngine
{

FollowComponent::FollowComponent(GameObject *gameObject) : Component(gameObject)
{
    transform = gameObject->GetComponent<TransformComponent>();
    if (transform == nullptr)
    {
        std::cout << "FollowComponent requires a TransformComponent." << std::endl;
        gameObject->RemoveComponent(this);
        return;
    }

    attackSound = gameObject->AddComponent<AudioComponent>();
    const sf::SoundBuffer *buffer = ResourceSystem::Instance()->GetSound("Attack");
    if (buffer != nullptr)
    {
        attackSound->SetAudio(*buffer);
        attackSound->SetLoop(false);
        attackSound->SetVolume(100.f);
    }
}

void FollowComponent::SetTarget(GameObject *targetObject)
{
    targetGameObject = targetObject;
    if (targetObject)
    {
        targetTransform = targetObject->GetComponent<TransformComponent>();
        if (targetObject == nullptr)
        {
            std::cout << "Target object has no TransformComponent." << std::endl;
        }
    }
}

void FollowComponent::Update(float deltaTime)
{
    if (animation == nullptr)
    {
        animation = gameObject->GetComponent<SpriteMovementAnimationComponent>();
    }

    if (transform == nullptr)
    {
        return;
    }

    if (stats == nullptr)
    {
        stats = gameObject->GetComponent<StatsComponent>();
    }

    if (stats != nullptr && stats->GetCurrentHealth() <= 0.f)
    {
        return;
    }

    if (isAttackActive)
    {
        swingTimer += deltaTime;

        if (!damageApplied && swingTimer >= damageMoment)
        {
            damageApplied = true;

            if (attack == nullptr)
            {
                attack = gameObject->GetComponent<AttackComponent>();
            }

            if (attack != nullptr && targetGameObject != nullptr)
            {
                attack->Attack(targetGameObject);
            }
        }

        if (swingTimer >= attackDuration)
        {
            isAttackActive = false;
        }

        return;
    }

    Vector2Df currentPosition = transform->GetWorldPosition();
    Vector2Df netDelta = previousPositionValid ? currentPosition - previousPosition : Vector2Df{0.f, 0.f};
    previousPosition = currentPosition;
    previousPositionValid = true;

    windowDelta = windowDelta + netDelta;
    windowTimer += deltaTime;

    float windowDistance = windowDelta.GetLength();
    if (windowTimer >= checkWindow || windowDistance > moveDistanceThreshold)
    {
        isWalking = windowDistance > moveDistanceThreshold;
        windowDelta = {0.f, 0.f};
        windowTimer = 0.f;
    }

    if (targetTransform == nullptr)
    {
        isWalking = false;
        windowDelta = {0.f, 0.f};
        windowTimer = 0.f;

        if (animation != nullptr)
        {
            animation->Play("idle");
        }
        return;
    }

    // a dead target is not chased (the corpse is left alone)
    if (targetGameObject != nullptr)
    {
        if (targetStats == nullptr)
        {
            targetStats = targetGameObject->GetComponent<StatsComponent>();
        }

        if (targetStats != nullptr && targetStats->GetCurrentHealth() <= 0.f)
        {
            if (animation != nullptr)
            {
                animation->Play("idle");
            }
            return;
        }
    }

    Vector2Df direction = targetTransform->GetWorldPosition() - currentPosition;
    float length = direction.GetLength();

    if (length > 0.001f && length > attackRange)
    {
        Vector2Df normalized = Vector2Df(direction.x / length, direction.y / length);
        transform->MoveBy(normalized * speed * deltaTime);
    }

    attackTimer += deltaTime;
    if (targetGameObject != nullptr && length < attackRange && attackTimer >= attackCooldown)
    {
        attackTimer = 0.f;
        isAttackActive = true;
        damageApplied = false;
        swingTimer = 0.f;

        if (animation != nullptr)
        {
            animation->Play("attack");
        }

        if (attackSound != nullptr)
        {
            attackSound->Stop();
            attackSound->Play();
        }
    }

    if (animation != nullptr && !animation->IsPlaying("attack") && !animation->IsCurrentAnimation("death"))
    {
        animation->Play(isWalking ? "walk" : "idle");
    }
}

void FollowComponent::Render()
{
}

void FollowComponent::SetSpeed(float newSpeed)
{
    speed = newSpeed;
}

void FollowComponent::SetAttackCooldown(float newCooldown)
{
    if (newCooldown > 0.f)
    {
        attackCooldown = newCooldown;
    }
}

float FollowComponent::getSpeed() const
{
    return speed;
}
} // namespace XYZEngine