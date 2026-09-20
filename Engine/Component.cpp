#include "pch.h"
#include "Component.h"
#include <iostream>

namespace XYZEngine
{
// Constructor: initializes the component with a reference to the parent GameObject
Component::Component(GameObject *gameObject) : gameObject(gameObject)
{
}

// Destructor: prints debug information about component removal
Component::~Component()
{
    std::cout << "Deleted component: " << this << std::endl;
}

// Gets the pointer to the parent GameObject this component is attached to
GameObject *Component::GetGameObject()
{
    return gameObject;
}
} // namespace XYZEngine