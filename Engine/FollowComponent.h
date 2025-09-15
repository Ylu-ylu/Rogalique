#pragma once
#include "Component.h"
#include "TransformComponent.h"
#include "SpriteRendererComponent.h"
#include "ResourceSystem.h"

namespace XYZEngine
{
	class FollowComponent : public Component
	{
	public:
		FollowComponent(GameObject* gameObject);
		void Update(float deltaTime) override;
		void Render() override;
		

		void SetTarget(GameObject* targetObject);
		void SetSpeed(float newSpeed);
		float getSpeed() const;

	private:
		TransformComponent* transform = nullptr;
		TransformComponent* targetTransform = nullptr;
		SpriteRendererComponent* spriteRenderer = nullptr;

		//Render
		std::vector<const sf::Texture*> textureMap;
		float secondsForFrame = 0.f;

		float speed = 100.f; //default speed
	};
}