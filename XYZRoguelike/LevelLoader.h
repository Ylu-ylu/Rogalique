#pragma once

#include <vector>
#include <string>
#include <memory>
#include <map>

namespace XYZRoguelike
{

struct Level
{
};

class LevelLoader final
{
  public:
    Level &GetLevel(int i);
    ~LevelLoader() = default;
    int GetLevelCount();

  private:
    std::vector<Level> levels;
};

} // namespace XYZRoguelike
