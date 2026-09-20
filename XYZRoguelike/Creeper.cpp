// XYZRoguelike\Creeper.cpp
#include "Creeper.h"
#include "GameConstants.h"
#include "../Engine/ResourceSystem.h"
#include "../Engine/RigidbodyComponent.h"
#include "../Engine/SpriteColliderComponent.h"
#include "../Engine/FollowComponent.h"
#include "../Engine/SpriteDirectionComponent.h"
#include "../Engine/StatsComponent.h"
#include "../Engine/AttackComponen.h"
#include "../Engine/GameWorld.h"
#include "GameSettings.h"

namespace XYZRoguelike
{
// Constructs a Creeper enemy with sprite renderer, follow behavior, physics, collider, and combat stats
Creeper::Creeper(const XYZEngine::Vector2Df &position, XYZEngine::GameObject *target) : AI(position, target)
{
    // Create GameObject
    gameObject = XYZEngine::GameWorld::Instance()->CreateGameObject(CREEPER_GAMEOBJECT_NAME);
    auto transform = gameObject->GetComponent<XYZEngine::TransformComponent>();
    transform->SetWorldPosition(position);

    // Add Sprite Renderer (use AI texture temporarily or create new one)
    auto renderer = gameObject->AddComponent<XYZEngine::SpriteRendererComponent>();
    renderer->SetTexture(*XYZEngine::ResourceSystem::Instance()->GetTextureMapElementShared(CREEPER_TEXTURE_KEY, DEFAULT_TEXTURE_INDEX));
    renderer->SetPixelSize(CREEPER_TEXTURE_WIDTH, CREEPER_TEXTURE_HEIGHT);

    // Add Follow Component (chase player)
    auto follower = gameObject->AddComponent<XYZEngine::FollowComponent>();
    follower->SetTarget(target);
    follower->SetSpeed(CREEPER_SPEED);

    gameObject->AddComponent<XYZEngine::SpriteDirectionComponent>();

    // Add Physics
    auto rigidbody = gameObject->AddComponent<XYZEngine::RigidbodyComponent>();
    rigidbody->SetKinematic(false);

    // Add Collider
    auto collider = gameObject->AddComponent<XYZEngine::SpriteColliderComponent>();

    // Add Stats (health, armor)
    auto statsComponent = gameObject->AddComponent<XYZEngine::StatsComponent>(CREEPER_HEALTH, CREEPER_ARMOR);

    // Add Attack Component
    auto attackComponent = gameObject->AddComponent<XYZEngine::AttackComponent>(CREEPER_ATTACK);
}

// Constructs a Creeper from template with player reference, enemy name, and unique ID
Creeper::Creeper(XYZEngine::GameObject *player, const std::string &enemyName, int id) : AI(player, enemyName, id)
{
    SetColor(sf::Color::Red);
}

// Creates a copy of this Creeper at the specified spawn position with given name and ID
std::unique_ptr<AI> Creeper::Clone(XYZEngine::Vector2Df spawnPosition, const std::string &enemyName, int id) const
{
    auto clone = std::make_unique<Creeper>(fallowTarget, enemyName, id);
    clone->SetPosition(spawnPosition);
    return clone;
}

// Example of correct Creeper position initialization
float creeperWidth = 80.0f;  // or get from renderer->GetPixelWidth()
float creeperHeight = 80.0f; // ��� �������� �� renderer->GetPixelHeight()

float x = static_cast<float>(rand() % (SETTINGS.SCREEN_WIDTH - static_cast<int>(creeperWidth)));
float y = static_cast<float>(rand() % (SETTINGS.SCREEN_HEIGHT - static_cast<int>(creeperHeight)));

// XYZEngine::TransformComponent *transform = gameObject->GetComponent<XYZEngine::TransformComponent>();

} // namespace XYZRoguelike