#pragma once
#include "Component.h"
#include "TransformComponent.h"
#include "SpriteRendererComponent.h"
#include "SpriteMovementAnimationComponent.h"
#include "StatsComponent.h"
#include "AttackComponen.h"
#include "ResourceSystem.h"

namespace XYZEngine
{
class FollowComponent : public Component
{
  public:
    FollowComponent(GameObject *gameObject);
    void Update(float deltaTime) override;
    void Render() override;

    void SetTarget(GameObject *targetObject);
    void SetSpeed(float newSpeed);
    float getSpeed() const;
    float GetSpeed() const
    {
        return speed;
    }

  private:
    TransformComponent *transform = nullptr;
    TransformComponent *targetTransform = nullptr;
    GameObject *targetGameObject = nullptr;
    SpriteMovementAnimationComponent *animation = nullptr;
    AttackComponent *attack = nullptr;
    StatsComponent *stats = nullptr;

    float attackRange = 140.f;
    float attackCooldown = 1.f;
    float attackTimer = 0.f;

    Vector2Df previousPosition = {0.f, 0.f};
    bool previousPositionValid = false;
    Vector2Df windowDelta = {0.f, 0.f};
    float windowTimer = 0.f;
    float checkWindow = 0.25f;
    float moveDistanceThreshold = 8.f;
    bool isWalking = false;

    // Render
    std::vector<const sf::Texture *> textureMap;
    float secondsForFrame = 0.f;

    float speed = 100.f; // default speed
};
} // namespace XYZEngine