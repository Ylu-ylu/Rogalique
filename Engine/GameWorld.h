#pragma once

#include "GameObject.h"
#include "PhysicsSystem.h"
#include <functional>

namespace XYZEngine
{
class GameWorld
{
  public:
    static GameWorld *Instance();

    void Update(float deltaTime);
    void FixedUpdate(float deltaTime);
    void Render();
    void LateUpdate();

    GameObject *CreateGameObject();
    GameObject *CreateGameObject(std::string name);
    void DestroyGameObject(GameObject *gameObject);
    void EnqueueLateAction(std::function<void()> action);
    void Clear();

    void Print() const;

  private:
    GameWorld()
    {
    }
    ~GameWorld()
    {
    }

    GameWorld(GameWorld const &) = delete;
    GameWorld &operator=(GameWorld const &) = delete;

    float fixedCounter = 0.f;

    std::vector<GameObject *> gameObjects = {};
    std::vector<GameObject *> markedToDestroyGameObjects = {};
    std::vector<std::function<void()>> lateActions = {};

    void DestroyGameObjectImmediate(GameObject *gameObject);
};
} // namespace XYZEngine