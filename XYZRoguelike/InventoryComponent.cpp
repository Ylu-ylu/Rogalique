#include "InventoryComponent.h"
#include "CreeperSpawner.h"
#include "../Engine/GameObject.h"
#include "../Engine/TransformComponent.h"
#include "../Engine/RenderSystem.h"
#include "../Engine/Logger.h"

#include <SFML/Window/Keyboard.hpp>

#include <cmath>
#include <string>
#include <vector>

namespace XYZRoguelike
{
InventoryComponent::InventoryComponent(XYZEngine::GameObject *gameObject) : XYZEngine::Component(gameObject)
{
    stats = gameObject->GetComponent<XYZEngine::StatsComponent>();
    attack = gameObject->GetComponent<XYZEngine::AttackComponent>();
    movement = gameObject->GetComponent<XYZEngine::MovementComponent>();
    transform = gameObject->GetComponent<XYZEngine::TransformComponent>();

    waveRing.setFillColor(sf::Color(100, 200, 255, 40));
    waveRing.setOutlineColor(sf::Color(120, 220, 255, 180));
    waveRing.setOutlineThickness(4.f);
}

void InventoryComponent::SetEnemySpawner(CreeperSpawner *spawner)
{
    enemySpawner = spawner;
}

void InventoryComponent::ResetStocks()
{
    healthPotions = MAX_STOCK;
    ragePotions = MAX_STOCK;
    wavePotions = MAX_STOCK;

    // cancel rage if active
    if (rageActive && attack != nullptr && movement != nullptr)
    {
        attack->SetAttackPower(baseAttackPower);
        movement->SetSpeed(baseSpeed);
        rageActive = false;
    }

    waveTimer = 0.f;

    LOG_INFO("Potion stocks restocked");
}

int InventoryComponent::GetHealthPotions() const
{
    return healthPotions;
}

int InventoryComponent::GetRagePotions() const
{
    return ragePotions;
}

int InventoryComponent::GetWavePotions() const
{
    return wavePotions;
}

bool InventoryComponent::UseHealthPotion()
{
    if (healthPotions <= 0 || stats == nullptr)
    {
        return false;
    }

    float hpMissing = stats->GetMaxHealth() - stats->GetCurrentHealth();
    float armorMissing = stats->GetMaxArmor() - stats->GetCurrentArmor();

    if (hpMissing <= 0.f && armorMissing <= 0.f)
    {
        LOG_INFO("Already at full HP and Armor");
        return false;
    }

    constexpr float potionPower = 25.f;

    if (hpMissing >= potionPower)
    {
        stats->Heal(potionPower);
    }
    else
    {
        stats->Heal(hpMissing);
        float leftover = potionPower - hpMissing;
        stats->RepairArmor(leftover);
    }

    healthPotions--;
    LOG_INFO("Health potion used. Left: " + std::to_string(healthPotions));
    return true;
}

bool InventoryComponent::UseRagePotion()
{
    if (ragePotions <= 0)
    {
        return false;
    }

    if (rageActive)
    {
        rageTimer = 5.f;
        ragePotions--;
        LOG_INFO("Rage potion refreshed. Left: " + std::to_string(ragePotions));
        return true;
    }

    if (attack != nullptr && movement != nullptr)
    {
        baseAttackPower = attack->GetAttackPower();
        baseSpeed = movement->GetSpeed();

        attack->SetAttackPower(baseAttackPower * 1.5f);
        movement->SetSpeed(baseSpeed * 1.5f);

        rageActive = true;
        rageTimer = 5.f;
        ragePotions--;

        LOG_INFO("Rage potion used. Left: " + std::to_string(ragePotions));
        return true;
    }

    return false;
}

bool InventoryComponent::UseWavePotion()
{
    if (wavePotions <= 0 || transform == nullptr)
    {
        return false;
    }

    auto position = transform->GetWorldPosition();

    // damage all enemies in radius 250
    if (enemySpawner != nullptr)
    {
        std::vector<XYZEngine::GameObject *> enemies;
        enemySpawner->FindEnemiesInRange(position, 250.f, enemies);

        for (XYZEngine::GameObject *enemy : enemies)
        {
            auto enemyStats = enemy->GetComponent<XYZEngine::StatsComponent>();
            if (enemyStats != nullptr)
            {
                enemyStats->TakeDamage(30.f);
            }
        }

        LOG_INFO("Wave potion hit " + std::to_string(enemies.size()) + " enemies");
    }

    // visual
    waveTimer = 0.5f;
    waveRing.setPosition(position.x, position.y);
    waveRing.setRadius(1.f);
    waveRing.setOrigin(1.f, 1.f);

    wavePotions--;
    LOG_INFO("Wave potion used. Left: " + std::to_string(wavePotions));
    return true;
}

void InventoryComponent::Update(float deltaTime)
{
    // rage timer
    if (rageActive)
    {
        rageTimer -= deltaTime;

        if (rageTimer <= 0.f && attack != nullptr && movement != nullptr)
        {
            attack->SetAttackPower(baseAttackPower);
            movement->SetSpeed(baseSpeed);
            rageActive = false;
            LOG_INFO("Rage potion expired");
        }
    }

    // wave visual timer
    if (waveTimer > 0.f)
    {
        waveTimer -= deltaTime;

        float progress = 1.f - (waveTimer / 0.5f);
        if (progress > 1.f)
        {
            progress = 1.f;
        }

        float radius = 250.f * progress;
        waveRing.setRadius(radius);
        waveRing.setOrigin(radius, radius);
    }

    // hotkeys 1/2/3 (edge-detected)
    bool keyOne = sf::Keyboard::isKeyPressed(sf::Keyboard::Num1);
    bool keyTwo = sf::Keyboard::isKeyPressed(sf::Keyboard::Num2);
    bool keyThree = sf::Keyboard::isKeyPressed(sf::Keyboard::Num3);

    if (keyOne && !keyOnePressedLastFrame)
    {
        UseHealthPotion();
    }

    if (keyTwo && !keyTwoPressedLastFrame)
    {
        UseRagePotion();
    }

    if (keyThree && !keyThreePressedLastFrame)
    {
        UseWavePotion();
    }

    keyOnePressedLastFrame = keyOne;
    keyTwoPressedLastFrame = keyTwo;
    keyThreePressedLastFrame = keyThree;
}

void InventoryComponent::Render()
{
    if (waveTimer > 0.f)
    {
        XYZEngine::RenderSystem::Instance()->Render(waveRing);
    }
}
} // namespace XYZRoguelike
