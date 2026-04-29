#pragma once

#include "AI.h"

namespace XYZRoguelike
{
class Creeper : public AI
{
  public:
    Creeper(const XYZEngine::Vector2Df &position, XYZEngine::GameObject *target);
    Creeper(XYZEngine::GameObject *player, const std::string &enemyName, int id);

    std::unique_ptr<AI> Clone(XYZEngine::Vector2Df spawnPosition, const std::string &enemyName, int id) const override;

  private:
    XYZEngine::GameObject *gameObject = nullptr;

    // Stats
    static constexpr float DEFAULT_HEALTH = 50.0f;
    static constexpr float DEFAULT_ARMOR = 20.0f;
    static constexpr float DEFAULT_ATTACK = 15.0f;
    static constexpr float DEFAULT_SPEED = 150.0f;
};
} // namespace XYZRoguelike