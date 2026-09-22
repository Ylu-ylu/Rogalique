#include "ExitLabelComponent.h"
#include "../Engine/GameObject.h"
#include "../Engine/TransformComponent.h"
#include "../Engine/RenderSystem.h"

namespace
{
sf::Font &sharedExitLabelFont()
{
    static sf::Font font;
    static bool loaded = font.loadFromFile("Resources/Fonts/Roboto-Bold.ttf");
    (void)loaded;
    return font;
}
} // namespace

namespace XYZRoguelike
{
ExitLabelComponent::ExitLabelComponent(XYZEngine::GameObject *gameObject) : XYZEngine::Component(gameObject)
{
    label.setFont(sharedExitLabelFont());
    label.setString("Exit");
    label.setCharacterSize(45);
    label.setFillColor(sf::Color(255, 60, 60));
    label.setOutlineColor(sf::Color::Black);
    label.setOutlineThickness(2.f);

    // the game camera uses an inverted Y view, so the text must be flipped
    label.setScale(1.f, -1.f);

    auto transform = gameObject->GetComponent<XYZEngine::TransformComponent>();
    if (transform != nullptr)
    {
        sf::FloatRect bounds = label.getLocalBounds();
        label.setOrigin(bounds.left + bounds.width * 0.5f, bounds.top + bounds.height * 0.5f);

        XYZEngine::Vector2Df position = transform->GetWorldPosition();
        label.setPosition(position.x, position.y + 40.f);
    }
}

void ExitLabelComponent::Update(float deltaTime)
{
}

void ExitLabelComponent::Render()
{
    XYZEngine::RenderSystem::Instance()->Render(label);
}

void ExitLabelComponent::SetOpened(bool opened)
{
    label.setFillColor(opened ? sf::Color(80, 255, 80) : sf::Color(255, 60, 60));
}
} // namespace XYZRoguelike
