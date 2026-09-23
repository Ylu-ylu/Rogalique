#pragma once

#include "../Engine/Component.h"
#include "../Engine/TransformComponent.h"
#include "../Engine/StatsComponent.h"
#include "../Engine/FollowComponent.h"
#include "../Engine/SpriteRendererComponent.h"

#include <SFML/Graphics.hpp>

namespace XYZRoguelike
{
// Unique boss abilities:
// 1) AOE with telegraph - every ~8s a red ring grows for 1s around the boss,
//    then everyone inside the radius takes damage;
// 2) Rage below 50% HP - movement speed x1.5, attack cooldown x0.5.
class BossAbilitiesComponent : public XYZEngine::Component
{
  public:
    BossAbilitiesComponent(XYZEngine::GameObject *gameObject, XYZEngine::GameObject *player);

    void Update(float deltaTime) override;
    void Render() override;

  private:
    XYZEngine::TransformComponent *transform = nullptr;
    XYZEngine::StatsComponent *stats = nullptr;
    XYZEngine::FollowComponent *follower = nullptr;
    XYZEngine::SpriteRendererComponent *renderer = nullptr;

    XYZEngine::TransformComponent *playerTransform = nullptr;
    XYZEngine::StatsComponent *playerStats = nullptr;

    // AOE
    float aoeCooldown = 8.f;
    float aoeTimer = 0.f;
    bool aoeTelegraphActive = false;
    float telegraphTimer = 0.f;
    float telegraphDuration = 1.f;
    float aoeRadius = 200.f;
    int aoeDamage = 25;
    sf::CircleShape telegraphRing;

    // Rage
    bool rageActive = false;
    float baseSpeed = 60.f;
    float baseAttackCooldown = 1.f;
};
} // namespace XYZRoguelike
