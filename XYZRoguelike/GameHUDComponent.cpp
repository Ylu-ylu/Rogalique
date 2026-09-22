#include "GameHUDComponent.h"
#include "DeveloperLevel.h"
#include "../Engine/GameObject.h"
#include "../Engine/RenderSystem.h"
#include "../Engine/ResourceSystem.h"
#include <string>

namespace XYZRoguelike
{

GameHUDComponent::GameHUDComponent(XYZEngine::GameObject *gameObject, int levelNumber, DeveloperLevel *level)
    : XYZEngine::Component(gameObject), levelNumber(levelNumber), level(level)
{
    if (font.loadFromFile("Resources/Fonts/Roboto-Bold.ttf"))
    {
        text.setFont(font);
        statusText.setFont(font);
    }

    text.setCharacterSize(26);
    text.setFillColor(sf::Color::White);
    text.setPosition(20.f, 20.f);

    statusText.setCharacterSize(26);
    statusText.setFillColor(sf::Color(255, 120, 40));
    statusText.setPosition(20.f, 116.f);

    heartTexture = XYZEngine::ResourceSystem::Instance()->GetTextureShared("Heart");
    if (heartTexture != nullptr)
    {
        heartSprite.setTexture(*heartTexture);

        const auto textureSize = heartTexture->getSize();
        const float heartSize = 30.f;
        heartSprite.setScale(heartSize / static_cast<float>(textureSize.x), heartSize / static_cast<float>(textureSize.y));

    }
}

void GameHUDComponent::SetTarget(XYZEngine::GameObject *player)
{
    if (player == nullptr)
    {
        return;
    }

    stats = player->GetComponent<XYZEngine::StatsComponent>();
    lives = player->GetComponent<PlayerLivesComponent>();
}

void GameHUDComponent::Update(float deltaTime)
{
    std::string statsLine;
    if (stats != nullptr)
    {
        statsLine = "HP: " + std::to_string(static_cast<int>(stats->GetCurrentHealth())) + "/" +
                    std::to_string(static_cast<int>(stats->GetMaxHealth())) + "  Armor: " +
                    std::to_string(static_cast<int>(stats->GetCurrentArmor())) + "/" +
                    std::to_string(static_cast<int>(stats->GetMaxArmor()));
    }

    // line 2 is reserved for the heart icons drawn in Render
    text.setString("Level: " + std::to_string(levelNumber) + "\n \n" + statsLine);

    if (level != nullptr)
    {
        if (level->IsExitOpened())
        {
            statusText.setString("Go to Exit!");
            statusText.setFillColor(sf::Color(80, 255, 80));
        }
        else
        {
            statusText.setString("Kill Boss for Exit!");
            statusText.setFillColor(sf::Color(255, 120, 40));
        }
    }
}

void GameHUDComponent::Render()
{
    auto &window = XYZEngine::RenderSystem::Instance()->GetMainWindow();

    sf::View currentView = window.getView();

    window.setView(window.getDefaultView());

    window.draw(text);
    window.draw(statusText);

    if (heartTexture != nullptr && lives != nullptr)
    {
        for (int i = 0; i < lives->GetLives(); ++i)
        {
            heartSprite.setPosition(20.f + i * 36.f, 50.f);
            window.draw(heartSprite);
        }
    }

    window.setView(currentView);
}
} // namespace XYZRoguelike
