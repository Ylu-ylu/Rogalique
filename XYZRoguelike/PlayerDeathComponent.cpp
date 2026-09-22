#include "PlayerDeathComponent.h"
#include "DeveloperLevel.h"
#include "PlayerLivesComponent.h"
#include "../Engine/GameObject.h"
#include "../Engine/GameWorld.h"
#include "../Engine/TransformComponent.h"
#include "../Engine/SpriteMovementAnimationComponent.h"
#include "../Engine/ResourceSystem.h"
#include "../Engine/RenderSystem.h"
#include "../Engine/Logger.h"

#include <SFML/Window/Keyboard.hpp>

namespace XYZRoguelike
{
PlayerDeathComponent::PlayerDeathComponent(XYZEngine::GameObject *gameObject, DeveloperLevel *level)
    : XYZEngine::Component(gameObject), level(level)
{
    stats = gameObject->GetComponent<XYZEngine::StatsComponent>();

    deathSound = gameObject->AddComponent<XYZEngine::AudioComponent>();
    const sf::SoundBuffer *buffer = XYZEngine::ResourceSystem::Instance()->GetSound("Player-death");
    if (buffer != nullptr)
    {
        deathSound->SetAudio(*buffer);
        deathSound->SetLoop(false);
    }
}

void PlayerDeathComponent::Update(float deltaTime)
{
    if (stats == nullptr || level == nullptr)
    {
        return;
    }

    if (stats->GetCurrentHealth() <= 0.f)
    {
        auto lives = gameObject->GetComponent<PlayerLivesComponent>();

        if (lives != nullptr && lives->HasLives())
        {
            // a life is spent, the player respawns with full stats at the
            // level center instead of restarting the level
            lives->LoseLife();

            stats->SetCurrentHealth(stats->GetMaxHealth());
            stats->SetCurrentArmor(stats->GetMaxArmor());

            auto transform = gameObject->GetComponent<XYZEngine::TransformComponent>();
            if (transform != nullptr)
            {
                // maze spawn point (width / 2 * 128 for the constant 15x15 maze)
                transform->SetWorldPosition({896.f, 896.f});
            }

            if (deathSound != nullptr)
            {
                deathSound->Stop();
                deathSound->Play();
            }

            LOG_INFO("Player respawned with a spare life");
            return;
        }

        if (!deathStarted)
        {
            deathStarted = true;
            deathTimer = 2.f;

            auto animation = gameObject->GetComponent<XYZEngine::SpriteMovementAnimationComponent>();
            if (animation != nullptr)
            {
                animation->Play("death");
            }

            if (deathSound != nullptr)
            {
                deathSound->Stop();
                deathSound->Play();
            }

            LOG_INFO("Player died - game over");
        }

        if (gameOverActive)
        {
            // R - restart the level, ESC - close the game (edge-detected)
            bool rPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::R);
            bool escPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Escape);

            if (rPressed && !rPressedLastFrame)
            {
                gameOverActive = false;
                XYZEngine::GameWorld::Instance()->EnqueueLateAction([level = level]() { level->Restart(); });
                return;
            }

            if (escPressed && !escPressedLastFrame)
            {
                XYZEngine::RenderSystem::Instance()->GetMainWindow().close();
                return;
            }

            rPressedLastFrame = rPressed;
            escPressedLastFrame = escPressed;
            return;
        }

        deathTimer -= deltaTime;

        if (deathTimer <= 0.f)
        {
            gameOverActive = true;

            if (font.loadFromFile("Resources/Fonts/Roboto-Bold.ttf"))
            {
                text.setFont(font);
            }

            text.setCharacterSize(48);
            text.setFillColor(sf::Color::White);
            text.setString("GAME OVER\n\nR - RESTART\nESC - EXIT");

            sf::FloatRect bounds = text.getLocalBounds();
            text.setOrigin(bounds.left + bounds.width * 0.5f, bounds.top + bounds.height * 0.5f);
            text.setPosition(640.f, 360.f);

            overlay.setSize(sf::Vector2f(1280.f, 720.f));
            overlay.setFillColor(sf::Color(0, 0, 0, 170));

            LOG_INFO("GAME OVER");
        }
    }
}

void PlayerDeathComponent::Render()
{
    if (!gameOverActive)
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
