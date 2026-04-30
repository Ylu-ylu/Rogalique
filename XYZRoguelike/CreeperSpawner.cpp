#include "CreeperSpawner.h"
#include "Creeper.h"
#include "StatsComponent.h"
#include "TransformComponent.h"
#include <cstdlib>
#include <ctime>
#include <cmath>

namespace XYZRoguelike
{
// Add missing preset definitions (fixes unresolved externals)
namespace SpawnPresets
{
SpawnConfig Normal(int count)
{
    SpawnConfig config;
    config.count = count;
    config.healthMultiplier = 1.0f;
    config.armorMultiplier = 1.0f;
    config.speedMultiplier = 1.0f;
    config.damageMultiplier = 1.0f;
    config.minDistanceFromPlayer = 300.0f;
    config.enemyTypeName = "Creeper";
    return config;
}

SpawnConfig Hard(int count)
{
    SpawnConfig config;
    config.count = count;
    config.healthMultiplier = 1.5f;
    config.armorMultiplier = 1.4f;
    config.speedMultiplier = 1.2f;
    config.damageMultiplier = 1.35f;
    config.minDistanceFromPlayer = 350.0f;
    config.enemyTypeName = "Creeper";
    return config;
}

SpawnConfig Boss()
{
    SpawnConfig config;
    config.count = 1;
    config.healthMultiplier = 4.0f;
    config.armorMultiplier = 3.0f;
    config.speedMultiplier = 0.9f;
    config.damageMultiplier = 2.5f;
    config.minDistanceFromPlayer = 500.0f;
    config.enemyTypeName = "BossCreeper";
    return config;
}

SpawnConfig Wave(int waveNumber)
{
    if (waveNumber < 0)
    {
        waveNumber = 0;
    }

    SpawnConfig config;
    config.count = 3 + waveNumber;
    config.healthMultiplier = 1.0f + (0.15f * waveNumber);
    config.armorMultiplier = 1.0f + (0.10f * waveNumber);
    config.speedMultiplier = 1.0f + (0.05f * waveNumber);
    config.damageMultiplier = 1.0f + (0.12f * waveNumber);
    config.minDistanceFromPlayer = 300.0f;
    config.enemyTypeName = "Creeper";
    return config;
}
} // namespace SpawnPresets

CreeperSpawner::CreeperSpawner()
{
    // Initialize random seed (if not already done elsewhere)
    static bool seeded = false;
    if (!seeded)
    {
        std::srand(static_cast<unsigned>(std::time(nullptr)));
        seeded = true;
    }
}

void CreeperSpawner::SpawnCreepers(
    int count, int mazeWidth, int mazeHeight, const std::vector<std::vector<bool>> &grid, XYZEngine::GameObject *playerTarget)
{
    for (int i = 0; i < count; ++i)
    {
        // Get random valid position from maze grid
        XYZEngine::Vector2Df spawnPos = GetRandomValidPosition(mazeWidth, mazeHeight, grid);

        // Create creeper at position targeting player
        auto creeper = std::make_unique<Creeper>(playerTarget, "Creeper", i);
        creeper->SetPosition(spawnPos);
        // For compatibility, push to 'enemies' as AI*
        enemies.push_back(std::move(creeper));
    }
}

const std::vector<std::unique_ptr<AI>> &CreeperSpawner::GetEnemies() const
{
    return enemies;
}

int CreeperSpawner::GetAliveCount() const
{
    int count = 0;
    for (const auto &enemy : enemies)
    {
        if (enemy && enemy->GetGameObject())
        {
            auto stats = enemy->GetGameObject()->GetComponent<XYZEngine::StatsComponent>();
            if (stats && stats->GetCurrentHealth() > 0.0f)
            {
                count++;
            }
        }
    }
    return count;
}

void CreeperSpawner::Clear()
{
    enemies.clear();
}

float CreeperSpawner::CalculateDistance(const XYZEngine::Vector2Df &pos1, const XYZEngine::Vector2Df &pos2) const
{
    float dx = pos1.x - pos2.x;
    float dy = pos1.y - pos2.y;
    return std::sqrt(dx * dx + dy * dy);
}

XYZEngine::Vector2Df CreeperSpawner::GetRandomValidPosition(int mazeWidth,
                                                            int mazeHeight,
                                                            const std::vector<std::vector<bool>> &grid,
                                                            float minDistanceFromPlayer)
{
    // Собираем все валидные позиции
    std::vector<std::pair<int, int>> validPositions;
    for (int y = 0; y < mazeHeight; ++y)
    {
        for (int x = 0; x < mazeWidth; ++x)
        {
            if (y < grid.size() && x < grid[y].size() && grid[y][x])
            {
                // Проверка на минимальную дистанцию до игрока
                if (minDistanceFromPlayer > 0.0f && cachedPlayer != nullptr)
                {
                    auto transform = cachedPlayer->GetComponent<XYZEngine::TransformComponent>();
                    if (transform)
                    {
                        XYZEngine::Vector2Df playerPos = transform->GetWorldPosition();
                        XYZEngine::Vector2Df pos{(x + 1) * 128.0f, (y + 1) * 128.0f};
                        if (CalculateDistance(pos, playerPos) < minDistanceFromPlayer)
                            continue;
                    }
                }
                validPositions.emplace_back(x, y);
            }
        }
    }

    if (!validPositions.empty())
    {
        auto [x, y] = validPositions[std::rand() % validPositions.size()];
        return XYZEngine::Vector2Df{(x + 1) * 128.0f, (y + 1) * 128.0f};
    }

    // Если не найдено — fallback в центр карты
    return XYZEngine::Vector2Df{mazeWidth * 64.0f, mazeHeight * 64.0f};
}

void CreeperSpawner::SpawnEnemies(
    const SpawnConfig &config, int mazeWidth, int mazeHeight, const std::vector<std::vector<bool>> &grid, XYZEngine::GameObject *playerTarget)
{
    cachedPlayer = playerTarget;

    for (int i = 0; i < config.count; ++i)
    {
        XYZEngine::Vector2Df spawnPos = GetRandomValidPosition(mazeWidth, mazeHeight, grid, config.minDistanceFromPlayer);

        std::unique_ptr<AI> enemy;
        if (config.enemyFactory)
        {
            enemy = config.enemyFactory(playerTarget, config.enemyTypeName, i);
        }
        else
        {
            // Default to Creeper
            enemy = std::make_unique<Creeper>(playerTarget, "Creeper", i);
        }

        enemy->SetPosition(spawnPos);
        enemy->SetHealthMultiplier(config.healthMultiplier);
        enemy->SetArmorMultiplier(config.armorMultiplier);
        enemy->SetAttackMultiplier(config.damageMultiplier);
        enemy->SetSpeedMultiplier(config.speedMultiplier);

        enemies.push_back(std::move(enemy));
    }
}

void CreeperSpawner::SpawnWave(
    int waveNumber, int mazeWidth, int mazeHeight, const std::vector<std::vector<bool>> &grid, XYZEngine::GameObject *playerTarget)
{
    SpawnConfig config = SpawnPresets::Wave(waveNumber);
    SpawnEnemies(config, mazeWidth, mazeHeight, grid, playerTarget);
}

void CreeperSpawner::SpawnBoss(int mazeWidth, int mazeHeight, const std::vector<std::vector<bool>> &grid, XYZEngine::GameObject *playerTarget)
{
    SpawnConfig config = SpawnPresets::Boss();
    SpawnEnemies(config, mazeWidth, mazeHeight, grid, playerTarget);
}
} // namespace XYZRoguelike