#include "pch.h"
#include "GameObject.h"
// #include <SFML/Window/Keyboard.hpp>

namespace XYZEngine
{
// Default constructor: creates a GameObject named "GameObject" and adds a TransformComponent
GameObject::GameObject()
{
    name = "GameObject";
    AddComponent<TransformComponent>();
}

// Parameterized constructor: creates a GameObject with the specified name and adds a TransformComponent
GameObject::GameObject(std::string newName)
{
    name = newName;
    AddComponent<TransformComponent>();
}

// Destructor: deletes all components and clears component and children lists
GameObject::~GameObject()
{
    for (auto component : components)
    {
        delete component;
    }
    components.clear();
    children.clear();
}

// Gets the name of the GameObject
std::string GameObject::GetName() const
{
    return name;
}

// Prints object information and its entire hierarchy (object and its descendants) to console
// The depth parameter is used for visual indentation in the hierarchy
void GameObject::Print(int depth) const
{
    std::cout << std::string(depth * 2, ' ') << GetName() << std::endl;
    for (auto &component : components)
    {
        std::cout << std::string(depth * 2, ' ') << "::" << component << std::endl;
    }

    for (GameObject *child : children)
    {
        child->Print(depth + 1);
    }
}

// Updates all components of the object each frame
void GameObject::Update(float deltaTime)
{
    for (auto &component : components)
    {
        component->Update(deltaTime);
    }
}

// Renders all components of the object to screen
void GameObject::Render()
{
    for (auto &component : components)
    {
        component->Render();
    }
}

// Adds a child to this GameObject
void GameObject::AddChild(GameObject *child)
{
    children.push_back(child);
}

// Removes a child from this GameObject
void GameObject::RemoveChild(GameObject *child)
{
    children.erase(std::remove_if(children.begin(), children.end(), [child](GameObject *obj) { return obj == child; }), children.end());
}
} // namespace XYZEngine