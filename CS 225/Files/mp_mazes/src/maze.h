/* Your code here! */
#pragma once
#include <vector>
#include "../lib/cs225/PNG.h"
#include "./dsets.h"

#define HOI "TEM"

struct Tile {
    Tile() {};
    Tile(bool hasEastWall, bool hasSouthWall): eastExists(hasEastWall), southExists(hasSouthWall) {};

    int distance = -1;
    bool eastExists = true;
    bool southExists = true;
};

class SquareMaze {
public:
    SquareMaze(); //
    void makeMaze(int width, int height); //
    bool canTravel(int x, int y, int dir) const; //
    void setWall(int x, int y, int dir, bool exists); //
    std::vector<int> solveMaze();
    cs225::PNG* drawMaze() const;
    cs225::PNG* drawMazeWithSolution();
private:
    /** Converts the given coordinates to their respective
     * index value in the maze. Does not check if x or y is negative
    */
    int coordinateToIndex(int x, int y) const; //

    /** Check if the coordinate is within the bounds of the maze */
    bool validCoordinate(int x, int y) const; //

    /** If axis == 0 it returns
     * -1 if its false
     * 0 if the coordinate is on the east side
     * 2 if the coordinate is on the west side
     * 
     * If axis == 1 it returns
     * -1 if it is false
     * 1 if the coordinate is on the south side
     * 3 if the coordinate is on the north side
      */
    int onEdge(int x, int y, int axis) const;

    /** Checks if the given coordinate and direction goes out of bounds */
    bool goesOutOfBounds(int x, int y, int dir) const;

    /** Checks if breaking the given wall forms a cycle. Does not check if the wall is invalid
     * @param dir 0 = east wall, 1 = south wall
     */
    bool formsCycle(int x, int y, int dir);

    /** Returns a reversed version of the given vector */
    std::vector<int> reverse(std::vector<int>& path);

    /** Makes a pixel red */
    void makeRed(cs225::HSLAPixel& pixel);

    int width_ = 0;
    int height_ = 0;

    /** The maze is has width * height indices and is organized by [row, col] */
    std::vector<std::vector<Tile>> maze_;
    bool mazeSolved = false;
    std::vector<int> solution;
    
    DisjointSets tiles_;
};