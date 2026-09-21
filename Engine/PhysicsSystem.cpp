#include "pch.h"
#include "PhysicsSystem.h"

namespace XYZEngine
{
PhysicsSystem *PhysicsSystem::Instance()
{
    static PhysicsSystem physicsSystem;
    return &physicsSystem;
}

float PhysicsSystem::GetFixedDeltaTime() const
{
    return fixedDeltaTime;
}

void PhysicsSystem::Update()
{
    for (int i = 0; i < colliders.size(); i++)
    {
        if (!colliders[i]->enabled)
        {
            continue;
        }

        auto body = colliders[i]->GetGameObject()->GetComponent<RigidbodyComponent>();
        if (body->GetKinematic())
        {
            continue;
        }

        for (int j = 0; j < colliders.size(); j++)
        {
            if (j == i || !colliders[j]->enabled)
            {
                continue;
            }

            sf::FloatRect intersection;
            if (colliders[i]->bounds.intersects(colliders[j]->bounds, intersection))
            {
                if (colliders[i]->isTrigger != colliders[j]->isTrigger)
                {
                    if (triggersEnteredPair.find(colliders[i]) == triggersEnteredPair.end() &&
                        triggersEnteredPair.find(colliders[j]) == triggersEnteredPair.end())
                    {
                        auto trigger = new Trigger(colliders[i], colliders[j]);
                         colliders[i]->OnTriggerEnter(*trigger);
                        colliders[j]->OnTriggerEnter(*trigger);

                        triggersEnteredPair.emplace(colliders[i], colliders[j]);
                    }
                }
                else if (!colliders[i]->isTrigger)
                {
                    float intersectionWidth = intersection.width;
                    float intersectionHeight = intersection.height;

                    auto aTransform = colliders[i]->GetGameObject()->GetComponent<TransformComponent>();

                    // Push apart along the axis of least penetration, away from
                    // the other collider's center. Center-based comparison keeps
                    // the direction correct so objects never get pushed into walls.
                    float aCenterX = colliders[i]->bounds.left + 0.5f * colliders[i]->bounds.width;
                    float aCenterY = colliders[i]->bounds.top + 0.5f * colliders[i]->bounds.height;
                    float bCenterX = colliders[j]->bounds.left + 0.5f * colliders[j]->bounds.width;
                    float bCenterY = colliders[j]->bounds.top + 0.5f * colliders[j]->bounds.height;

                    if (intersectionWidth > intersectionHeight)
                    {
                        float direction = (aCenterY >= bCenterY) ? 1.f : -1.f;
                        aTransform->MoveBy({0.f, direction * intersectionHeight});
                    }
                    else
                    {
                        float direction = (aCenterX >= bCenterX) ? 1.f : -1.f;
                        aTransform->MoveBy({direction * intersectionWidth, 0.f});
                    }

                    auto collision = new Collision(colliders[i], colliders[j], intersection);
                    colliders[i]->OnCollision(*collision);
                    colliders[j]->OnCollision(*collision);
                }
            }
        }
    }

    for (auto triggeredPair = triggersEnteredPair.cbegin(), nextTriggeredPair = triggeredPair; triggeredPair != triggersEnteredPair.cend();
         triggeredPair = nextTriggeredPair)
    {
        ++nextTriggeredPair;
        if (!triggeredPair->first->bounds.intersects(triggeredPair->second->bounds))
        {
            auto trigger = new Trigger(triggeredPair->first, triggeredPair->second);
            triggeredPair->first->OnTriggerExit(*trigger);
            triggeredPair->second->OnTriggerExit(*trigger);

            triggersEnteredPair.erase(triggeredPair);
        }
    }
}

void PhysicsSystem::Subscribe(ColliderComponent *collider)
{
    std::cout << "Subscribe " << collider << std::endl;
    colliders.push_back(collider);
}
void PhysicsSystem::Unsubscribe(ColliderComponent *collider)
{
    std::cout << "Unsubscribe " << collider << std::endl;

    for (auto it = triggersEnteredPair.cbegin(); it != triggersEnteredPair.cend();)
    {
        if (it->first == collider || it->second == collider)
        {
            it = triggersEnteredPair.erase(it);
        }
        else
        {
            ++it;
        }
    }

    colliders.erase(std::remove_if(colliders.begin(), colliders.end(), [collider](ColliderComponent *obj) { return obj == collider; }),
                    colliders.end());
}
} // namespace XYZEngine