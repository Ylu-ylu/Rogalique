#include "HealthBarComponent.h"
#include "../Engine/GameObject.h"
#include "../Engine/RenderSystem.h"

#include <algorithm>

namespace XYZRoguelike
{
HealthBarComponent::HealthBarComponent(XYZEngine::GameObject *gameObject) : XYZEngine::Component(gameObject)
{
    transform = gameObject->GetComponent<XYZEngine::TransformComponent>();
    stats = gameObject->GetComponent<XYZEngine::StatsComponent>();

    background.setFillColor(sf::Color(40, 40, 40));
    fill.setFillColor(sf::Color::Red);
}

void HealthBarComponent::Update(float deltaTime)
{
    if (transform == nullptr || stats == nullptr)
    {
        return;
    }

    float maxValue = useArmor ? stats->GetMaxArmor() : stats->GetMaxHealth();
    float currentValue = useArmor ? stats->GetCurrentArmor() : stats->GetCurrentHealth();

    float healthPercent = 0.f;
    if (maxValue > 0.f)
    {
        healthPercent = currentValue / maxValue;
        healthPercent = std::clamp(healthPercent, 0.0f, 1.0f);
    }

    XYZEngine::Vector2Df position = transform->GetWorldPosition();

    background.setSize(sf::Vector2f(size.x, size.y));
    background.setOrigin(size.x * 0.5f, size.y * 0.5f);
    background.setPosition(position.x + offset.x, position.y + offset.y);

    fill.setSize(sf::Vector2f(size.x * healthPercent, size.y));
    fill.setOrigin(size.x * 0.5f, size.y * 0.5f);
    fill.setPosition(position.x + offset.x, position.y + offset.y);
}

void HealthBarComponent::Render()
{
    XYZEngine::RenderSystem::Instance()->Render(background);
    XYZEngine::RenderSystem::Instance()->Render(fill);
}

void HealthBarComponent::SetOffset(float x, float y)
{
    offset = {x, y};
}

void HealthBarComponent::SetSize(float width, float height)
{
    size = {width, height};
}

void HealthBarComponent::UseArmor(bool value)
{
    useArmor = value;
}

void HealthBarComponent::SetFillColor(const sf::Color &color)
{
    fill.setFillColor(color);
}
} // namespace XYZRoguelike
