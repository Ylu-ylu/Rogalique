#include "MazeGenerator.h"
#include <cstdlib>
#include <ctime>
#include <queue>

namespace XYZRoguelike
{
// Initialize the MazeGenerator with specified dimensions, target level and exit tile
MazeGenerator::MazeGenerator(int width, int height, DeveloperLevel *level, int exitX, int exitY)
    : width(width), height(height), level(level), exitX(exitX), exitY(exitY)
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

    ConnectExitToMaze();
}

// Carves a guaranteed walkable corridor from the exit gap to the nearest maze cell.
void MazeGenerator::ConnectExitToMaze()
{
    int inX = exitX;
    int inY = exitY;

    if (exitX == 0)
    {
        inX = 1;
    }
    else if (exitX == width)
    {
        inX = width - 2;
    }
    else if (exitY == 0)
    {
        inY = 1;
    }
    else
    {
        inY = height - 2;
    }

    if (inX < 0 || inX >= width || inY < 0 || inY >= height)
    {
        return;
    }

    if (grid[inY][inX])
    {
        return;
    }

    std::vector<std::vector<std::pair<int, int>>> parent(height, std::vector<std::pair<int, int>>(width, {-1, -1}));
    std::queue<std::pair<int, int>> cells;
    cells.push({inX, inY});
    parent[inY][inX] = {inX, inY};

    const int dx[] = {1, -1, 0, 0};
    const int dy[] = {0, 0, 1, -1};

    std::vector<std::pair<int, int>> path;
    bool found = false;

    while (!cells.empty() && !found)
    {
        std::pair<int, int> current = cells.front();
        cells.pop();
        int cx = current.first;
        int cy = current.second;

        if (grid[cy][cx] && !(cx == inX && cy == inY))
        {
            int bx = cx;
            int by = cy;
            while (!(bx == inX && by == inY))
            {
                path.push_back({bx, by});
                std::pair<int, int> p = parent[by][bx];
                bx = p.first;
                by = p.second;
            }
            found = true;
            break;
        }

        for (int d = 0; d < 4; d++)
        {
            int nx = cx + dx[d];
            int ny = cy + dy[d];

            if (nx >= 0 && nx < width && ny >= 0 && ny < height && parent[ny][nx].first == -1)
            {
                parent[ny][nx] = {cx, cy};
                cells.push({nx, ny});
            }
        }
    }

    if (!found)
    {
        return;
    }

    auto carveCell = [&](int x, int y)
    {
        if (x < 0 || x >= width || y < 0 || y >= height)
        {
            return;
        }

        grid[y][x] = true;
        level->floors.push_back(std::make_unique<Floor>(XYZEngine::Vector2Df{x * 128.f, y * 128.f}, 0));

        XYZEngine::Vector2Df cellPosition{x * 128.f, y * 128.f};
        for (auto it = level->walls.begin(); it != level->walls.end();)
        {
            auto transform = (*it)->GetGameObject()->GetComponent<XYZEngine::TransformComponent>();
            if (transform != nullptr && transform->GetWorldPosition() == cellPosition)
            {
                XYZEngine::GameWorld::Instance()->DestroyGameObject((*it)->GetGameObject());
                it = level->walls.erase(it);
            }
            else
            {
                ++it;
            }
        }
    };

    carveCell(inX, inY);
    for (const auto &cell : path)
    {
        carveCell(cell.first, cell.second);
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