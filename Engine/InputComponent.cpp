#include "pch.h"
#include "InputComponent.h"
#include "Logger.h"

namespace XYZEngine
{
InputComponent::InputComponent(GameObject *gameObject) : Component(gameObject)
{
}

void InputComponent::Update(float deltaTime)
{
    verticalAxis = 0.f;
    horizontalAxis = 0.f;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    {
        verticalAxis += 1.0f;
        LOG_INFO("Move forward");
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
        verticalAxis -= 1.0f;
        LOG_INFO("Move backward");
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        horizontalAxis += 1.0f;
        LOG_INFO("Move right");
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        horizontalAxis -= 1.0f;
        LOG_INFO("Move left");
    }
}
void InputComponent::Render()
{
}

float InputComponent::GetHorizontalAxis() const
{
    return horizontalAxis;
}
float InputComponent::GetVerticalAxis() const
{
    return verticalAxis;
}
} // namespace XYZEngine