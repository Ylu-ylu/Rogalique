#include "Game.h"
#include <assert.h>
#include <algorithm>
#include "GameStatePlaying.h"
#include "GameStateGameOver.h"
#include "GameStatePauseMenu.h"
#include "GameStateMainMenu.h"
#include "GameStateRecords.h"

namespace XYZRoguelike
{

// Constructs game instance with initial records and switches to main menu state
Game::Game()
{
    // Generate fake records table
    recordsTable = {
        {"John", SETTINGS.MAX_APPLES / 2},
        {"Jane", SETTINGS.MAX_APPLES / 3},
        {"Alice", SETTINGS.MAX_APPLES / 4},
        {"Bob", SETTINGS.MAX_APPLES / 5},
        {"Clementine", SETTINGS.MAX_APPLES / 5},
    };

    stateChangeType = GameStateChangeType::None;
    pendingGameStateType = GameStateType::None;
    pendingGameStateIsExclusivelyVisible = false;
    SwitchStateTo(GameStateType::MainMenu);
}

// Destructs game instance and cleans up all active game states
Game::~Game()
{
    Shutdown();
}

// Processes all pending window events and delegates them to the current game state
void Game::HandleWindowEvents(sf::RenderWindow &window)
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        // Close window if close button or Escape key pressed
        if (event.type == sf::Event::Closed)
        {
            window.close();
        }

        if (stateStack.size() > 0)
        {
            stateStack.back().HandleWindowEvent(event);
        }
    }
}

// Updates game state and processes pending state changes; returns false when game should exit
bool Game::Update(float timeDelta)
{
    if (stateChangeType == GameStateChangeType::Switch)
    {
        // Shutdown all game states
        while (stateStack.size() > 0)
        {
            stateStack.pop_back();
        }
    }
    else if (stateChangeType == GameStateChangeType::Pop)
    {
        // Shutdown only current game state
        if (stateStack.size() > 0)
        {
            stateStack.pop_back();
        }
    }

    // Initialize new game state if needed
    if (pendingGameStateType != GameStateType::None)
    {
        stateStack.push_back(GameState(pendingGameStateType, pendingGameStateIsExclusivelyVisible));
    }

    stateChangeType = GameStateChangeType::None;
    pendingGameStateType = GameStateType::None;
    pendingGameStateIsExclusivelyVisible = false;

    if (stateStack.size() > 0)
    {
        stateStack.back().Update(timeDelta);
        return true;
    }

    return false;
}

// Renders visible game states to the window, respecting exclusive visibility flags
void Game::Draw(sf::RenderWindow &window)
{
    if (stateStack.size() > 0)
    {
        std::vector<GameState *> visibleGameStates;
        for (auto it = stateStack.rbegin(); it != stateStack.rend(); ++it)
        {
            visibleGameStates.push_back(&(*it));
            if (it->IsExclusivelyVisible())
            {
                break;
            }
        }

        for (auto it = visibleGameStates.rbegin(); it != visibleGameStates.rend(); ++it)
        {
            (*it)->Draw(window);
        }
    }
}

// Clears all game states from the stack and resets state change tracking
void Game::Shutdown()
{
    // Shutdown all game states
    while (stateStack.size() > 0)
    {
        stateStack.pop_back();
    }

    stateChangeType = GameStateChangeType::None;
    pendingGameStateType = GameStateType::None;
    pendingGameStateIsExclusivelyVisible = false;
}

// Pushes a new game state onto the stack, optionally making it block underlying states from rendering
void Game::PushState(GameStateType stateType, bool isExclusivelyVisible)
{
    pendingGameStateType = stateType;
    pendingGameStateIsExclusivelyVisible = isExclusivelyVisible;
    stateChangeType = GameStateChangeType::Push;
}

// Removes the current game state from the stack and returns to the previous one
void Game::PopState()
{
    pendingGameStateType = GameStateType::None;
    pendingGameStateIsExclusivelyVisible = false;
    stateChangeType = GameStateChangeType::Pop;
}

// Exits current gameplay and returns to main menu
void Game::ExitGame()
{
    SwitchStateTo(GameStateType::MainMenu);
}

// Switches to a new game state, clearing all existing states from the stack
void Game::SwitchStateTo(GameStateType newState)
{
    pendingGameStateType = newState;
    pendingGameStateIsExclusivelyVisible = false;
    stateChangeType = GameStateChangeType::Switch;
}

// Displays the records/high scores screen as an exclusive overlay
void Game::ShowRecords()
{
    PushState(GameStateType::Records, true);
}

// Checks if a specific game option is currently enabled
bool Game::IsEnableOptions(GameOptions option) const
{
    const bool isEnable = ((std::uint8_t)options & (std::uint8_t)option) != (std::uint8_t)GameOptions::Empty;
    return isEnable;
}

// Enables or disables a specific game option using bitwise operations
void Game::SetOption(GameOptions option, bool value)
{
    if (value)
    {
        options = (GameOptions)((std::uint8_t)options | (std::uint8_t)option);
    }
    else
    {
        options = (GameOptions)((std::uint8_t)options & ~(std::uint8_t)option);
    }
}

// Retrieves the best score for a player by their ID; returns 0 if player not found
int Game::GetRecordByPlayerId(const std::string &playerId) const
{
    auto it = recordsTable.find(playerId);
    return it == recordsTable.end() ? 0 : it->second;
}

// Quits the game entirely by switching to None state
void Game::QuitGame()
{
    SwitchStateTo(GameStateType::None);
}

// Updates a player's record with the highest score between current and new value
void Game::UpdateRecord(const std::string &playerId, int score)
{
    recordsTable[playerId] = std::max(recordsTable[playerId], score);
}

// Switches to the Playing game state to begin a new game session
void Game::StartGame()
{
    SwitchStateTo(GameStateType::Playing);
}

// Pauses gameplay by pushing an exit dialog state onto the stack
void Game::PauseGame()
{
    PushState(GameStateType::ExitDialog, false);
}

// Displays game win screen as an overlay
void Game::WinGame()
{
    PushState(GameStateType::GameWin, false);
}

// Displays game over screen as an overlay
void Game::LooseGame()
{
    PushState(GameStateType::GameOver, false);
}

// Main game loop update: processes events, updates game state, and renders frame
void Game::UpdateGame(float timeDelta, sf::RenderWindow &window)
{
    HandleWindowEvents(window);
    if (Update(timeDelta))
    {
        // Draw everything here
        // Clear the window first
        window.clear();

        Draw(window);

        // End the current frame, display window contents on screen
        window.display();
    }
    else
    {
        window.close();
    }
}

// Delegates level progression to the current Playing game state
void Game::LoadNextLevel()
{
    assert(stateStack.back().GetType() == GameStateType::Playing);
    auto playingData = (stateStack.back().GetData<GameStatePlayingData>());
    playingData->LoadNextLevel();
}
} // namespace XYZRoguelike
