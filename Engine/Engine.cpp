#include "pch.h"
#include "Engine.h"
#include <iostream>
#include "GameWorld.h"
#include "RenderSystem.h"

#include <SFML/Window/Keyboard.hpp>

namespace XYZEngine
{
Engine *Engine::Instance()
{
    static Engine instance;
    return &instance;
}

Engine::Engine()
{
    unsigned int seed = (unsigned int)time(nullptr);
    srand(seed);

    setupLogger();
}

void Engine::SetPaused(bool value)
{
    isPaused = value;
}

bool Engine::IsPaused() const
{
    return isPaused;
}

void Engine::Run()
{
    sf::Clock gameClock;
    sf::Event event;

    LOG_INFO("Program was started!");

    RenderSystem::Instance()->GetMainWindow().setFramerateLimit(60);

    while (RenderSystem::Instance()->GetMainWindow().isOpen())
    {
        sf::Time dt = gameClock.restart();
        float deltaTime = dt.asSeconds();

        while (RenderSystem::Instance()->GetMainWindow().pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                RenderSystem::Instance()->GetMainWindow().close();
            }
        }

        if (!RenderSystem::Instance()->GetMainWindow().isOpen())
        {
            break;
        }

        // pause toggle: handled here so unpausing works while the world is frozen
        bool escPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Escape);
        if (escPressed && !escPressedLastFrame)
        {
            isPaused = !isPaused;
        }
        escPressedLastFrame = escPressed;

        RenderSystem::Instance()->GetMainWindow().clear();

        if (!isPaused)
        {
            GameWorld::Instance()->Update(deltaTime);
            GameWorld::Instance()->FixedUpdate(deltaTime);
        }

        GameWorld::Instance()->Render();

        if (!isPaused)
        {
            GameWorld::Instance()->LateUpdate();
        }

        RenderSystem::Instance()->GetMainWindow().display();
    }
}

void Engine::setupLogger()
{
    auto logger = std::make_shared<Logger>();
    logger->AddSink(std::make_shared<ConsoleSink>());
    logger->AddSink(std::make_shared<FileSink>("log.txt"));

    LoggerRegistry::getInstance().registerLogger("global", logger);
    LoggerRegistry::getInstance().setDefaultLogger(logger);
}
} // namespace XYZEngine