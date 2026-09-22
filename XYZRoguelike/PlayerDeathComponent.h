#pragma once

#include "../Engine/Component.h"
#include "../Engine/StatsComponent.h"
#include "../Engine/AudioComponent.h"

#include <SFML/Graphics.hpp>

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
    XYZEngine::AudioComponent *deathSound = nullptr;
    DeveloperLevel *level = nullptr;

    bool deathStarted = false;
    bool deathQueued = false;
    float deathTimer = 0.f;

    bool gameOverActive = false;
    bool rPressedLastFrame = false;
    bool escPressedLastFrame = false;

    sf::Font font;
    sf::Text text;
    sf::RectangleShape overlay;
};
} // namespace XYZRoguelike
