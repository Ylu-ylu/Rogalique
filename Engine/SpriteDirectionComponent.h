#pragma once

#include "Component.h"
#include "TransformComponent.h"
#include "SpriteRendererComponent.h"

namespace XYZEngine
{
class SpriteDirectionComponent : public Component
{
  public:
    SpriteDirectionComponent(GameObject *gameObject);

    void Update(float deltaTime) override;
    void Render() override;

    void SetDirection(float xAxis);
    bool IsFacingRight() const;
    void SetFlipThreshold(float threshold);
    void SetConfirmationTime(float seconds);
    float GetConfirmationTime() const;

  private:
    TransformComponent *transform = nullptr;
    SpriteRendererComponent *spriteRenderer = nullptr;

    Vector2Df previousPosition = {0.f, 0.f};
    float flipThreshold = 0.001f;
    float confirmationTime = 0.08f;
    float pendingTimer = 0.f;
    bool hasPendingDirection = false;
    bool pendingFacingRight = true;
    bool isFacingRight = true;
};
} // namespace XYZEngine
