#include "Player.h"
#include "Player.h"
#include "GameConstants.h"
#include <ResourceSystem.h>
#include "RigidbodyComponent.h"
#include "SpriteColliderComponent.h"
#include <MovementComponent.h>
#include <SpriteDirectionComponent.h>
#include <StatsComponent.h>
#include <AttackComponen.h>

namespace XYZRoguelike
{
// Constructs the player game object with all necessary components including renderer, camera, input, movement, physics, and stats
Player::Player(const XYZEngine::Vector2Df &position)
{
    gameObject = XYZEngine::GameWorld::Instance()->CreateGameObject(PLAYER_GAMEOBJECT_NAME);
    auto transform = gameObject->GetComponent<XYZEngine::TransformComponent>();
    transform->SetWorldPosition(position);

    auto playerRenderer = gameObject->AddComponent<XYZEngine::SpriteRendererComponent>();
    const sf::Texture *playerTexture = XYZEngine::ResourceSystem::Instance()->GetTextureMapElementShared(PLAYER_TEXTURE_KEY, DEFAULT_TEXTURE_INDEX);
    playerRenderer->SetTexture(*playerTexture);

    // keep texture aspect ratio (no squash)
    const auto playerTexSize = playerTexture->getSize();
    const int desiredWidth = static_cast<int>(PLAYER_TEXTURE_HEIGHT * static_cast<float>(playerTexSize.x) / static_cast<float>(playerTexSize.y));
    playerRenderer->SetPixelSize(desiredWidth, static_cast<int>(PLAYER_TEXTURE_HEIGHT));

    auto playerCamera = gameObject->AddComponent<XYZEngine::CameraComponent>();
    playerCamera->SetWindow(&XYZEngine::RenderSystem::Instance()->GetMainWindow());
    playerCamera->SetBaseResolution(CAMERA_WIDTH, CAMERA_HEIGHT);

    auto playerInput = gameObject->AddComponent<XYZEngine::InputComponent>();

    auto movement = gameObject->AddComponent<XYZEngine::MovementComponent>();
    movement->SetSpeed(PLAYER_SPEED);

    gameObject->AddComponent<XYZEngine::SpriteDirectionComponent>();

    auto rigidbody = gameObject->AddComponent<XYZEngine::RigidbodyComponent>();
    rigidbody->SetKinematic(false);

    auto collider = gameObject->AddComponent<XYZEngine::SpriteColliderComponent>();

    // Add health, damage, armor stats
    auto statsComponent = gameObject->AddComponent<XYZEngine::StatsComponent>(PLAYER_HEALTH, PLAYER_ARMOR);

    // Add AttackComponent
    auto attackComponent = gameObject->AddComponent<XYZEngine::AttackComponent>(PLAYER_ATTACK);
}

// Gets the player's game object
XYZEngine::GameObject *Player::GetGameObject()
{
    return gameObject;
}

} // namespace XYZRoguelike