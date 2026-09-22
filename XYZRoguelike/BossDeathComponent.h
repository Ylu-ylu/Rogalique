#pragma once

#include "../Engine/Component.h"
#include "../Engine/StatsComponent.h"

namespace XYZRoguelike
{
class DeveloperLevel;

// Opens the level exit when the boss dies (the exit stays closed while the
// boss is alive). Deferred through EnqueueLateAction, like the level restart.
class BossDeathComponent : public XYZEngine::Component
{
  public:
    BossDeathComponent(XYZEngine::GameObject *gameObject, DeveloperLevel *level);

    void Update(float deltaTime) override;
    void Render() override;

  private:
    XYZEngine::StatsComponent *stats = nullptr;
    DeveloperLevel *level = nullptr;

    bool fired = false;
};
} // namespace XYZRoguelike
