#pragma once

#include <iostream>
#include <array>
#include "../Engine/Scene.h"
#include "Player.h"
#include "Music.h"
#include "AI.h"
#include "Floor.h"
#include "Wall.h"
#include "CreeperSpawner.h"

using namespace XYZEngine;

namespace XYZRoguelike
{
class DeveloperLevel : public Scene
{
  public:
    void Start() override;
    void Restart() override;
    void Stop() override;
    std::vector<std::unique_ptr<Wall>> walls;
    std::vector<std::unique_ptr<Floor>> floors;

    std::shared_ptr<Player> GetPlayer();

  private:
    void ChooseExitTile(int width, int height, int &outExitX, int &outExitY) const;
    void CreateExitTrigger(int exitX, int exitY);
    void LoadNextLevel();

  private:
    std::shared_ptr<Player> player;
    std::shared_ptr<AI> ai;
    std::unique_ptr<Music> music;
    std::unique_ptr<CreeperSpawner> creeperSpawner;

    int currentLevel = 1;
    bool isLevelTransitionInProgress = false;
};
} // namespace XYZRoguelike