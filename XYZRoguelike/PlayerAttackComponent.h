#pragma once

#include "../Engine/Component.h"
#include "../Engine/TransformComponent.h"
#include "../Engine/StatsComponent.h"
#include "../Engine/AttackComponen.h"
#include "../Engine/SpriteMovementAnimationComponent.h"

namespace XYZRoguelike
{
class CreeperSpawner;

class PlayerAttackComponent : public XYZEngine::Component
{
  public:
    PlayerAttackComponent(XYZEngine::GameObject *gameObject);

    void Update(float deltaTime) override;
    void Render() override;

    void SetEnemySpawner(CreeperSpawner *spawner);

  private:
    XYZEngine::TransformComponent *transform = nullptr;
    XYZEngine::AttackComponent *attack = nullptr;
    XYZEngine::StatsComponent *stats = nullptr;
    XYZEngine::SpriteMovementAnimationComponent *animation = nullptr;
    CreeperSpawner *enemySpawner = nullptr;

    float attackRange = 180.f;
    float attackCooldown = 0.5f;
    float currentCooldown = 0.f;
    float damageMoment = 0.22f;
    float attackDuration = 0.4f;
    float attackTimer = 0.f;
    bool isAttackActive = false;
    bool damageApplied = false;

    void TryDealDamage();
};
} // namespace XYZRoguelike
