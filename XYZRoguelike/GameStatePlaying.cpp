#include "GameStatePlaying.h"
#include "Application.h"
#include "Game.h"
#include "Text.h"
#include "randomizer.h"
#include <assert.h>
#include <sstream>

namespace XYZRoguelike
{
void GameStatePlayingData::Init()
{
    // Init game resources (terminate if error)
    assert(font.loadFromFile(SETTINGS.FONTS_PATH + "Roboto-Regular.ttf"));
    assert(gameOverSoundBuffer.loadFromFile(SETTINGS.SOUNDS_PATH + "Death.wav"));

    // Init background
    background.setSize(sf::Vector2f(static_cast<float>(SETTINGS.SCREEN_WIDTH), static_cast<float>(SETTINGS.SCREEN_HEIGHT)));
    background.setPosition(0.f, 0.f);
    background.setFillColor(sf::Color(0, 0, 0));

    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::Yellow);

    inputHintText.setFont(font);
    inputHintText.setCharacterSize(24);
    inputHintText.setFillColor(sf::Color::White);
    inputHintText.setString("Use arrow keys to move, ESC to pause");
    inputHintText.setOrigin(GetTextOrigin(inputHintText, {1.f, 0.f}));

    // Init sounds
    gameOverSound.setBuffer(gameOverSoundBuffer);
    bonusSound.setBuffer(bonusSoundBuffer);
}

void GameStatePlayingData::HandleWindowEvent(const sf::Event &event)
{
    if (event.type == sf::Event::KeyPressed)
    {
        if (event.key.code == sf::Keyboard::Escape)
        {
            Application::Instance().GetGame().PauseGame();
        }
    }
}

void GameStatePlayingData::ConfigureSpawnContext(int width,
                                                 int height,
                                                 const std::vector<std::vector<bool>> &gridData,
                                                 XYZEngine::GameObject *playerObj)
{
    mazeWidth = width;
    mazeHeight = height;
    mazeGrid = gridData;
    playerTarget = playerObj;
}

void GameStatePlayingData::Update(float timeDelta)
{
    (void)timeDelta;

    // Спауним первую волну
    if (currentWave == 0 && !hasSpawned)
    {
        const bool hasValidSpawnContext = mazeWidth > 0 && mazeHeight > 0 && !mazeGrid.empty() && playerTarget != nullptr;

        if (!hasValidSpawnContext)
        {
            return;
        }

        spawner.SpawnWave(currentWave, mazeWidth, mazeHeight, mazeGrid, playerTarget);
        currentWave++;
        hasSpawned = true;
    }
}

void GameStatePlayingData::Draw(sf::RenderWindow &window)
{
    // Draw background
    window.draw(background);

    static auto drawFunc = [&window](auto block) { block->Draw(window); };
    // Draw game objects

    scoreText.setOrigin(GetTextOrigin(scoreText, {0.f, 0.f}));
    scoreText.setPosition(10.f, 10.f);
    window.draw(scoreText);

    sf::Vector2f viewSize = window.getView().getSize();
    inputHintText.setPosition(viewSize.x - 10.f, 10.f);
    window.draw(inputHintText);
}

void GameStatePlayingData::LoadNextLevel()
{
    if (currentLevel >= levelLoder.GetLevelCount() - 1)
    {
        Game &game = Application::Instance().GetGame();

        game.WinGame();
    }
    else
    {
    }
}

void GameStatePlayingData::GetBallInverse(const sf::Vector2f &ballPos, const sf::FloatRect &blockRect, bool &needInverseDirX, bool &needInverseDirY)
{

    if (ballPos.y > blockRect.top + blockRect.height)
    {
        needInverseDirY = true;
    }
    if (ballPos.x < blockRect.left)
    {
        needInverseDirX = true;
    }
    if (ballPos.x > blockRect.left + blockRect.width)
    {
        needInverseDirX = true;
    }
}

void GameStatePlayingData::Notify(std::shared_ptr<IObservable> observable)
{
}
} // namespace XYZRoguelike
