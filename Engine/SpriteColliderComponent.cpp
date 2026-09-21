#include "pch.h"
#include "SpriteColliderComponent.h"

namespace XYZEngine
{
SpriteColliderComponent::SpriteColliderComponent(GameObject *gameObject) : ColliderComponent(gameObject), sprite(nullptr)
{
    auto spriteRenderer = gameObject->GetComponent<SpriteRendererComponent>();
    if (spriteRenderer == nullptr)
    {
        std::cout << "SpriteRenderer required to SpriteCollider." << std::endl;
        gameObject->RemoveComponent(this);
        return;
    }

    sprite = gameObject->GetComponent<SpriteRendererComponent>()->GetSprite();

    auto spriteBounds = sprite->getGlobalBounds();
    auto position = gameObject->GetComponent<TransformComponent>()->GetWorldPosition();
    bounds = {position.x - 0.5f * spriteBounds.width,
              position.y - 0.5f * spriteBounds.height,
              spriteBounds.width,
              spriteBounds.height};

    PhysicsSystem::Instance()->Subscribe(this);
}
SpriteColliderComponent::~SpriteColliderComponent()
{
    if (&bounds != nullptr)
    {
        std::destroy_at(&bounds);
    }
    PhysicsSystem::Instance()->Unsubscribe(this);
}

void SpriteColliderComponent::Update(float deltaTime)
{
    // Bounds must come from the transform: freshly created objects have not
    // been rendered yet, so the sprite position would still be (0, 0).
    auto spriteBounds = sprite->getGlobalBounds();
    auto position = gameObject->GetComponent<TransformComponent>()->GetWorldPosition();
    bounds = {position.x - 0.5f * spriteBounds.width,
              position.y - 0.5f * spriteBounds.height,
              spriteBounds.width,
              spriteBounds.height};
}
void SpriteColliderComponent::Render()
{
    sf::RectangleShape rectangle(sf::Vector2f(bounds.width, bounds.height));
    rectangle.setPosition(bounds.left, bounds.top);
    rectangle.setFillColor(sf::Color::Transparent);
    rectangle.setOutlineColor(sf::Color::White);
    rectangle.setOutlineThickness(4);

    RenderSystem::Instance()->Render(rectangle);
}
} // namespace XYZEngine