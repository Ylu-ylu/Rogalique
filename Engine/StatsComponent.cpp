#include "pch.h"
#include "StatsComponent.h"
#include "Logger.h"

namespace XYZEngine
{
StatsComponent::StatsComponent(GameObject *gameObject) : Component(gameObject)
{
}

StatsComponent::StatsComponent(GameObject *gameObject, float health, float armor)
    : Component(gameObject), currentHealth(health), maxHealth(health), currentArmor(armor), maxArmor(armor)
{
}

float StatsComponent::GetCurrentHealth() const
{
    return currentHealth;
}

float StatsComponent::GetMaxHealth() const
{
    return maxHealth;
}

float StatsComponent::GetCurrentArmor() const
{
    return currentArmor;
}

float StatsComponent::GetMaxArmor() const
{
    return maxArmor;
}

void StatsComponent::SetMaxHealth(float health)
{
    maxHealth = health;
    if (currentHealth > maxHealth)
    {
        currentHealth = maxHealth;
    }
}

void StatsComponent::SetCurrentHealth(float health)
{
    currentHealth = health;
    if (currentHealth > maxHealth)
    {
        currentHealth = maxHealth;
    }
    if (currentHealth < 0.0f)
    {
        currentHealth = 0.0f;
    }
}

void StatsComponent::SetMaxArmor(float armor)
{
    maxArmor = armor;
    if (currentArmor > maxArmor)
    {
        currentArmor = maxArmor;
    }
}

void StatsComponent::SetCurrentArmor(float armor)
{
    currentArmor = armor;
    if (currentArmor > maxArmor)
    {
        currentArmor = maxArmor;
    }
    if (currentArmor < 0.0f)
    {
        currentArmor = 0.0f;
    }
}

void StatsComponent::TakeDamage(float damage)
{
    if (damage <= 0.0f)
    {
        return;
    }

    float remainingDamage = damage;
    if (currentArmor > 0.0f)
    {
        if (currentArmor >= remainingDamage)
        {
            currentArmor -= remainingDamage;
            remainingDamage = 0.0f;
        }
        else
        {
            remainingDamage -= currentArmor;
            currentArmor = 0.0f;
        }
    }

    if (remainingDamage > 0.0f)
    {
        currentHealth -= remainingDamage;
        if (currentHealth < 0.0f)
        {
            currentHealth = 0.0f;
        }
    }

    LOG_INFO("Took " + std::to_string(damage) + " damage, Current health: " + std::to_string(currentHealth) +
             ", Current armor: " + std::to_string(currentArmor));
}

void StatsComponent::Heal(float amount)
{
    currentHealth += amount;
    if (currentHealth > maxHealth)
    {
        currentHealth = maxHealth;
    }

    LOG_INFO("Healed " + std::to_string(amount) + " amount, Current health: " + std::to_string(currentHealth));
}

void StatsComponent::RepairArmor(float amount)
{
    currentArmor += amount;
    if (currentArmor > maxArmor)
    {
        currentArmor = maxArmor;
    }

    LOG_INFO("Repaired " + std::to_string(amount) + " armor, Current armor: " + std::to_string(currentArmor));
}

void StatsComponent::Update(float deltaTime)
{
    (void)deltaTime;
}

void StatsComponent::Render()
{
}
} // namespace XYZEngine