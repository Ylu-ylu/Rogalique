#pragma once

#include "../Engine/Component.h"
#include "../Engine/StatsComponent.h"
#include "../Engine/AttackComponen.h"
#include "../Engine/MovementComponent.h"

#include <SFML/Graphics.hpp>

namespace XYZRoguelike
{
class CreeperSpawner;

// Potion inventory: health (1), rage (2), wave (3). Max 5 each,
// restocked to full on every level transition.
class InventoryComponent : public XYZEngine::Component
{
  public:
    InventoryComponent(XYZEngine::GameObject *gameObject);

    void Update(float deltaTime) override;
    void Render() override;

    void SetEnemySpawner(CreeperSpawner *spawner);
    void ResetStocks();

    int GetHealthPotions() const;
    int GetRagePotions() const;
    int GetWavePotions() const;

  private:
    XYZEngine::StatsComponent *stats = nullptr;
    XYZEngine::AttackComponent *attack = nullptr;
    XYZEngine::MovementComponent *movement = nullptr;
    XYZEngine::TransformComponent *transform = nullptr;
    CreeperSpawner *enemySpawner = nullptr;

    // stocks (max 5 each, reset on level transition)
    static constexpr int MAX_STOCK = 5;
    int healthPotions = MAX_STOCK;
    int ragePotions = MAX_STOCK;
    int wavePotions = MAX_STOCK;

    // rage buff
    bool rageActive = false;
    float rageTimer = 0.f;
    float baseAttackPower = 10.f;
    float baseSpeed = 400.f;

    // wave visual
    float waveTimer = 0.f;
    sf::CircleShape waveRing;

    bool keyOnePressedLastFrame = false;
    bool keyTwoPressedLastFrame = false;
    bool keyThreePressedLastFrame = false;

    bool UseHealthPotion();
    bool UseRagePotion();
    bool UseWavePotion();
};
} // namespace XYZRoguelike
