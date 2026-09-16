#pragma once

#include "../Engine/CameraComponent.h"
#include "../Engine/GameWorld.h"
#include "../Engine/SpriteRendererComponent.h"
#include "../Engine/RenderSystem.h"
#include "../Engine/InputComponent.h"
#include "../Engine/GameObject.h"
#include "Vector.h"

namespace XYZRoguelike
{
class Player
{
  public:
    Player(const XYZEngine::Vector2Df &position);
    XYZEngine::GameObject *GetGameObject();

  private:
    XYZEngine::GameObject *gameObject;
};
} // namespace XYZRoguelike