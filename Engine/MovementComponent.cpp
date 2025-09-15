#include "pch.h"
#include "MovementComponent.h"

namespace XYZEngine
{
	MovementComponent::MovementComponent(GameObject* gameObject)
		: Component(gameObject)
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
		float xAxis = input->GetHorizontalAxis();
		float yAxis = input->GetVerticalAxis();

		transform->MoveBy(speed * deltaTime * Vector2Df{ xAxis, yAxis });

		acceleration = transform->GetWorldPosition() - previousPosition;
		previousPosition = transform->GetWorldPosition();
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

}


