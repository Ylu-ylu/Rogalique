#pragma once

#include "ColliderComponent.h"
#include "SpriteRendererComponent.h"
#include "RenderSystem.h"
#include <SFML/Graphics/Sprite.hpp>

namespace XYZEngine
{
class SpriteColliderComponent : public ColliderComponent
{
  public:
    SpriteColliderComponent(GameObject *gameObject);
    ~SpriteColliderComponent();

    void Update(float deltaTime) override;
    void Render() override;

    void SetSize(float width, float height);

  private:
    const sf::Sprite *sprite;

    bool useCustomSize = false;
    float customWidth = 0.f;
    float customHeight = 0.f;
};
} // namespace XYZEngine
