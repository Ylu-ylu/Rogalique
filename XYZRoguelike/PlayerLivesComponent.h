#pragma once

#include "../Engine/Component.h"

namespace XYZRoguelike
{
// Respawn stock: each death while lives remain respawns the player with
// full stats instead of restarting the level. The last life lost means
// the level restarts (see PlayerDeathComponent).
class PlayerLivesComponent : public XYZEngine::Component
{
  public:
    PlayerLivesComponent(XYZEngine::GameObject *gameObject);

    void Update(float deltaTime) override;
    void Render() override;

    int GetLives() const;
    bool HasLives() const;

    void LoseLife();
    void ResetLives();

  private:
    int lives = 3;
};
} // namespace XYZRoguelike
