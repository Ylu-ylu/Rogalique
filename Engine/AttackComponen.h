#pragma once
#include "Component.h"
#include "GameObject.h"
#include "StatsComponent.h"

namespace XYZEngine
{
	class AttackComponent : public Component
	{
	public:
		AttackComponent(GameObject* gameObject, float attackPower):
			Component(gameObject), attackPower(attackPower) {}


		float GetAttackPower() const { return attackPower; }	
		void SetAttackPower(float newAttackPower) { attackPower = newAttackPower; }

		void Attack(GameObject* target);

		// Implement pure virtuals from Component
		void Update(float deltaTime) override;
		void Render() override;

	private:	
		
		float attackPower;

	};
}