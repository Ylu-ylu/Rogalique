#pragma once
#define NOMINMAX
#include <algorithm>
#include "SFML/Graphics.hpp"
#include "Logger.h"

namespace XYZEngine
{
class Engine
{
  public:
    Engine(const Engine &app) = delete;
    Engine &operator=(const Engine &) = delete;

    static Engine *Instance();

    void Run();

    void SetPaused(bool value);
    bool IsPaused() const;

  private:
    Engine();
    ~Engine() = default;

    void setupLogger();

    bool isPaused = false;
    bool escPressedLastFrame = false;
};
} // namespace XYZEngine