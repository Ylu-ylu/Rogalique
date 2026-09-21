#pragma once

#include "../Engine/Component.h"
#include "../Engine/TransformComponent.h"
#include "../Engine/StatsComponent.h"

#include <SFML/Graphics.hpp>

namespace XYZRoguelike
{
class HealthBarComponent : public XYZEngine::Component
{
  public:
    HealthBarComponent(XYZEngine::GameObject *gameObject);

    void Update(float deltaTime) override;
    void Render() override;

    void SetOffset(float x, float y);
    void SetSize(float width, float height);
    void UseArmor(bool value);
    void SetFillColor(const sf::Color &color);

  private:
    XYZEngine::TransformComponent *transform = nullptr;
    XYZEngine::StatsComponent *stats = nullptr;

    bool useArmor = false;

    XYZEngine::Vector2Df offset = {0.f, 80.f};
    XYZEngine::Vector2Df size = {70.f, 8.f};

    sf::RectangleShape background;
    sf::RectangleShape fill;
};
} // namespace XYZRoguelike
