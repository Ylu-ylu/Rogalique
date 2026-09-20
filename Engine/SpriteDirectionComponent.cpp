#include "pch.h"
#include "SpriteDirectionComponent.h"
#include <cmath>

namespace XYZEngine
{
SpriteDirectionComponent::SpriteDirectionComponent(GameObject *gameObject) : Component(gameObject)
{
    transform = gameObject->GetComponent<TransformComponent>();
    spriteRenderer = gameObject->GetComponent<SpriteRendererComponent>();

    if (transform != nullptr)
    {
        previousPosition = transform->GetWorldPosition();
    }
}

void SpriteDirectionComponent::Update(float deltaTime)
{
    if (transform == nullptr || spriteRenderer == nullptr)
    {
        return;
    }

    Vector2Df currentPosition = transform->GetWorldPosition();
    Vector2Df delta = currentPosition - previousPosition;
    previousPosition = currentPosition;

    if (std::abs(delta.x) <= flipThreshold)
    {
        hasPendingDirection = false;
        pendingTimer = 0.f;
        return;
    }

    bool movingRight = delta.x > 0.f;

    if (movingRight == isFacingRight)
    {
        hasPendingDirection = false;
        pendingTimer = 0.f;
        return;
    }

    if (!hasPendingDirection || movingRight != pendingFacingRight)
    {
        hasPendingDirection = true;
        pendingFacingRight = movingRight;
        pendingTimer = 0.f;
    }

    pendingTimer += deltaTime;
    if (pendingTimer >= confirmationTime)
    {
        isFacingRight = pendingFacingRight;
        spriteRenderer->FlipX(!isFacingRight);
        hasPendingDirection = false;
        pendingTimer = 0.f;
    }
}

void SpriteDirectionComponent::Render()
{
}

void SpriteDirectionComponent::SetDirection(float xAxis)
{
    if (xAxis < -flipThreshold)
    {
        isFacingRight = false;
    }
    else if (xAxis > flipThreshold)
    {
        isFacingRight = true;
    }

    hasPendingDirection = false;
    pendingTimer = 0.f;

    if (spriteRenderer != nullptr)
    {
        spriteRenderer->FlipX(!isFacingRight);
    }
}

bool SpriteDirectionComponent::IsFacingRight() const
{
    return isFacingRight;
}

void SpriteDirectionComponent::SetFlipThreshold(float threshold)
{
    if (threshold > 0.f)
    {
        flipThreshold = threshold;
    }
}

void SpriteDirectionComponent::SetConfirmationTime(float seconds)
{
    if (seconds > 0.f)
    {
        confirmationTime = seconds;
    }
}

float SpriteDirectionComponent::GetConfirmationTime() const
{
    return confirmationTime;
}
} // namespace XYZEngine
