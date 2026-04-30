#pragma once

#include <memory>
#include <string>
#include <SFML/Graphics/Color.hpp>
#include "GameObject.h"
#include "Vector.h"

namespace XYZRoguelike
{
class AI
{
  public:
    AI(const XYZEngine::Vector2Df &position, XYZEngine::GameObject *player);
    AI(XYZEngine::GameObject *player, const std::string &enemyName, int id);
    virtual ~AI() = default;

    AI(const AI &) = delete;
    AI &operator=(const AI &) = delete;

    XYZEngine::GameObject *GetGameObject() const;

    virtual std::unique_ptr<AI> Clone(XYZEngine::Vector2Df spawnPosition, const std::string &enemyName, int id) const;

    void SetPosition(const XYZEngine::Vector2Df &spawnPosition);
    void SetColor(const sf::Color &color);
    XYZEngine::Vector2Df GetPosition() const;

    // ✅ НОВЫЕ МЕТОДЫ для настройки характеристик
    void SetHealthMultiplier(float multiplier);
    void SetArmorMultiplier(float multiplier);
    void SetAttackMultiplier(float multiplier);
    void SetSpeedMultiplier(float multiplier);

    float GetMaxHealth() const
    {
        return maxHealth;
    }
    float GetMaxArmor() const
    {
        return maxArmor;
    }
    float GetAttackPower() const
    {
        return attackPower;
    }

  protected:
    XYZEngine::GameObject *fallowTarget = nullptr;

  private:
    XYZEngine::GameObject *gameObject = nullptr;

    float maxHealth = 100.f;
    float maxArmor = 50.f;
    float attackPower = 10.f;

    // Базовые значения для множителей
    float baseHealth = 100.f;
    float baseArmor = 50.f;
    float baseAttack = 10.f;
};
} // namespace XYZRoguelike