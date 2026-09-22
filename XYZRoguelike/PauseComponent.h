#pragma once

#include "../Engine/Component.h"

#include <SFML/Graphics.hpp>

namespace XYZRoguelike
{
// Draws the PAUSED overlay while the engine is paused (Esc toggles the pause
// in Engine::Run). Render is called even when the world is frozen, so the
// overlay is the only thing that keeps drawing on top of the frozen frame.
class PauseComponent : public XYZEngine::Component
{
  public:
    PauseComponent(XYZEngine::GameObject *gameObject);

    void Update(float deltaTime) override;
    void Render() override;

  private:
    sf::Font font;
    sf::Text text;
    sf::RectangleShape overlay;
};
} // namespace XYZRoguelike
