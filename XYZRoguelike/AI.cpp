
#include "AI.h"
#include <ResourceSystem.h>
#include "RigidbodyComponent.h"
#include "SpriteColliderComponent.h"
#include "FollowComponent.h"
#include "Player.h"
#include "DeveloperLevel.h"
#include <StatsComponent.h>
#include <AttackComponen.h>


namespace XYZRoguelike
{
	AI::AI(const XYZEngine::Vector2Df& position, XYZEngine::GameObject* player)
	{
		gameObject = XYZEngine::GameWorld::Instance()->CreateGameObject("ai");
		auto transform = gameObject->GetComponent<XYZEngine::TransformComponent>();
		transform->SetWorldPosition(position);

		auto enemyRenderer = gameObject->AddComponent<XYZEngine::SpriteRendererComponent>();
		enemyRenderer->SetTexture(*XYZEngine::ResourceSystem::Instance()->GetTextureMapElementShared("ai", 0));
		enemyRenderer->SetPixelSize(100, 100);

		auto enemyCamera = gameObject->AddComponent<XYZEngine::CameraComponent>();
		enemyCamera->SetWindow(&XYZEngine::RenderSystem::Instance()->GetMainWindow());
		enemyCamera->SetBaseResolution(1280, 720);

		//Add Follow Component
		DeveloperLevel developerLevel;
		auto follower = gameObject->AddComponent<XYZEngine::FollowComponent>();
		
		follower->SetTarget(player);
		follower->SetSpeed(120.f);

		//auto transform = gameObject->GetComponent<XYZEngine::TransformComponent>();

		auto rigidbody = gameObject->AddComponent<XYZEngine::RigidbodyComponent>();
		rigidbody->SetKinematic(false);

		auto collider = gameObject->AddComponent<XYZEngine::SpriteColliderComponent>();

		//Add health, damage, armor stats
		auto statsComponent = gameObject->AddComponent<XYZEngine::StatsComponent>(maxArmor, maxArmor);

		//Add AttackComponent
		auto attackComponent = gameObject->AddComponent<XYZEngine::AttackComponent>(attackPower);

	}

	XYZEngine::GameObject* AI::GetGameObject()
	{
		return gameObject;
	}
}