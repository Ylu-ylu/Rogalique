#pragma once
#include <stack>
#include <vector>

#include "DeveloperLevel.h"

namespace XYZRoguelike
{

class DeveloperLevel;

class MazeGenerator
{
  public:
    MazeGenerator(int width, int height, DeveloperLevel *level);
    void Generate();

    // ✅ NEW: Getter for grid (for spawner)
    const std::vector<std::vector<bool>> &GetGrid() const;

  private:
    int width;
    int height;
    DeveloperLevel *level;
    std::vector<std::vector<bool>> grid;

    std::vector<std::pair<int, int>> GetAvailableDirections(int x, int y);
    void RemoveWall(int x1, int y1, int x2, int y2);
};

} // namespace XYZRoguelike
