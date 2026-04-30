#pragma once

#include "Component.h"

namespace XYZEngine
{
class StatsComponent : public Component
{
  public:
    explicit StatsComponent(GameObject *gameObject);
    StatsComponent(GameObject *gameObject, float health, float armor);

    void Update(float deltaTime) override;
    void Render() override;

    float GetCurrentHealth() const;
    float GetMaxHealth() const;
    float GetCurrentArmor() const;
    float GetMaxArmor() const;

    void SetCurrentHealth(float health);
    void SetMaxHealth(float health);
    void SetCurrentArmor(float armor);
    void SetMaxArmor(float armor);

    void TakeDamage(float damage);
    void Heal(float amount);
    void RepairArmor(float amount);

  private:
    float currentHealth = 100.0f;
    float maxHealth = 100.0f;
    float currentArmor = 0.0f;
    float maxArmor = 0.0f;
};
} // namespace XYZEngine