#include "MazeGenerator.h"
#include <cstdlib>
#include <ctime>

namespace XYZRoguelike
{
// Initialize the MazeGenerator with specified dimensions and target level
MazeGenerator::MazeGenerator(int width, int height, DeveloperLevel *level)
    : width(width), height(height), level(level) // Store pointer to the DeveloperLevel where walls/floors will be create
{
    //  Initialize the 2D grid that represents the maze structure:
    // - First dimension (height): number of rows in the maze
    // - Second dimension (width): number of columns in each row
    // - Initial value (false): represents walls (true would represent open paths)
    grid.resize(height, std::vector<bool>(width, false));
}

void MazeGenerator::Generate()
{
    // Create a seed for the random number generator
    //  This ensures different maze layouts each time the function is called
    std::srand(std::time(nullptr));

    // Start the maze at a random position
    //  startX: random column (0 to width-1)
    //  startY: random row (0 to height-1)
    int startX = std::rand() % width;
    int startY = std::rand() % height;

    // Initialize the stack for depth-first search with backtracking
    // The stack stores coordinates of cells we can return to when we hit a dead end
    std::stack<std::pair<int, int>> stack;
    // Add starting position to stack and mark it as part of the maze
    stack.push({startX, startY});
    grid[startY][startX] = true; // Mark starting cell as carved path (true = open, false = wall)

    // Main maze generation loop - continues until all reachable cells are processed
    while (!stack.empty())
    {
        // Get current position from top of stack(peek, don't remove yet)
        auto [x, y] = stack.top();
        stack.pop(); // Remove current position from stack

        // Step 7: Find all valid directions we can carve from current position
        // GetAvailableDirections() should return neighboring cells that are:
        // - Within maze boundaries
        // - Not already carved (still walls)
        // - At least 2 cells away to maintain wall thickness
        std::vector<std::pair<int, int>> directions = GetAvailableDirections(x, y);

        if (!directions.empty())
        {
            // Push current position back onto stack for potential backtracking
            //  This allows us to return here later if the chosen path leads to a dead end
            stack.push({x, y});

            // Randomly select one direction from available options
            std::pair<int, int> dir = directions[std::rand() % directions.size()];
            // Calculate coordinates of the next cell to carve
            int nx = x + dir.first;
            int ny = y + dir.second;

            // Remove wall between current cell and next cell
            // This physically connects the two cells in the maze
            RemoveWall(x, y, nx, ny);
            // Add the newly carved cell to the stack for further exploration
            stack.push({nx, ny});
            // Mark the new cell as carved in the grid
            grid[ny][nx] = true;
        }
    }

    // Coordinates of the exit (e.g., bottom right corner)
    int exitX = width - 1;
    int exitY = height - 1;

    // Count the number of walls around the exit
    int wallCount = 0;
    std::vector<std::pair<int, int>> neighbors = {{exitX - 1, exitY}, {exitX + 1, exitY}, {exitX, exitY - 1}, {exitX, exitY + 1}};

    for (const auto &[nx, ny] : neighbors)
    {
        if (nx >= 0 && nx < width && ny >= 0 && ny < height)
        {
            if (!grid[nx][ny])
                wallCount++;
        }
    }

    // If there are more than two walls, make one of the neighboring cells passable
    if (wallCount > 2)
    {
        for (const auto &[nx, ny] : neighbors)
        {
            if (nx >= 0 && nx < width && ny >= 0 && ny < height)
            {
                grid[nx][ny] = true; // Open the passage
                wallCount--;
                if (wallCount <= 2)
                    break;
            }
        }
    }
}

const std::vector<std::vector<bool>> &MazeGenerator::GetGrid() const
{
    return grid;
}

// GetAvailableDirections: Returns a list of valid, unvisited neighboring cells.
std::vector<std::pair<int, int>> MazeGenerator::GetAvailableDirections(int x, int y)
{
    // Define possible directions: up, down, left, right (2 cells away to leave
    // space for walls).
    std::vector<std::pair<int, int>> directions = {
        {0, -2}, // Up
        {0, 2},  // Down
        {-2, 0}, // Left
        {2, 0}   // Right
    };

    std::vector<std::pair<int, int>> available;

    // Check each direction to see if it leads to a valid, unvisited cell.
    for (const auto &dir : directions)
    {
        int nx = x + dir.first;
        int ny = y + dir.second;

        // Ensure the neighbor is within bounds and unvisited.
        if (nx >= 0 && nx < width && ny >= 0 && ny < height && !grid[ny][nx])
        {
            available.push_back(dir);
        }
    }

    return available;
}

// RemoveWall: Removes the wall between two cells and adds floors and walls to
// the level.
void MazeGenerator::RemoveWall(int x1, int y1, int x2, int y2)
{
    // Calculate the position of the wall between the two cells.
    int wallX = (x1 + x2) / 2;
    int wallY = (y1 + y2) / 2;

    // Add floors to the current cell and the neighboring cell.
    level->floors.push_back(std::make_unique<Floor>(XYZEngine::Vector2Df{x1 * 128.f, y1 * 128.f}, 0));
    level->floors.push_back(std::make_unique<Floor>(XYZEngine::Vector2Df{x2 * 128.f, y2 * 128.f}, 0));

    // Add a wall at the midpoint if the cells are not directly adjacent.
    if (wallX != x1 || wallY != y1)
    {
        level->walls.push_back(std::make_unique<Wall>(XYZEngine::Vector2Df{wallX * 128.f, wallY * 128.f}, 14));
    }
}

} // namespace XYZRoguelike