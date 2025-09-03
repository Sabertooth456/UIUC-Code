/* Your code here! */
#include "maze.h"
#include <random>
#include <queue>

SquareMaze::SquareMaze() {}

void SquareMaze::makeMaze(int width, int height) {
    if (width < 0 || height < 0) {
        std::cout << "makeMaze() : Invalid width or height" << std::endl;
        return;
    }
    maze_.clear();
    width_ = width;
    height_ = height;
    mazeSolved = false;
    solution = std::vector<int>();
    tiles_ = DisjointSets();
    tiles_.addelements(width_ * height_);
    maze_ = std::vector<std::vector<Tile>>(height_, std::vector<Tile>(width_, Tile()));
    std::vector<int> walls;
    for (int idx = 0; idx < width_ * height_ * 2; idx++) {
        walls.push_back(idx);
    }
    while(!walls.empty()) {
        int idx = rand() % walls.size();
        int curTile = walls[idx] / 2;
        int dir = walls[idx] % 2;
        int x = curTile % width_;
        int y = curTile / width_;
        if (!goesOutOfBounds(x, y, dir) && !formsCycle(x, y, dir)) {
            setWall(x, y, dir, false);
            int index = coordinateToIndex(x, y);
            if (dir == 0) {
                int new_index = coordinateToIndex(x + 1, y);
                tiles_.setunion(index, new_index);
            } else {
                int new_index = coordinateToIndex(x, y + 1);
                tiles_.setunion(index, new_index);
            }
        }
        std::swap(walls[idx], walls[walls.size() - 1]);
        walls.pop_back();
    }
    return;
}

bool SquareMaze::formsCycle(int x, int y, int dir) {
    int index = coordinateToIndex(x, y);
    if (dir == 0) {
        int new_x = x + 1;
        int new_y = y;
        int new_index = coordinateToIndex(new_x, new_y);
        return (tiles_.find(index) != -1 && tiles_.find(index) == tiles_.find(new_index));
    } else {
        int new_x = x;
        int new_y = y + 1;
        int new_index = coordinateToIndex(new_x, new_y);
        return (tiles_.find(index) != -1 && tiles_.find(index) == tiles_.find(new_index));
    }
}

bool SquareMaze::canTravel(int x, int y, int dir) const {
    if (dir < 0 || dir > 3) {
        std::cout << "canTravel() : invalid dir parameter" << std::endl;
        return false;
    }
    if (!validCoordinate(x, y)) {
        return false;
    }
    // dir = 0 represents a rightward step (+1 to the x coordinate)
    // dir = 1 represents a downward step (+1 to the y coordinate)
    // dir = 2 represents a leftward step (-1 to the x coordinate)
    // dir = 3 represents an upward step (-1 to the y coordinate)
    if (dir == 0) {
        return !maze_[y][x].eastExists;
    } else if (dir == 1) {
        return !maze_[y][x].southExists;
    } else if (dir == 2) {
        return canTravel(x - 1, y, 0);
    } else {
        return canTravel(x, y - 1, 1);
    }
}

void SquareMaze::setWall(int x, int y, int dir, bool exists) {
    if (!validCoordinate(x, y)) {
        std::cout << "setWall() : invalid x or y parameter" << std::endl;
        return;
    }
    if (dir == 0) {
        maze_[y][x].eastExists = exists;
    } else if (dir == 1) {
        maze_[y][x].southExists = exists;
    } else {
        std::cout << "setWall() : invalid dir parameter" << std::endl;
    }
}

std::vector<int> SquareMaze::solveMaze() {
    if (mazeSolved) {
        return solution;
    }
    std::queue<std::pair<int, int>> queue; // Queue is ordered by [row][col]
    queue.push(std::pair<int,int>(0,0));
    maze_[0][0].distance = 0;
    while (!queue.empty()) {
        std::pair<int, int> cord = queue.front();
        queue.pop();
        int row = cord.first;
        int col = cord.second;
        int curDist = maze_[row][col].distance;
        // dir = 0 represents a rightward step (+1 to the x coordinate)
        // dir = 1 represents a downward step (+1 to the y coordinate)
        // dir = 2 represents a leftward step (-1 to the x coordinate)
        // dir = 3 represents an upward step (-1 to the y coordinate)
        if (canTravel(col, row, 0) && maze_[row][col + 1].distance == -1) {
            maze_[row][col + 1].distance = curDist + 1;
            queue.push(std::pair<int, int>(row, col + 1));
        }
        if (canTravel(col, row, 1) && maze_[row + 1][col].distance == -1) {
            maze_[row + 1][col].distance = curDist + 1;
            queue.push(std::pair<int, int>(row + 1, col));
        }
        if (canTravel(col, row, 2) && maze_[row][col - 1].distance == -1) {
            maze_[row][col - 1].distance = curDist + 1;
            queue.push(std::pair<int, int>(row, col - 1));
        }
        if (canTravel(col, row, 3) && maze_[row - 1][col].distance == -1) {
            maze_[row - 1][col].distance = curDist + 1;
            queue.push(std::pair<int, int>(row - 1, col));
        }
    }

    int longestPath = -1;
    int longestIndex = 0;
    for (int idx = 0; idx < width_; idx++) {
        if (maze_[height_ - 1][idx].distance > longestPath) {
            longestPath = maze_[height_ - 1][idx].distance;
            longestIndex = idx;
        }
    }

    std::vector<int> reversedPath;
    std::pair<int, int> curPos(height_ - 1, longestIndex); // Organized by [row][col]
    while (curPos.first != 0 || curPos.second != 0) {
        int row = curPos.first;
        int col = curPos.second;
        int distance = maze_[row][col].distance;
        if (canTravel(col, row, 0) && maze_[row][col + 1].distance == distance - 1) {
            curPos.second++;
            reversedPath.push_back(2);
        } else if (canTravel(col, row, 1) && maze_[row + 1][col].distance == distance - 1) {
            curPos.first++;
            reversedPath.push_back(3);
        } else if (canTravel(col, row, 2) && maze_[row][col - 1].distance == distance - 1) {
            curPos.second--;
            reversedPath.push_back(0);
        } else if (canTravel(col, row, 3) && maze_[row - 1][col].distance == distance - 1) {
            curPos.first--;
            reversedPath.push_back(1);
        } else {
            std::cout << "solveMaze() : Something went horribly wrong" << std::endl;
            break;
        }
    }
    solution = reverse(reversedPath);
    mazeSolved = true;
    return solution;
}

