#pragma once

#include "../Engine/Component.h"

#include <SFML/Graphics.hpp>

namespace XYZRoguelike
{
// "Exit" label over the maze exit tile: red while the boss is alive,
// green once the exit is opened (all bosses defeated).
class ExitLabelComponent : public XYZEngine::Component
{
  public:
    ExitLabelComponent(XYZEngine::GameObject *gameObject);

    void Update(float deltaTime) override;
    void Render() override;

    void SetOpened(bool opened);

  private:
    sf::Text label;
};
} // namespace XYZRoguelike
