#include "DeveloperLevel.h"
#include "Floor.h"
#include "Wall.h"
#include "MazeGenerator.h"
#include "PlayerDeathComponent.h"
#include "PlayerAttackComponent.h"
#include "BossDeathComponent.h"
#include "HealingWallComponent.h"
#include "GameHUDComponent.h"
#include "ExitLabelComponent.h"
#include "BossAbilitiesComponent.h"
#include "InventoryComponent.h"
#include "GameConstants.h"
#include "GameSettings.h"
#include <cstdlib>
#include <algorithm>
#include "../Engine/ResourceSystem.h"
#include "../Engine/SpriteRendererComponent.h"
#include "../Engine/RigidbodyComponent.h"
#include "../Engine/SpriteColliderComponent.h"
#include "../Engine/FollowComponent.h"
#include "../Engine/AudioComponent.h"
#include "../Engine/Logger.h"

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
    // gray while closed: the exit opens only after the boss is defeated
    renderer->SetColor(sf::Color(120, 120, 120, 220));

    auto rigidbody = exitObject->AddComponent<XYZEngine::RigidbodyComponent>();
    rigidbody->SetKinematic(true);

    auto collider = exitObject->AddComponent<XYZEngine::SpriteColliderComponent>();
    collider->SetTrigger(true);
    collider->SetEnabled(false);

    // "Exit" label: red while the bosses are alive, green once opened
    exitObject->AddComponent<ExitLabelComponent>();

    auto exitSound = exitObject->AddComponent<XYZEngine::AudioComponent>();
    const sf::SoundBuffer *soundBuffer = XYZEngine::ResourceSystem::Instance()->GetSound("Transition");
    if (soundBuffer != nullptr)
    {
        exitSound->SetAudio(*soundBuffer);
        exitSound->SetLoop(false);

        if (currentLevel > 1)
        {
            exitSound->Play();
        }
    }

    auto playerCollider = player->GetGameObject()->GetComponent<XYZEngine::SpriteColliderComponent>();
    if (playerCollider != nullptr)
    {
        playerCollider->SubscribeTriggerEnter([this](XYZEngine::Trigger) {
            LOG_INFO("Level exit triggered");
            XYZEngine::GameWorld::Instance()->EnqueueLateAction([this]() { LoadNextLevel(); });
        });
    }

    this->exitObject = exitObject;
}