std::vector<int> SquareMaze::reverse(std::vector<int>& path) {
    std::vector<int> reversed;
    for (size_t idx = path.size() - 1; idx > 0; idx--) {
        reversed.push_back(path[idx]);
    }
    reversed.push_back(path[0]);
    return reversed;
}

cs225::PNG* SquareMaze::drawMaze() const {
    cs225::PNG* picture = new cs225::PNG(width_ * 10 + 1, height_ * 10 + 1);
    for (int col = 10; col < width_ * 10; col++) {
        picture->getPixel(col, 0).l = 0;
    }
    for (int row = 0; row < height_ * 10; row++) {
        picture->getPixel(0, row).l = 0;
    }
    for (int row = 0; row < height_; row++) {
        for (int col = 0; col < width_; col++) {
            if (maze_[row][col].eastExists) {
                for (int idx = 0; idx <= 10; idx++) {
                    picture->getPixel((col + 1) * 10, row * 10 + idx).l = 0;
                }
            }
            if (maze_[row][col].southExists) {
                for (int idx = 0; idx <= 10; idx++) {
                    picture->getPixel(col * 10 + idx, (row + 1) * 10).l = 0;
                }
            }
        }
    }
    return picture;
}
cs225::PNG* SquareMaze::drawMazeWithSolution() {
    cs225::PNG* picture = drawMaze();
    solveMaze();
    std::pair<int, int> curPos(0,0);
    std::pair<int, int> curPixel(5,5); // Organized by [row][col]
    // dir = 0 represents a rightward step (+1 to the x coordinate)
    // dir = 1 represents a downward step (+1 to the y coordinate)
    // dir = 2 represents a leftward step (-1 to the x coordinate)
    // dir = 3 represents an upward step (-1 to the y coordinate)
    for (int direction : solution) {
        makeRed(picture->getPixel(curPixel.second, curPixel.first));
        if (direction == 0) {
            curPos.second++;
            for (int idx = 0; idx < 10; idx++) {
                curPixel.second++;
                makeRed(picture->getPixel(curPixel.second, curPixel.first));
            }
        } else if (direction == 1) {
            curPos.first++;
            for (int idx = 0; idx < 10; idx++) {
                curPixel.first++;
                makeRed(picture->getPixel(curPixel.second, curPixel.first));
            }            
        } else if (direction == 2) {
            curPos.second--;
            for (int idx = 0; idx < 10; idx++) {
                curPixel.second--;
                makeRed(picture->getPixel(curPixel.second, curPixel.first));
            }
        } else {
            curPos.first--;
            for (int idx = 0; idx < 10; idx++) {
                curPixel.first--;
                makeRed(picture->getPixel(curPixel.second, curPixel.first));
            }
        }
    }
    for (int idx = 1; idx <= 9; idx++) {
        picture->getPixel(curPos.second * 10 + idx, (curPos.first + 1) * 10).l = 1;
    }
    return picture;
}

void SquareMaze::makeRed(cs225::HSLAPixel& pixel) {
    pixel.h = 0;
    pixel.s = 1;
    pixel.l = 0.5;
    pixel.a = 1;
}

int SquareMaze::coordinateToIndex(int x, int y) const {
    return x + y * width_;
}

bool SquareMaze::validCoordinate(int x, int y) const {
    return (x >= 0 && x < width_) && (y >= 0 && y < height_);
}

int SquareMaze::onEdge(int x, int y, int axis) const {
    if (axis == 0) {
        if (x == width_ - 1) {
            return 0;
        } else if (x == 0) {
            return 2;
        }
    }
    else if (axis == 1) {
        if (y == height_ - 1) {
            return 1;
        } else if (y == 0) {
            return 3;
        }
    }
    return -1;
}

bool SquareMaze::goesOutOfBounds(int x, int y, int dir) const {
    return onEdge(x, y, dir % 2) == dir;
}