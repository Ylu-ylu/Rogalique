#pragma once
#include "AI.h"
#include "Vector.h"
#include <functional>
#include <memory>
#include <vector>

namespace XYZEngine
{
class GameObject;
class TransformComponent;

} // namespace XYZEngine

namespace XYZRoguelike
{
// ✅ СТРУКТУРА КОНФИГУРАЦИИ СПАУНА
struct SpawnConfig
{
    int count = 1;                        // Количество врагов
    float healthMultiplier = 1.0f;        // Множитель здоровья
    float armorMultiplier = 1.0f;         // Множитель брони
    float speedMultiplier = 1.0f;         // Множитель скорости
    float damageMultiplier = 1.0f;        // Множитель урона
    float minDistanceFromPlayer = 300.0f; // Минимальная дистанция от игрока

    // Фабрика для создания врагов (по умолчанию - Creeper)
    std::function<std::unique_ptr<AI>(XYZEngine::GameObject *, const std::string &, int)> enemyFactory;

    // Имя типа врага (для логирования)
    std::string enemyTypeName = "Creeper";
};

// ✅ ПРЕДУСТАНОВЛЕННЫЕ КОНФИГУРАЦИИ
namespace SpawnPresets
{
SpawnConfig Normal(int count);
SpawnConfig Hard(int count);
SpawnConfig Boss();
SpawnConfig Wave(int waveNumber);
} // namespace SpawnPresets

class CreeperSpawner
{
  public:
    CreeperSpawner();

    void SpawnEnemies(
        const SpawnConfig &config, int mazeWidth, int mazeHeight, const std::vector<std::vector<bool>> &grid, XYZEngine::GameObject *playerTarget);

    void SpawnWave(int waveNumber, int mazeWidth, int mazeHeight, const std::vector<std::vector<bool>> &grid, XYZEngine::GameObject *playerTarget);

    void SpawnBoss(int mazeWidth, int mazeHeight, const std::vector<std::vector<bool>> &grid, XYZEngine::GameObject *playerTarget);

    void SpawnCreepers(int count, int mazeWidth, int mazeHeight, const std::vector<std::vector<bool>> &grid, XYZEngine::GameObject *playerTarget);

    const std::vector<std::unique_ptr<AI>> &GetEnemies() const;
    int GetAliveCount() const;

    void Clear();

  private:
    std::vector<std::unique_ptr<AI>> enemies;
    XYZEngine::GameObject *cachedPlayer = nullptr;

    XYZEngine::Vector2Df GetRandomValidPosition(int mazeWidth,
                                                int mazeHeight,
                                                const std::vector<std::vector<bool>> &grid,
                                                float minDistanceFromPlayer = 0.0f);

    // Добавлено: расчет расстояния
    float CalculateDistance(const XYZEngine::Vector2Df &pos1, const XYZEngine::Vector2Df &pos2) const;
};
} // namespace XYZRoguelike