void DeveloperLevel::OpenExit()
{
    if (exitObject == nullptr)
    {
        return;
    }

    exitOpened = true;

    auto exitRenderer = exitObject->GetComponent<XYZEngine::SpriteRendererComponent>();
    if (exitRenderer != nullptr)
    {
        exitRenderer->SetColor(sf::Color(120, 255, 120, 220));
    }

    auto exitCollider = exitObject->GetComponent<XYZEngine::SpriteColliderComponent>();
    if (exitCollider != nullptr)
    {
        exitCollider->SetEnabled(true);
    }

    auto exitLabel = exitObject->GetComponent<ExitLabelComponent>();
    if (exitLabel != nullptr)
    {
        exitLabel->SetOpened(true);
    }

    LOG_INFO("Boss defeated - exit opened");
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
    // maze size stays the same every level, only the wall layout changes
    int width = 15;
    int height = 15;

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

    MazeGenerator mazeGenerator(width, height, this, exitX, exitY);
    mazeGenerator.Generate();

    player = std::make_shared<Player>(std::forward<XYZEngine::Vector2Df>({width / 2 * 128.f, height / 2 * 128.f}));
    player->GetGameObject()->AddComponent<PlayerDeathComponent>(this);
    player->GetGameObject()->AddComponent<PlayerAttackComponent>();

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

    // healing walls: green restore health, yellow restore armor; the count
    // doubles each level (1 -> 2 -> 4 -> 8) and is capped at 10 per color
    {
        std::vector<XYZEngine::GameObject *> innerWalls;
        for (auto &wall : walls)
        {
            auto wallTransform = wall->GetGameObject()->GetComponent<XYZEngine::TransformComponent>();
            if (wallTransform == nullptr)
            {
                continue;
            }

            auto wallPosition = wallTransform->GetWorldPosition();
            const int wx = static_cast<int>(wallPosition.x / 128.f);
            const int wy = static_cast<int>(wallPosition.y / 128.f);

            if (wx >= 1 && wx <= width - 2 && wy >= 1 && wy <= height - 2)
            {
                innerWalls.push_back(wall->GetGameObject());
            }
        }

        const int wallsPerColor = std::min(1 << (currentLevel - 1), 10);

        for (int color = 0; color < 2 && !innerWalls.empty(); ++color)
        {
            const bool restoreArmor = (color == 1);

            for (int i = 0; i < wallsPerColor && !innerWalls.empty(); ++i)
            {
                const size_t index = std::rand() % innerWalls.size();
                XYZEngine::GameObject *wallObject = innerWalls[index];
                innerWalls.erase(innerWalls.begin() + index);

                wallObject->AddComponent<HealingWallComponent>(player->GetGameObject(), restoreArmor);
            }
        }
    }

    ai = std::make_shared<AI>(std::forward<XYZEngine::Vector2Df>({width / 3 * 128.f, height / 3 * 128.f}), player->GetGameObject());

    creeperSpawner = std::make_unique<CreeperSpawner>();
    creeperSpawner->ReservePosition(ai->GetPosition());
    creeperSpawner->AddExternalEnemy(ai.get());

    auto playerAttack = player->GetGameObject()->GetComponent<PlayerAttackComponent>();
    if (playerAttack != nullptr)
    {
        playerAttack->SetEnemySpawner(creeperSpawner.get());
    }

    auto playerInventory = player->GetGameObject()->GetComponent<InventoryComponent>();
    if (playerInventory != nullptr)
    {
        playerInventory->SetEnemySpawner(creeperSpawner.get());
    }

    // difficulty scales by level (creeper count reduced by 20%, rounded)
    const int baseCount = ((2 + currentLevel) * 4 + 2) / 5;
    creeperSpawner->SpawnCreepers(baseCount, width, height, mazeGenerator.GetGrid(), player->GetGameObject());

    SpawnConfig customConfig;
    customConfig.count = ((1 + currentLevel / 2) * 4 + 2) / 5;
    customConfig.healthMultiplier = 1.0f + currentLevel * 0.25f;
    customConfig.armorMultiplier = 1.0f + currentLevel * 0.15f;
    customConfig.speedMultiplier = 1.0f + currentLevel * 0.10f;
    customConfig.damageMultiplier = 1.0f + currentLevel * 0.20f;
    customConfig.enemyTypeName = "Scaled Creeper";
    creeperSpawner->SpawnEnemies(customConfig, width, height, mazeGenerator.GetGrid(), player->GetGameObject());

    creeperSpawner->SpawnWave(currentLevel, width, height, mazeGenerator.GetGrid(), player->GetGameObject());

    // bosses on every level: one per level, capped at four. The exit opens
    // only after ALL of them are defeated.
    const int bossCount = std::min(currentLevel, 4);
    bossObjects.clear();

    for (int i = 0; i < bossCount; ++i)
    {
        creeperSpawner->SpawnBoss(width, height, mazeGenerator.GetGrid(), player->GetGameObject());

        const auto &spawnedEnemies = creeperSpawner->GetEnemies();
        if (!spawnedEnemies.empty())
        {
            // the boss is spawned last by SpawnBoss
            XYZEngine::GameObject *bossObject = spawnedEnemies.back()->GetGameObject();
            bossObject->AddComponent<BossDeathComponent>(this);
            SetupBoss(bossObject);
            bossObjects.push_back(bossObject);
        }
    }

    CreateExitTrigger(exitX, exitY);

    auto hudObject = XYZEngine::GameWorld::Instance()->CreateGameObject("GameHUD");
    auto hud = hudObject->AddComponent<GameHUDComponent>(currentLevel, this);
    hud->SetTarget(player->GetGameObject());

    // potion inventory: keys 1/2/3, restocked every level
    auto inventory = player->GetGameObject()->GetComponent<InventoryComponent>();
    if (inventory == nullptr)
    {
        inventory = player->GetGameObject()->AddComponent<InventoryComponent>();
    }
    inventory->ResetStocks();
    inventory->SetEnemySpawner(creeperSpawner.get());

    music = std::make_unique<Music>("music");
    isLevelTransitionInProgress = false;
    exitOpened = false;
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
    exitObject = nullptr;
    bossObjects.clear();

    XYZEngine::GameWorld::Instance()->Clear();
}

