#include "HealingWallComponent.h"
#include "../Engine/GameObject.h"
#include "../Engine/RenderSystem.h"

#include <cmath>

namespace
{
// shared font for all healing wall labels (loaded once)
sf::Font &sharedLabelFont()
{
    static sf::Font font;
    static bool loaded = font.loadFromFile("Resources/Fonts/Roboto-Bold.ttf");
    (void)loaded;
    return font;
}
} // namespace

namespace XYZRoguelike
{
HealingWallComponent::HealingWallComponent(XYZEngine::GameObject *gameObject, XYZEngine::GameObject *player, bool restoreArmor)
    : XYZEngine::Component(gameObject), restoreArmor(restoreArmor)
{
    transform = gameObject->GetComponent<XYZEngine::TransformComponent>();
    renderer = gameObject->GetComponent<XYZEngine::SpriteRendererComponent>();

    if (player != nullptr)
    {
        playerTransform = player->GetComponent<XYZEngine::TransformComponent>();
        playerStats = player->GetComponent<XYZEngine::StatsComponent>();
    }

    label.setFont(sharedLabelFont());
    label.setString(restoreArmor ? "+Armor" : "+HP");
    label.setCharacterSize(45);
    label.setFillColor(sf::Color::White);
    label.setOutlineColor(sf::Color::Black);
    label.setOutlineThickness(3.f);

    // the game camera uses an inverted Y view, so the text must be flipped
    label.setScale(1.f, -1.f);

    if (transform != nullptr)
    {
        sf::FloatRect bounds = label.getLocalBounds();
        label.setOrigin(bounds.left + bounds.width * 0.5f, bounds.top + bounds.height * 0.5f);

        XYZEngine::Vector2Df wallPosition = transform->GetWorldPosition();
        label.setPosition(wallPosition.x, wallPosition.y);
    }
}

void HealingWallComponent::Update(float deltaTime)
{
    if (transform == nullptr || playerTransform == nullptr || playerStats == nullptr)
    {
        return;
    }

    XYZEngine::Vector2Df wallPosition = transform->GetWorldPosition();
    XYZEngine::Vector2Df playerPosition = playerTransform->GetWorldPosition();

    float dx = wallPosition.x - playerPosition.x;
    float dy = wallPosition.y - playerPosition.y;
    float distance = std::sqrt(dx * dx + dy * dy);

    if (distance < healRadius)
    {
        healTimer -= deltaTime;

        if (healTimer <= 0.f)
        {
            healTimer = 0.25f;

            if (restoreArmor)
            {
                playerStats->RepairArmor(1.f);
            }
            else
            {
                playerStats->Heal(1.f);
            }
        }
    }
    else
    {
        healTimer = 0.f;
    }

    pulseTimer += deltaTime;
    if (renderer != nullptr)
    {
        float alpha = 170.f + 70.f * std::sin(pulseTimer * 3.f);
        if (restoreArmor)
        {
            renderer->SetColor(sf::Color(255, 220, 60, static_cast<sf::Uint8>(alpha)));
        }
        else
        {
            renderer->SetColor(sf::Color(120, 255, 120, static_cast<sf::Uint8>(alpha)));
        }
    }
}

void HealingWallComponent::Render()
{
    XYZEngine::RenderSystem::Instance()->Render(label);
}
} // namespace XYZRoguelike
