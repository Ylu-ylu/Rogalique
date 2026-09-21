#pragma once

#include "../Engine/Component.h"
#include "../Engine/StatsComponent.h"

namespace XYZRoguelike
{
class EnemyDeathComponent : public XYZEngine::Component
{
  public:
    EnemyDeathComponent(XYZEngine::GameObject *gameObject);

    void Update(float deltaTime) override;
    void Render() override;

  private:
    XYZEngine::StatsComponent *stats = nullptr;

    bool dying = false;
    float deathTimer = 0.f;
};
} // namespace XYZRoguelike