void DeveloperLevel::SetupBoss(XYZEngine::GameObject *bossObject)
{
    // unique boss abilities: AOE with telegraph + rage below 50% HP
    bossObject->AddComponent<BossAbilitiesComponent>(player->GetGameObject());

    // restyle the boss with the Creeper texture set (same size as regular
    // enemies so he can follow the player between walls)
    auto bossRenderer = bossObject->GetComponent<XYZEngine::SpriteRendererComponent>();
    const sf::Texture *bossTexture =
        XYZEngine::ResourceSystem::Instance()->GetTextureMapElementShared(CREEPER_TEXTURE_KEY, DEFAULT_TEXTURE_INDEX);
    if (bossRenderer != nullptr && bossTexture != nullptr)
    {
        bossRenderer->SetTexture(*bossTexture);
        bossRenderer->SetColor(sf::Color(170, 0, 255, 255));

        const auto texSize = bossTexture->getSize();
        const int bossHeight = 128;
        const int desiredWidth = static_cast<int>(bossHeight * static_cast<float>(texSize.x) / static_cast<float>(texSize.y));
        bossRenderer->SetPixelSize(desiredWidth, bossHeight);
    }

    auto bossAnimation = bossObject->GetComponent<XYZEngine::SpriteMovementAnimationComponent>();
    if (bossAnimation != nullptr)
    {
        // boss walk is two times slower than the creepers' (0.2s per frame vs 0.1s)
        bossAnimation->AddAnimation("walk", CREEPER_TEXTURE_KEY, {0, 1, 2, 3, 4, 5, 6}, 0.2f, true);
        bossAnimation->AddAnimation("idle", CREEPER_TEXTURE_KEY, {DEFAULT_TEXTURE_INDEX}, 0.1f, true);
        bossAnimation->AddAnimation("attack", CREEPER_TEXTURE_KEY, {7, 8, 9, 10, 11}, 0.08f, false);
        bossAnimation->AddAnimation("death", CREEPER_TEXTURE_KEY, {12}, 0.15f, false);
        bossAnimation->Play("idle");
    }

    // boss moves two times slower than the creepers (120 -> 60)
    auto bossFollower = bossObject->GetComponent<XYZEngine::FollowComponent>();
    if (bossFollower != nullptr)
    {
        bossFollower->SetSpeed(60.f);
    }

    // collider that fits the 128px corridors (same size as the player's)
    auto bossCollider = bossObject->GetComponent<XYZEngine::SpriteColliderComponent>();
    if (bossCollider != nullptr)
    {
        bossCollider->SetSize(122.f, 128.f);
    }
}

void DeveloperLevel::OnBossDied()
{
    // the exit opens only when every boss of the level is dead
    for (XYZEngine::GameObject *boss : bossObjects)
    {
        if (boss == nullptr)
        {
            continue;
        }

        auto bossStats = boss->GetComponent<XYZEngine::StatsComponent>();
        if (bossStats != nullptr && bossStats->GetCurrentHealth() > 0.f)
        {
            return;
        }
    }

    OpenExit();
}

std::shared_ptr<Player> DeveloperLevel::GetPlayer()
{
    return player;
}
} // namespace XYZRoguelike