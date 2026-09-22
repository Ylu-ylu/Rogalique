#pragma once

#include "../Engine/Component.h"
#include "../Engine/StatsComponent.h"
#include "../Engine/AudioComponent.h"

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
    XYZEngine::AudioComponent *deathSound = nullptr;

    bool dying = false;
    float deathTimer = 0.f;
};
} // namespace XYZRoguelike
