#include "PauseComponent.h"
#include "../Engine/Engine.h"
#include "../Engine/RenderSystem.h"

namespace XYZRoguelike
{
PauseComponent::PauseComponent(XYZEngine::GameObject *gameObject) : XYZEngine::Component(gameObject)
{
    if (font.loadFromFile("Resources/Fonts/Roboto-Bold.ttf"))
    {
        text.setFont(font);
    }

    text.setCharacterSize(48);
    text.setFillColor(sf::Color::White);
    text.setString("PAUSED\n\nEsc - resume");

    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.left + bounds.width * 0.5f, bounds.top + bounds.height * 0.5f);
    text.setPosition(640.f, 360.f);

    overlay.setSize(sf::Vector2f(1280.f, 720.f));
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
}

void PauseComponent::Update(float deltaTime)
{
}

void PauseComponent::Render()
{
    if (!XYZEngine::Engine::Instance()->IsPaused())
    {
        return;
    }

    auto &window = XYZEngine::RenderSystem::Instance()->GetMainWindow();

    sf::View currentView = window.getView();

    window.setView(window.getDefaultView());

    window.draw(overlay);
    window.draw(text);

    window.setView(currentView);
}
} // namespace XYZRoguelike
