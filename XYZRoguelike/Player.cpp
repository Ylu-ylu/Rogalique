#include "Player.h"
#include <ResourceSystem.h>
#include "RigidbodyComponent.h"
#include "SpriteColliderComponent.h"
#include <MovementComponent.h>


namespace XYZRoguelike
{
	Player::Player(const XYZEngine::Vector2Df& position)
	{
		
		//Old code!!!!
		/*gameObject = XYZEngine::GameWorld::Instance()->CreateGameObject("Player");
		//gameObject = XYZEngine::GameWorld::Instance()->CreateGameObject();
		auto playerRenderer = gameObject->AddComponent<XYZEngine::SpriteRendererComponent>();

		playerRenderer->SetTexture(*XYZEngine::ResourceSystem::Instance()->GetTextureShared("Player"));
		playerRenderer->SetPixelSize(32, 32);

		auto playerCamera = gameObject->AddComponent<XYZEngine::CameraComponent>();
		playerCamera->SetWindow(&XYZEngine::RenderSystem::Instance()->GetMainWindow());
		playerCamera->SetBaseResolution(1280, 720);

		auto playerInput = gameObject->AddComponent<XYZEngine::InputComponent>();

		auto transform = gameObject->GetComponent<XYZEngine::TransformComponent>();*/

		/////
		/*transform->RotateBy(90.f);
		transform->MoveBy(1.f, 1.f);

		auto test=XYZEngine::GameWorld::Instance()->CreateGameObject();
		auto testTransform=test->GetComponent<XYZEngine::TransformComponent>();
		testTransform->SetParent(transform);

		transform->RotateBy(-90.f);
		transform->Print();
		testTransform->Print();*/

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
	}

	XYZEngine::GameObject* Player::GetGameObject()
	{
		return gameObject;
	}
}