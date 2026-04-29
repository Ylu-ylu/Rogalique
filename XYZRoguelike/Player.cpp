#include "Player.h"
#include <ResourceSystem.h>
#include "RigidbodyComponent.h"
#include "SpriteColliderComponent.h"
#include <MovementComponent.h>
#include <StatsComponent.h>
#include <AttackComponen.h>

namespace XYZRoguelike
{
Player::Player(const XYZEngine::Vector2Df &position)
{
    gameObject = XYZEngine::GameWorld::Instance()->CreateGameObject("Player");
    auto transform = gameObject->GetComponent<XYZEngine::TransformComponent>();
    transform->SetWorldPosition(position);

    auto playerRenderer = gameObject->AddComponent<XYZEngine::SpriteRendererComponent>();
    const sf::Texture *playerTexture = XYZEngine::ResourceSystem::Instance()->GetTextureMapElementShared("Player", 0);
    playerRenderer->SetTexture(*playerTexture);

    // keep texture aspect ratio (no squash)
    const float desiredHeight = 128.0f;
    const auto playerTexSize = playerTexture->getSize();
    const int desiredWidth = static_cast<int>(desiredHeight * static_cast<float>(playerTexSize.x) / static_cast<float>(playerTexSize.y));
    playerRenderer->SetPixelSize(desiredWidth, static_cast<int>(desiredHeight));

    auto playerCamera = gameObject->AddComponent<XYZEngine::CameraComponent>();
    playerCamera->SetWindow(&XYZEngine::RenderSystem::Instance()->GetMainWindow());
    playerCamera->SetBaseResolution(1280, 720);

    auto playerInput = gameObject->AddComponent<XYZEngine::InputComponent>();

    auto movement = gameObject->AddComponent<XYZEngine::MovementComponent>();
    movement->SetSpeed(400.f);

    auto rigidbody = gameObject->AddComponent<XYZEngine::RigidbodyComponent>();
    rigidbody->SetKinematic(false);

    auto collider = gameObject->AddComponent<XYZEngine::SpriteColliderComponent>();

    // Add health, damage, armor stats
    auto statsComponent = gameObject->AddComponent<XYZEngine::StatsComponent>(100.0f, 50.0f);

    // Add AttackComponent
    auto attackComponent = gameObject->AddComponent<XYZEngine::AttackComponent>(10.0f);
}

XYZEngine::GameObject *Player::GetGameObject()
{
    return gameObject;
}
} // namespace XYZRoguelike