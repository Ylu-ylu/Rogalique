#pragma once

#include "../Engine/Component.h"
#include "../Engine/StatsComponent.h"

namespace XYZRoguelike
{
class DeveloperLevel;

class PlayerDeathComponent : public XYZEngine::Component
{
  public:
    PlayerDeathComponent(XYZEngine::GameObject *gameObject, DeveloperLevel *level);

    void Update(float deltaTime) override;
    void Render() override;

  private:
    XYZEngine::StatsComponent *stats = nullptr;
    DeveloperLevel *level = nullptr;

    bool deathStarted = false;
    bool deathQueued = false;
    float deathTimer = 0.f;
};
} // namespace XYZRoguelike
