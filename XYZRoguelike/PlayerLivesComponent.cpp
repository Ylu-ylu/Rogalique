#include "PlayerLivesComponent.h"
#include "../Engine/GameObject.h"
#include "../Engine/Logger.h"

namespace XYZRoguelike
{
PlayerLivesComponent::PlayerLivesComponent(XYZEngine::GameObject *gameObject) : XYZEngine::Component(gameObject)
{
}

void PlayerLivesComponent::Update(float deltaTime)
{
}

void PlayerLivesComponent::Render()
{
}

int PlayerLivesComponent::GetLives() const
{
    return lives;
}

bool PlayerLivesComponent::HasLives() const
{
    return lives > 0;
}

void PlayerLivesComponent::LoseLife()
{
    if (lives <= 0)
    {
        return;
    }

    lives--;

    LOG_INFO("Player lost life. Lives left: " + std::to_string(lives));

    if (lives <= 0)
    {
        LOG_INFO("GAME OVER");
    }
}

void PlayerLivesComponent::ResetLives()
{
    lives = 3;
}
} // namespace XYZRoguelike
