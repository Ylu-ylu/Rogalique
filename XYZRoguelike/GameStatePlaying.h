#pragma once
#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"
#include "GameStateData.h"
#include "LevelLoader.h"
#include "IObserver.h"
#include <unordered_map>
#include <vector>
#include "CreeperSpawner.h"

namespace XYZEngine
{
class GameObject;
}

namespace XYZRoguelike
{
class Game;

class GameStatePlayingData : public GameStateData, public IObserver, public std::enable_shared_from_this<GameStatePlayingData>
{
  public:
    void Init() override;
    void HandleWindowEvent(const sf::Event &event) override;
    void Update(float timeDelta) override;
    void Draw(sf::RenderWindow &window) override;
    void LoadNextLevel();
    void Notify(std::shared_ptr<IObservable> observable) override;

    void ConfigureSpawnContext(int width, int height, const std::vector<std::vector<bool>> &gridData, XYZEngine::GameObject *playerObj);

  private:
    void GetBallInverse(const sf::Vector2f &ballPos, const sf::FloatRect &blockRect, bool &needInverseDirX, bool &needInverseDirY);

    // Resources
    sf::Texture appleTexture;
    sf::Texture rockTexture;
    sf::Font font;
    sf::SoundBuffer eatAppleSoundBuffer;
    sf::SoundBuffer gameOverSoundBuffer;
    sf::SoundBuffer bonusSoundBuffer;

    // Game data

    // UI data
    sf::Text scoreText;
    sf::Text inputHintText;
    sf::RectangleShape background;

    // Sounds
    sf::Sound gameOverSound;
    sf::Sound bonusSound;

    // Levels
    LevelLoader levelLoder;
    int currentLevel = 0;

    // Bonus
    CreeperSpawner spawner;
    int currentWave = 0;
    bool hasSpawned = false;

    // Spawn context (fixes undeclared identifiers in Update)
    int mazeWidth = 0;
    int mazeHeight = 0;
    std::vector<std::vector<bool>> mazeGrid;
    XYZEngine::GameObject *playerTarget = nullptr;
};
} // namespace XYZRoguelike
