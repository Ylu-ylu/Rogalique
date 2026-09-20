#include "pch.h"
#include "TransformComponent.h"
#include <cmath>
#include <iostream>

namespace XYZEngine
{
// Constructor: initializes the Transform component attached to a GameObject
TransformComponent::TransformComponent(GameObject *gameObject) : Component(gameObject)
{
}

void TransformComponent::Update(float deltaTime)
{
}
void TransformComponent::Render()
{
}

// Moves the object by the specified offset vector
void TransformComponent::MoveBy(const Vector2Df &offset)
{
    MoveBy(offset.x, offset.y);
    isUpdated = false;
}
// Moves the object by the specified X and Y offsets
void TransformComponent::MoveBy(float offsetX, float offsetY)
{
    localPosition.x += offsetX;
    localPosition.y += offsetY;
    isUpdated = false;
}
// Sets the world position from a vector
void TransformComponent::SetWorldPosition(const Vector2Df &position)
{
    SetWorldPosition(position.x, position.y);
    isUpdated = false;
}
// Sets the world position using X and Y coordinates
void TransformComponent::SetWorldPosition(float positionX, float positionY)
{
    Vector2Df newPosition = {positionX, positionY};
    if (parent == nullptr)
    {
        localPosition = newPosition;
    }
    else
    {
        Matrix2D newWorldTransform = createTransform(newPosition, GetWorldRotation(), GetWorldScale());
        localTransform = parent->GetWorldTransform().GetInversed() * newWorldTransform;

        setLocalInfoFrom(localTransform);

        isUpdated = false;
        updateLocalTransform();
    }
}
// Sets the local position from a vector
void TransformComponent::SetLocalPosition(const Vector2Df &position)
{
    SetLocalPosition(position.x, position.y);
    isUpdated = false;
}
// Sets the local position using X and Y coordinates
void TransformComponent::SetLocalPosition(float positionX, float positionY)
{
    localPosition.x = positionX;
    localPosition.y = positionY;
    isUpdated = false;
}

// Rotates the object by the specified angle offset in degrees
void TransformComponent::RotateBy(float offset)
{
    localRotation = localRotation + offset;
    while (localRotation >= 360)
    {
        localRotation -= 360;
    }
    while (localRotation < 0)
    {
        localRotation += 360;
    }
    isUpdated = false;
}
// Sets the world rotation angle in degrees
void TransformComponent::SetWorldRotation(float angle)
{
    if (parent == nullptr)
    {
        localRotation = angle;
    }
    else
    {
        Matrix2D newWorldTransform = createTransform(GetWorldPosition(), angle, GetWorldScale());
        localTransform = parent->GetWorldTransform().GetInversed() * newWorldTransform;

        setLocalInfoFrom(localTransform);

        isUpdated = false;
        updateLocalTransform();
    }
}
// Sets the local rotation angle in degrees
void TransformComponent::SetLocalRotation(float angle)
{
    localRotation = angle;
    while (localRotation >= 360)
    {
        localRotation -= 360;
    }
    while (localRotation < 0)
    {
        localRotation += 360;
    }
    isUpdated = false;
}

// Scales the object by the specified scale vector (multiplies current scale)
void TransformComponent::ScaleBy(const Vector2Df &scale)
{
    ScaleBy(scale.x, scale.y);
    isUpdated = false;
}
// Scales the object by the specified X and Y scale factors
void TransformComponent::ScaleBy(float scaleX, float scaleY)
{
    localScale.x *= scaleX;
    localScale.y *= scaleY;
    isUpdated = false;
}
// Sets the world scale from a vector
void TransformComponent::SetWorldScale(const Vector2Df &scale)
{
    SetWorldScale(scale.x, scale.y);
    isUpdated = false;
}
// Sets the world scale using X and Y scale factors
void TransformComponent::SetWorldScale(float scaleX, float scaleY)
{
    Vector2Df newScale = {scaleX, scaleY};
    if (parent == nullptr)
    {
        localScale = newScale;
    }
    else
    {
        Matrix2D newWorldTransform = createTransform(GetWorldPosition(), GetWorldRotation(), newScale);
        localTransform = parent->GetWorldTransform().GetInversed() * newWorldTransform;

        setLocalInfoFrom(localTransform);

        isUpdated = false;
        updateLocalTransform();
    }
}
// Sets the local scale from a vector
void TransformComponent::SetLocalScale(const Vector2Df &scale)
{
    SetLocalScale(scale.x, scale.y);
    isUpdated = false;
}
// Sets the local scale using X and Y scale factors
void TransformComponent::SetLocalScale(float scaleX, float scaleY)
{
    localScale.x = scaleX;
    localScale.y = scaleY;
    isUpdated = false;
}

// Gets the world position of the object
const Vector2Df &TransformComponent::GetWorldPosition() const
{
    updateLocalTransform();
    if (parent == nullptr)
    {
        return GetLocalPosition();
    }

    setWorldInfoFrom(parent->GetWorldTransform() * localTransform);
    return position;
}
// Gets the local position of the object relative to its parent
const Vector2Df &TransformComponent::GetLocalPosition() const
{
    updateLocalTransform();
    return localPosition;
}

// Gets the world rotation angle in degrees
const float TransformComponent::GetWorldRotation() const
{
    updateLocalTransform();
    if (parent == nullptr)
    {
        return GetLocalRotation();
    }

    setWorldInfoFrom(parent->GetWorldTransform() * localTransform);
    return rotation;
}

// Gets the local rotation angle in degrees relative to its parent
const float TransformComponent::GetLocalRotation() const
{
    updateLocalTransform();
    return localRotation;
}

// Gets the world scale of the object
const Vector2Df &TransformComponent::GetWorldScale() const
{
    updateLocalTransform();
    if (parent == nullptr)
    {
        return GetLocalScale();
    }

    setWorldInfoFrom(parent->GetWorldTransform() * localTransform);
    return scale;
}
// Gets the local scale relative to its parent
const Vector2Df &TransformComponent::GetLocalScale() const
{
    updateLocalTransform();
    return localScale;
}

// Sets the parent transform for hierarchical transformation
void TransformComponent::SetParent(TransformComponent *newParent)
{
    if (parent == nullptr && newParent != nullptr)
    {
        newParent->gameObject->AddChild(gameObject);
        localTransform = newParent->GetWorldTransform().GetInversed() * localTransform;
    }
    else if (parent != nullptr && newParent == nullptr)
    {
        parent->gameObject->RemoveChild(gameObject);
        localTransform = parent->GetWorldTransform() * localTransform;
    }
    else if (parent != nullptr && newParent != nullptr)
    {
        parent->gameObject->RemoveChild(gameObject);
        newParent->gameObject->AddChild(gameObject);
        localTransform = newParent->GetWorldTransform().GetInversed() * (parent->GetWorldTransform() * localTransform);
    }

    setLocalInfoFrom(localTransform);

    parent = newParent;
    isUpdated = false;
}
// Gets the parent transform, or null if this is a root transform
TransformComponent *TransformComponent::GetParent() const
{
    return parent;
}

// Gets the combined world transformation matrix
const Matrix2D TransformComponent::GetWorldTransform() const
{
    updateLocalTransform();

    if (parent == nullptr)
    {
        return localTransform;
    }

    return parent->GetWorldTransform() * localTransform;
}

// Prints detailed debug information about this transform's position, rotation, and scale
void TransformComponent::Print() const
{
    std::cout << "Transform name : " << gameObject->GetName() << std::endl;

    std::cout << "Position" << std::endl;
    std::cout << "  World: (" << GetWorldPosition().x << ", " << GetWorldPosition().y << ")" << std::endl;
    std::cout << "  Local: (" << GetLocalPosition().x << ", " << GetLocalPosition().y << ")" << std::endl;

    std::cout << "Rotation" << std::endl;
    std::cout << "  World: " << GetWorldRotation() << std::endl;
    std::cout << "  Local: " << GetLocalRotation() << std::endl;

    std::cout << "Scale" << std::endl;
    std::cout << "  World: (" << GetWorldScale().x << ", " << GetWorldScale().y << ")" << std::endl;
    std::cout << "  Local: (" << GetLocalScale().x << ", " << GetLocalScale().y << ")" << std::endl;
}

void TransformComponent::setWorldInfoFrom(const Matrix2D &transform) const
{
    position.x = transform.GetMatrix()[0][2];
    position.y = transform.GetMatrix()[1][2];

    scale.x = std::sqrt(transform.GetMatrix()[0][0] * transform.GetMatrix()[0][0] + transform.GetMatrix()[1][0] * transform.GetMatrix()[1][0]);
    scale.y = std::sqrt(transform.GetMatrix()[0][1] * transform.GetMatrix()[0][1] + transform.GetMatrix()[1][1] * transform.GetMatrix()[1][1]);

    rotation = std::atan2(transform.GetMatrix()[0][1], transform.GetMatrix()[0][0]) * 180 / 3.14159265;
}
void TransformComponent::setLocalInfoFrom(const Matrix2D &transform) const
{
    localPosition.x = transform.GetMatrix()[0][2];
    localPosition.y = transform.GetMatrix()[1][2];

    localScale.x = std::sqrt(transform.GetMatrix()[0][0] * transform.GetMatrix()[0][0] + transform.GetMatrix()[1][0] * transform.GetMatrix()[1][0]);
    localScale.y = std::sqrt(transform.GetMatrix()[0][1] * transform.GetMatrix()[0][1] + transform.GetMatrix()[1][1] * transform.GetMatrix()[1][1]);

    localRotation = std::atan2(transform.GetMatrix()[0][1], transform.GetMatrix()[0][0]) * 180 / 3.14159265;
}
void TransformComponent::updateLocalTransform() const
{
    if (!isUpdated)
    {
        localTransform = createTransform(localPosition, localRotation, localScale);
        isUpdated = true;
    }
}
void TransformComponent::updateLocalTransform(const Vector2Df &position, float rotation, const Vector2Df &scale) const
{
    if (!isUpdated)
    {
        localTransform = createTransform(position, rotation, scale);
        isUpdated = true;
    }
}
Matrix2D TransformComponent::createTransform(const Vector2Df &position, float rotation, const Vector2Df &scale) const
{
    return Matrix2D(position, rotation, scale);
}
} // namespace XYZEngine