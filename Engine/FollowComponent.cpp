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
    }
}

void FollowComponent::SetTarget(GameObject *targetObject)
{
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

    Vector2Df direction = targetTransform->GetWorldPosition() - currentPosition;
    float length = direction.GetLength();

    if (length > 0.001f)
    {
        Vector2Df normalized = Vector2Df(direction.x / length, direction.y / length);
        transform->MoveBy(normalized * speed * deltaTime);
    }

    if (animation != nullptr)
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

float FollowComponent::getSpeed() const
{
    return speed;
}
} // namespace XYZEngine