#include "pch.h"
#include "RigidbodyComponent.h"

namespace XYZEngine
{
// Constructor: initializes the Rigidbody component and retrieves a reference to the Transform component
RigidbodyComponent::RigidbodyComponent(GameObject *gameObject) : Component(gameObject)
{
    transform = gameObject->GetComponent<TransformComponent>();
}

// Updates physics each frame:
// 1. Apply linear and angular velocities to the object's transform
// 2. Apply damping to gradually reduce velocities
// 3. Zero out very small velocities to prevent infinite drift
void RigidbodyComponent::Update(float deltaTime)
{
    transform->MoveBy(linearVelocity);
    transform->RotateBy(angleVelocity);

    linearVelocity = (1.f - linearDamping * deltaTime) * linearVelocity;
    angleVelocity = (1.f - angleDamping * deltaTime) * angleVelocity;

    if (linearVelocity.x < 0.001f)
    {
        linearVelocity = {0.f, linearVelocity.y};
    }
    if (linearVelocity.y < 0.001f)
    {
        linearVelocity = {linearVelocity.x, 0.f};
    }

    if (angleVelocity < 0.001f)
    {
        angleVelocity = 0.f;
    }
}
// Render is empty since Rigidbody is not rendered directly to screen
// Physical data is rendered by other components (e.g., SpriteRendererComponent)
void RigidbodyComponent::Render()
{
}

// Sets the linear velocity of the object (completely replaces the current velocity)
// Used for explicitly setting movement speed
void RigidbodyComponent::SetLinearVelocity(Vector2Df newLinearVelocity)
{
    linearVelocity = newLinearVelocity;
}
// Adds additional linear velocity to the current velocity
// Used for impulses and forces that affect an already moving object
void RigidbodyComponent::AddLinearVelocity(Vector2Df linearOffset)
{
    linearVelocity = linearVelocity + linearOffset;
}
// Gets the current linear velocity of the object
// Returns a velocity vector (X and Y components)
Vector2Df RigidbodyComponent::GetLinearVelocity() const
{
    return linearVelocity;
}

// Sets the angular velocity of the object (completely replaces the current angular velocity)
// Used for explicitly setting rotation speed in radians/frame
void RigidbodyComponent::SetAngleVelocity(float newAngleVelocity)
{
    angleVelocity = newAngleVelocity;
}
// Adds additional angular velocity to the current angular velocity
// Used for rotation impulses
void RigidbodyComponent::AddAngleVelocity(float angleOffset)
{
    angleVelocity += angleOffset;
}
// Gets the current angular velocity of the object
// Returns angular velocity in radians/frame
float RigidbodyComponent::GetAngleVelocity() const
{
    return angleVelocity;
}

// Sets the linear damping coefficient
// Higher values = object stops faster
// Range: 0.0 (no damping) to 1.0 (maximum damping)
void RigidbodyComponent::SetLinearDamping(float newLinearDamping)
{
    linearDamping = newLinearDamping;
}
// Gets the current linear damping coefficient
// Returns a value from 0.0 to 1.0
float RigidbodyComponent::GetLinearDamping() const
{
    return linearDamping;
}

// Sets the angular damping coefficient for rotation
// Higher values = rotation stops faster
// Range: 0.0 (no damping) to 1.0 (maximum damping)
void RigidbodyComponent::SetAngleDamping(float newAngleDamping)
{
    angleDamping = newAngleDamping;
}
// Gets the current angular damping coefficient
// Returns a value from 0.0 to 1.0
float RigidbodyComponent::GetAngleDamping() const
{
    return angleDamping;
}

// Sets whether the object is kinematic (true) or dynamic (false)
// Kinematic object: manually controlled through velocity, not subject to physical collisions
// Dynamic object: subject to physical interactions and collisions
void RigidbodyComponent::SetKinematic(bool newIsKinematic)
{
    isKinematic = newIsKinematic;
}
// Gets the current Rigidbody mode (kinematic or dynamic)
// Returns true if kinematic, false if dynamic
bool RigidbodyComponent::GetKinematic() const
{
    return isKinematic;
}
} // namespace XYZEngine