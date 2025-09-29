#include "Player.h"
#include <ResourceSystem.h>
#include "RigidbodyComponent.h"
#include "SpriteColliderComponent.h"
#include <MovementComponent.h>
#include <StatsComponent.h>
#include <AttackComponen.h>


namespace XYZRoguelike
{
	Player::Player(const XYZEngine::Vector2Df& position)
	{
		gameObject = XYZEngine::GameWorld::Instance()->CreateGameObject("Player");
		auto transform = gameObject->GetComponent<XYZEngine::TransformComponent>();
		transform->SetWorldPosition(position);

		auto playerRenderer = gameObject->AddComponent<XYZEngine::SpriteRendererComponent>();
		playerRenderer->SetTexture(*XYZEngine::ResourceSystem::Instance()->GetTextureMapElementShared("Player",0));
		playerRenderer->SetPixelSize(100, 100);

		auto playerCamera = gameObject->AddComponent<XYZEngine::CameraComponent>();
		playerCamera->SetWindow(&XYZEngine::RenderSystem::Instance()->GetMainWindow());
		playerCamera->SetBaseResolution(1280, 720);

		auto playerInput = gameObject->AddComponent<XYZEngine::InputComponent>();

		auto movement = gameObject->AddComponent<XYZEngine::MovementComponent>();
		movement->SetSpeed(400.f);

		auto rigidbody = gameObject->AddComponent<XYZEngine::RigidbodyComponent>();
		rigidbody->SetKinematic(false);

		auto collider = gameObject->AddComponent<XYZEngine::SpriteColliderComponent>();

		//Add health, damage, armor stats
		auto statsComponent=gameObject->AddComponent<XYZEngine::StatsComponent>(100.0f,50.0f);

		//Add AttackComponent
		auto attackComponent = gameObject->AddComponent<XYZEngine::AttackComponent>(10.0f);

	}

	XYZEngine::GameObject* Player::GetGameObject()
	{
		return gameObject;
	}
}