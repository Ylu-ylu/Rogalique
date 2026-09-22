#include "pch.h"
#include "MovementComponent.h"

namespace XYZEngine
{
MovementComponent::MovementComponent(GameObject *gameObject) : Component(gameObject)
{
    input = gameObject->GetComponent<InputComponent>();
    transform = gameObject->GetComponent<TransformComponent>();

    if (input == nullptr)
    {
        std::cout << "Need input component for movement" << std::endl;
        gameObject->RemoveComponent(this);
    }
}

void MovementComponent::Update(float deltaTime)
{
    if (stats == nullptr)
    {
        stats = gameObject->GetComponent<StatsComponent>();
    }

    if (stats != nullptr && stats->GetCurrentHealth() <= 0.f)
    {
        return;
    }

    float xAxis = input->GetHorizontalAxis();
    float yAxis = input->GetVerticalAxis();

    transform->MoveBy(speed * deltaTime * Vector2Df{xAxis, yAxis});

    acceleration = transform->GetWorldPosition() - previousPosition;
    previousPosition = transform->GetWorldPosition();

    if (animation == nullptr)
    {
        animation = gameObject->GetComponent<SpriteMovementAnimationComponent>();
    }

    if (animation != nullptr && !animation->IsPlaying("attack") && !animation->IsCurrentAnimation("death"))
    {
        if (xAxis != 0.f || yAxis != 0.f)
        {
            animation->Play("walk");
        }
        else
        {
            animation->Play("idle");
        }
    }
}

void MovementComponent::Render()
{
}

void MovementComponent::SetSpeed(float newSpeed)
{
    speed = newSpeed;
}

float MovementComponent::GetSpeed() const
{
    return speed;
}

float MovementComponent::GetAccelerationSquared() const
{
    return acceleration.x * acceleration.x + acceleration.y * acceleration.y;
}

} // namespace XYZEngine
