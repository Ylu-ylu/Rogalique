#include "DeveloperLevel.h"
#include "Floor.h"
#include "Wall.h"
#include "MazeGenerator.h"
#include "GameSettings.h"
#include <cstdlib>

#include <ResourceSystem.h>
#include <SpriteRendererComponent.h>
#include <RigidbodyComponent.h>
#include <SpriteColliderComponent.h>

using namespace XYZEngine;

namespace XYZRoguelike
{
void DeveloperLevel::ChooseExitTile(int width, int height, int &outExitX, int &outExitY) const
{
    const int side = std::rand() % 4; // 0-left, 1-right, 2-bottom, 3-top

    if (side == 0)
    {
        outExitX = 0;
        outExitY = 1 + (std::rand() % (height - 1));
    }
    else if (side == 1)
    {
        outExitX = width;
        outExitY = 1 + (std::rand() % (height - 1));
    }
    else if (side == 2)
    {
        outExitY = 0;
        outExitX = 1 + (std::rand() % (width - 1));
    }
    else
    {
        outExitY = height;
        outExitX = 1 + (std::rand() % (width - 1));
    }
}

void DeveloperLevel::CreateExitTrigger(int exitX, int exitY)
{
    auto exitObject = XYZEngine::GameWorld::Instance()->CreateGameObject("LevelExit");

    auto transform = exitObject->GetComponent<XYZEngine::TransformComponent>();
    transform->SetWorldPosition({exitX * 128.f, exitY * 128.f});

    auto renderer = exitObject->AddComponent<XYZEngine::SpriteRendererComponent>();
    renderer->SetTexture(*XYZEngine::ResourceSystem::Instance()->GetTextureMapElementShared("level_floors", 0));
    renderer->SetPixelSize(128, 128);
    renderer->SetColor(sf::Color(120, 255, 120, 220)); // visual marker

    auto rigidbody = exitObject->AddComponent<XYZEngine::RigidbodyComponent>();
    rigidbody->SetKinematic(true);

    auto collider = exitObject->AddComponent<XYZEngine::SpriteColliderComponent>();
    collider->SetTrigger(true);

    auto playerCollider = player->GetGameObject()->GetComponent<XYZEngine::SpriteColliderComponent>();
    if (playerCollider != nullptr)
    {
        playerCollider->SubscribeTriggerEnter([this](XYZEngine::Trigger) { LoadNextLevel(); });
    }
}

void DeveloperLevel::LoadNextLevel()
{
    if (isLevelTransitionInProgress)
    {
        return;
    }

    isLevelTransitionInProgress = true;
    currentLevel++;
    Restart();
}

void DeveloperLevel::Start()
{
    // harder level => larger map
    int width = 15 + (currentLevel - 1) * 2;
    int height = 15 + (currentLevel - 1) * 2;

    int exitX = 0;
    int exitY = 0;
    ChooseExitTile(width, height, exitX, exitY);

    // Build maze walls/floors
    for (int y = 0; y < height + 1; y++)
    {
        for (int x = 0; x < width + 1; x++)
        {
            const bool isExitTile = (x == exitX && y == exitY);

            if (x != 0 && x != width && y != 0 && y != height)
            {
                floors.push_back(std::make_unique<Floor>(std::forward<XYZEngine::Vector2Df>({x * 128.f, y * 128.f}), std::forward<int>(0)));
            }

            if (x == 0 && y == 0 && !isExitTile)
            {
                walls.push_back(std::make_unique<Wall>(std::forward<XYZEngine::Vector2Df>({x * 128.f, y * 128.f}), std::forward<int>(25)));
            }

            if (x == width && y == 0 && !isExitTile)
            {
                walls.push_back(std::make_unique<Wall>(std::forward<XYZEngine::Vector2Df>({x * 128.f, y * 128.f}), std::forward<int>(27)));
            }

            if (x == 0 && y == height && !isExitTile)
            {
                walls.push_back(std::make_unique<Wall>(std::forward<XYZEngine::Vector2Df>({x * 128.f, y * 128.f}), std::forward<int>(1)));
            }

            if (x == width && y == height && !isExitTile)
            {
                walls.push_back(std::make_unique<Wall>(std::forward<XYZEngine::Vector2Df>({x * 128.f, y * 128.f}), std::forward<int>(3)));
            }

            if (x == 0 && y != height && y != 0)
            {
                floors.push_back(std::make_unique<Floor>(std::forward<XYZEngine::Vector2Df>({x * 128.f, y * 128.f}), std::forward<int>(18)));
                if (!isExitTile)
                {
                    walls.push_back(std::make_unique<Wall>(std::forward<XYZEngine::Vector2Df>({x * 128.f, y * 128.f}), std::forward<int>(12)));
                }
            }

            if (x == width && y != height && y != 0)
            {
                floors.push_back(std::make_unique<Floor>(std::forward<XYZEngine::Vector2Df>({x * 128.f, y * 128.f}), std::forward<int>(19)));
                if (!isExitTile)
                {
                    walls.push_back(std::make_unique<Wall>(std::forward<XYZEngine::Vector2Df>({x * 128.f, y * 128.f}), std::forward<int>(12)));
                }
            }

            if (y == 0 && x != width && x != 0 && !isExitTile)
            {
                walls.push_back(std::make_unique<Wall>(std::forward<XYZEngine::Vector2Df>({x * 128.f, y * 128.f}), std::forward<int>(38)));
            }

            if (y == height && x != width && x != 0 && !isExitTile)
            {
                walls.push_back(std::make_unique<Wall>(std::forward<XYZEngine::Vector2Df>({x * 128.f, y * 128.f}), std::forward<int>(38)));
            }
        }
    }

    // ensure exit has walkable visual tile
    floors.push_back(std::make_unique<Floor>(std::forward<XYZEngine::Vector2Df>({exitX * 128.f, exitY * 128.f}), std::forward<int>(0)));

    MazeGenerator mazeGenerator(width, height, this);
    mazeGenerator.Generate();

    player = std::make_shared<Player>(std::forward<XYZEngine::Vector2Df>({width / 2 * 128.f, height / 2 * 128.f}));

    auto camera = player->GetGameObject()->GetComponent<XYZEngine::CameraComponent>();
    if (camera != nullptr)
    {
        const float mapWidth = static_cast<float>((width + 1) * 128);
        const float mapHeight = static_cast<float>((height + 1) * 128);

        const float windowAspect = static_cast<float>(SETTINGS.SCREEN_WIDTH) / static_cast<float>(SETTINGS.SCREEN_HEIGHT);
        const float mapAspect = mapWidth / mapHeight;

        int viewWidth = 0;
        int viewHeight = 0;

        if (mapAspect > windowAspect)
        {
            viewWidth = static_cast<int>(mapWidth);
            viewHeight = static_cast<int>(mapWidth / windowAspect);
        }
        else
        {
            viewHeight = static_cast<int>(mapHeight);
            viewWidth = static_cast<int>(mapHeight * windowAspect);
        }

        camera->SetBaseResolution(viewWidth, viewHeight);
    }

    ai = std::make_shared<AI>(std::forward<XYZEngine::Vector2Df>({width / 3 * 128.f, height / 3 * 128.f}), player->GetGameObject());

    creeperSpawner = std::make_unique<CreeperSpawner>();

    // difficulty scales by level
    const int baseCount = 2 + currentLevel;
    creeperSpawner->SpawnCreepers(baseCount, width, height, mazeGenerator.GetGrid(), player->GetGameObject());

    SpawnConfig customConfig;
    customConfig.count = 1 + currentLevel / 2;
    customConfig.healthMultiplier = 1.0f + currentLevel * 0.25f;
    customConfig.armorMultiplier = 1.0f + currentLevel * 0.15f;
    customConfig.speedMultiplier = 1.0f + currentLevel * 0.10f;
    customConfig.damageMultiplier = 1.0f + currentLevel * 0.20f;
    customConfig.enemyTypeName = "Scaled Creeper";
    creeperSpawner->SpawnEnemies(customConfig, width, height, mazeGenerator.GetGrid(), player->GetGameObject());

    creeperSpawner->SpawnWave(currentLevel, width, height, mazeGenerator.GetGrid(), player->GetGameObject());

    if (currentLevel % 3 == 0)
    {
        creeperSpawner->SpawnBoss(width, height, mazeGenerator.GetGrid(), player->GetGameObject());
    }

    CreateExitTrigger(exitX, exitY);

    music = std::make_unique<Music>("music");
    isLevelTransitionInProgress = false;
}

void DeveloperLevel::Restart()
{
    Stop();
    Start();
}

void DeveloperLevel::Stop()
{
    if (creeperSpawner)
    {
        creeperSpawner->Clear();
    }

    walls.clear();
    floors.clear();
    ai.reset();
    player.reset();
    music.reset();

    XYZEngine::GameWorld::Instance()->Clear();
}

std::shared_ptr<Player> DeveloperLevel::GetPlayer()
{
    return player;
}
} // namespace XYZRoguelike