#pragma once

#include "../Engine/Component.h"
#include "../Engine/TransformComponent.h"
#include "../Engine/StatsComponent.h"
#include "../Engine/SpriteRendererComponent.h"

namespace XYZRoguelike
{
// Healing wall: restores the player while he stands next to it.
// Green walls restore health, yellow walls restore armor.
class HealingWallComponent : public XYZEngine::Component
{
  public:
    HealingWallComponent(XYZEngine::GameObject *gameObject, XYZEngine::GameObject *player, bool restoreArmor);

    void Update(float deltaTime) override;
    void Render() override;

  private:
    XYZEngine::TransformComponent *transform = nullptr;
    XYZEngine::TransformComponent *playerTransform = nullptr;
    XYZEngine::StatsComponent *playerStats = nullptr;
    XYZEngine::SpriteRendererComponent *renderer = nullptr;

    bool restoreArmor = false;
    float healTimer = 0.f;
    float pulseTimer = 0.f;
    float healRadius = 135.f;

    sf::Text label;
};
} // namespace XYZRoguelike
