// XYZRoguelike\Creeper.cpp
#include "Creeper.h"
#include <ResourceSystem.h>
#include "RigidbodyComponent.h"
#include "SpriteColliderComponent.h"
#include "FollowComponent.h"
#include <StatsComponent.h>
#include <AttackComponen.h>
#include <GameWorld.h>
#include "GameSettings.h" // Для доступа к SETTINGS

namespace XYZRoguelike
{
Creeper::Creeper(const XYZEngine::Vector2Df &position, XYZEngine::GameObject *target) : AI(position, target)
{
    // Create GameObject
    gameObject = XYZEngine::GameWorld::Instance()->CreateGameObject("Creeper");
    auto transform = gameObject->GetComponent<XYZEngine::TransformComponent>();
    transform->SetWorldPosition(position);

    // Add Sprite Renderer (use AI texture temporarily or create new one)
    auto renderer = gameObject->AddComponent<XYZEngine::SpriteRendererComponent>();
    renderer->SetTexture(*XYZEngine::ResourceSystem::Instance()->GetTextureMapElementShared("Creeper", 0));
    renderer->SetPixelSize(80, 80); // Slightly smaller than AI/Player

    // Add Follow Component (chase player)
    auto follower = gameObject->AddComponent<XYZEngine::FollowComponent>();
    follower->SetTarget(target);
    follower->SetSpeed(DEFAULT_SPEED);

    // Add Physics
    auto rigidbody = gameObject->AddComponent<XYZEngine::RigidbodyComponent>();
    rigidbody->SetKinematic(false);

    // Add Collider
    auto collider = gameObject->AddComponent<XYZEngine::SpriteColliderComponent>();

    // Add Stats (health, armor)
    auto statsComponent = gameObject->AddComponent<XYZEngine::StatsComponent>(DEFAULT_HEALTH, DEFAULT_ARMOR);

    // Add Attack Component
    auto attackComponent = gameObject->AddComponent<XYZEngine::AttackComponent>(DEFAULT_ATTACK);
}

Creeper::Creeper(XYZEngine::GameObject *player, const std::string &enemyName, int id) : AI(player, enemyName, id)
{
    SetColor(sf::Color::Red);
}

std::unique_ptr<AI> Creeper::Clone(XYZEngine::Vector2Df spawnPosition, const std::string &enemyName, int id) const
{
    auto clone = std::make_unique<Creeper>(fallowTarget, enemyName, id);
    clone->SetPosition(spawnPosition);
    return clone;
}

// Пример корректной инициализации позиции крипера
float creeperWidth = 80.0f;  // или получите из renderer->GetPixelWidth()
float creeperHeight = 80.0f; // или получите из renderer->GetPixelHeight()

float x = static_cast<float>(rand() % (SETTINGS.SCREEN_WIDTH - static_cast<int>(creeperWidth)));
float y = static_cast<float>(rand() % (SETTINGS.SCREEN_HEIGHT - static_cast<int>(creeperHeight)));

// XYZEngine::TransformComponent *transform = gameObject->GetComponent<XYZEngine::TransformComponent>();

} // namespace XYZRoguelike