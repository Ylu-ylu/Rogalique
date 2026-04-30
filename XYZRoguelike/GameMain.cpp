#include <SFML/Graphics.hpp>
#include "Player.h"
#include "Engine.h"
#include "ResourceSystem.h"
#include "DeveloperLevel.h"
#include "Matrix2D.h"
#include <cassert>
#include <stdexcept>
#include <fstream>
#include <map>
#include <SFML/Audio/SoundBuffer.hpp>
#include <Logger.h>
// #include <windows.h>

using namespace XYZRoguelike;

int main()
{
    try
    {
        // Create main window
        auto *window = new sf::RenderWindow(sf::VideoMode(1280, 720), "XYZRoguelike");
        if (window == nullptr)
        {
            LOG_ERROR("Failed to allocate main window");
            throw std::runtime_error("Window allocation failed");
        }
        XYZEngine::RenderSystem::Instance()->SetMainWindow(window);

        // Pre-check resource files (warn if missing before attempting to load)
        auto warnIfMissing = [](const std::string &path) {
            std::ifstream f(path);
            if (!f.good())
            {
                LOG_WARN(std::string("Resource file missing: ") + path);
            }
        };

        warnIfMissing("Resources/TextureMaps/Wizard.png");
        warnIfMissing("Resources/TextureMaps/Man.png");
        warnIfMissing("Resources/TextureMaps/Floor.png");
        warnIfMissing("Resources/TextureMaps/Wall.png");
        warnIfMissing("Resources/TextureMaps/Creeper.png");
        warnIfMissing("Resources/Sounds/neon-gaming.wav");

        LOG_INFO("Loading texture maps and sounds");

        // Load textures
        XYZEngine::ResourceSystem::Instance()->LoadTextureMap("ai", "Resources/TextureMaps/Wizard.png", {46, 140}, 1, false);
        if (XYZEngine::ResourceSystem::Instance()->GetTextureMapElementShared("ai", 0) == nullptr)
        {
            LOG_ERROR("Texture map 'ai' failed to load or is empty");
            throw std::runtime_error("Failed to load required texture map: ai");
        }

        XYZEngine::ResourceSystem::Instance()->LoadTextureMap("Player", "Resources/TextureMaps/Man.png", {45, 140}, 1, false);
        if (XYZEngine::ResourceSystem::Instance()->GetTextureMapElementShared("Player", 0) == nullptr)
        {
            LOG_ERROR("Texture map 'Player' failed to load or is empty");
            throw std::runtime_error("Failed to load required texture map: Player");
        }

        XYZEngine::ResourceSystem::Instance()->LoadTextureMap("Creeper", "Resources/TextureMaps/Creeper.png", {45, 140}, 1, false);
        if (XYZEngine::ResourceSystem::Instance()->GetTextureMapElementShared("Creeper", 0) == nullptr)
        {
            LOG_ERROR("Texture map 'Player' failed to load or is empty");
            throw std::runtime_error("Failed to load required texture map: Creeper");
        }

        ResourceSystem::Instance()->LoadTextureMap("level_floors", "Resources/TextureMaps/Floor.png", {16, 16}, 49, false);
        if (ResourceSystem::Instance()->GetTextureMapElementShared("level_floors", 0) == nullptr)
        {
            LOG_ERROR("Texture map 'level_floors' failed to load or is empty");
            throw std::runtime_error("Failed to load required texture map: level_floors");
        }

        ResourceSystem::Instance()->LoadTextureMap("level_walls", "Resources/TextureMaps/Wall.png", {16, 16}, 48, false);
        if (ResourceSystem::Instance()->GetTextureMapElementShared("level_walls", 0) == nullptr)
        {
            LOG_ERROR("Texture map 'level_walls' failed to load or is empty");
            throw std::runtime_error("Failed to load required texture map: level_walls");
        }

        // Load and validate sound
        XYZEngine::ResourceSystem::Instance()->LoadSound("music", "Resources/Sounds/neon-gaming.wav");
        if (XYZEngine::ResourceSystem::Instance()->GetSound("music") == nullptr)
        {
            LOG_ERROR("Sound 'music' failed to load");
            throw std::runtime_error("Failed to load required sound: music");
        }

        LOG_INFO("Initializing DeveloperLevel");
        auto developerLevel = std::make_shared<DeveloperLevel>();
        if (developerLevel == nullptr)
        {
            LOG_ERROR("Failed to create DeveloperLevel instance");
            throw std::runtime_error("DeveloperLevel allocation failed");
        }
        developerLevel->Start();

        LOG_INFO("Starting engine run");
        XYZEngine::Engine::Instance()->Run();

        LOG_INFO("Game exiting");
        return 0;
    }
    catch (const std::exception &ex)
    {
        LOG_ERROR(std::string("Unhandled exception: ") + ex.what());
        return EXIT_FAILURE;
    }
    catch (...)
    {
        LOG_ERROR("Unhandled non-standard exception");
        return EXIT_FAILURE;
    }
}
