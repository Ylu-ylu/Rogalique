#pragma once

#include "../Engine/Component.h"
#include "../Engine/StatsComponent.h"
#include "PlayerLivesComponent.h"

#include <SFML/Graphics.hpp>

namespace XYZRoguelike
{
class DeveloperLevel;
class InventoryComponent;

// Top-left screen-space HUD: level number, lives (hearts), player stats and
// the exit hint ("Kill Boss for Exit!" / "Go to Exit!").
// Drawn in the default window view (view-swap trick) so it stays fixed on
// screen while the game camera follows the player.
class GameHUDComponent: public XYZEngine::Component
{
  public:
    GameHUDComponent(XYZEngine::GameObject *gameObject, int levelNumber, DeveloperLevel *level);

    void Update(float deltaTime) override;
    void Render() override;

    void SetTarget(XYZEngine::GameObject *player);

  private:
    XYZEngine::StatsComponent *stats = nullptr;
    PlayerLivesComponent *lives = nullptr;
    InventoryComponent *inventory = nullptr;
    DeveloperLevel *level = nullptr;

    XYZEngine::GameObject *playerObject = nullptr;

    int levelNumber = 1;

    sf::Font font;
    sf::Text text;
    sf::Text statusText;

    const sf::Texture *heartTexture = nullptr;
    sf::Sprite heartSprite;
};
} // namespace XYZRoguelike
