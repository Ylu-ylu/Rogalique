#include "pch.h"
#include "GameWorld.h"

namespace XYZEngine
{
// Gets the singleton instance of GameWorld
GameWorld *GameWorld::Instance()
{
    static GameWorld world;
    return &world;
}

// Updates all game objects each frame
// Called once per frame to update the logic of all objects
void GameWorld::Update(float deltaTime)
{
    for (int i = 0; i < gameObjects.size(); i++)
    {
        gameObjects[i]->Update(deltaTime);
    }
}
// Updates the physics system with a fixed time step
// Accumulates time and performs physics update when fixed time is reached
void GameWorld::FixedUpdate(float deltaTime)
{
    fixedCounter += deltaTime;
    if (fixedCounter > PhysicsSystem::Instance()->GetFixedDeltaTime())
        return;

        fixedCounter -= PhysicsSystem::Instance()->GetFixedDeltaTime();
        PhysicsSystem::Instance()->Update();

}
// Renders all game objects to the screen
// Called once per frame to visualize all objects
void GameWorld::Render()
{
    for (int i = 0; i < gameObjects.size(); i++)
    {
        gameObjects[i]->Render();
    }
}
// Performs deferred operations after all frame updates
// Runs queued actions first, then removes objects marked for destruction
void GameWorld::LateUpdate()
{
    while (!lateActions.empty())
    {
        auto action = lateActions.front();
        lateActions.erase(lateActions.begin());
        action();
    }

    for (int i = markedToDestroyGameObjects.size() - 1; i >= 0; i--)
    {
        DestroyGameObjectImmediate(markedToDestroyGameObjects[i]);
    }
}

// Defers an action to the end of the frame, after physics and rendering finished.
// Level transitions must use this: rebuilding the world inside a physics
// callback would mutate colliders while PhysicsSystem iterates them.
void GameWorld::EnqueueLateAction(std::function<void()> action)
{
    lateActions.push_back(action);
}

// Creates a new game object without a name and adds it to the world
// Returns a pointer to the created object
GameObject *GameWorld::CreateGameObject()
{
    GameObject *newGameObject = new GameObject();
    gameObjects.push_back(newGameObject);
    return newGameObject;
}
// Creates a new game object with the specified name and adds it to the world
// Returns a pointer to the created object
GameObject *GameWorld::CreateGameObject(std::string name)
{
    GameObject *newGameObject = new GameObject(name);
    gameObjects.push_back(newGameObject);
    return newGameObject;
}
// Marks a game object for destruction (will be removed in LateUpdate)
// This is a safe deletion method since it doesn't break iterators during loops
void GameWorld::DestroyGameObject(GameObject *gameObject)
{
    markedToDestroyGameObjects.push_back(gameObject);
}
// Clears the entire world: removes all root objects (those without a parent)
// Child objects are removed along with their parents
// Also resets the counter for physics
void GameWorld::Clear()
{
    for (int i = gameObjects.size() - 1; i >= 0; i--)
    {
        if (gameObjects[i] == nullptr)
        {
            continue;
        }

        if (gameObjects[i]->GetComponent<TransformComponent>()->GetParent() == nullptr)
        {
            DestroyGameObjectImmediate(gameObjects[i]);
        }
    }

    fixedCounter = 0.f;
}

// Prints the hierarchy of all objects in the world to console for debugging
// Only root objects (those without a parent) and their hierarchy are printed
void GameWorld::Print() const
{
    for (auto &obj : gameObjects)
    {
        if (obj == nullptr)
        {
            continue;
        }
        if (obj->GetComponent<TransformComponent>()->GetParent() == nullptr)
        {
            obj->Print();
        }
    }
}

// Immediately removes a game object and all of its children
// Detaches the object from parent, removes from gameObjects and markedToDestroyGameObjects arrays,
// then recursively deletes all descendants
void GameWorld::DestroyGameObjectImmediate(GameObject *gameObject)
{
    auto parent = gameObject->GetComponent<TransformComponent>()->GetParent();
    if (parent != nullptr)
    {
        parent->GetGameObject()->RemoveChild(gameObject);
    }

    for (auto transform : gameObject->GetComponentsInChildren<TransformComponent>())
    {
        GameObject *gameObjectToDelete = transform->GetGameObject();

        gameObjects.erase(
            std::remove_if(gameObjects.begin(), gameObjects.end(), [gameObjectToDelete](GameObject *obj) { return obj == gameObjectToDelete; }),
            gameObjects.end());
        markedToDestroyGameObjects.erase(std::remove_if(markedToDestroyGameObjects.begin(),
                                                        markedToDestroyGameObjects.end(),
                                                        [gameObjectToDelete](GameObject *obj) { return obj == gameObjectToDelete; }),
                                         markedToDestroyGameObjects.end());

        delete gameObjectToDelete;
    }
}
} // namespace XYZEngine