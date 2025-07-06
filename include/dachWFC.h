#pragma once

#include <vector>
#include <array>
#include <set>
#include <queue>
#include <cstdlib>
#include <iostream>

enum direction {UP, RIGHT, DOWN, LEFT};

struct Tile
{
    //
    size_t index;
    // How frequent the tile is
    size_t frequency = 0;
    // Define an array with vectors of int
    std::set<size_t> compatibleNeighbors[4];

    Tile(size_t i) : index(i) {}
};

struct Grid
{
    size_t sizeX;
    size_t sizeY;
    std::vector<std::set<size_t>> data;

    Grid() {}

    Grid(size_t x, size_t y) : sizeX(x), sizeY(y), data(x * y){
    }

    std::set<size_t>& operator()(size_t x, size_t y){
        return data[sizeX * y + x];
    }

    const std::set<size_t>& operator()(size_t x, size_t y) const{
        return data[sizeX * y + x];
    }
};

class DachWFC
{
public:
    std::vector<Tile> _tileSet;
    Grid grid;

    std::pair<size_t, size_t> getLowestEntropyCell(){
        size_t minEntropy = _tileSet.size() + 1;
        std::pair<size_t, size_t> cell = { -1, -1 };

        size_t sizeX = grid.sizeX;
        size_t sizeY = grid.sizeY;

        for (size_t x = 0; x < sizeX; ++x) {
            for (size_t y = 0; y < sizeY; ++y) {
                size_t entropy = grid(x,y).size();
                if(minEntropy == 2){
                    return cell;
                }
                if (entropy > 1 && entropy < minEntropy) {
                    minEntropy = entropy;
                    cell = { x, y };
                }
            }
        }
        return cell;
    }

    bool isFullyCollapsed() {
        for (const auto& cell : grid.data) {
            if (cell.size() > 1) {
                return false;
            }
        }
        return true;
    }

    void collapseCell(size_t x, size_t y){
        // pick a random possible cell
        // #ToDo take in consideration the frequency of the possible cells
        if (grid(x,y).size() > 1) {
            size_t chosenTile = *next(grid(x,y).begin(), rand() % grid(x,y).size());
            grid(x,y) = { chosenTile };
        }
    }
    
    void addSets(std::set<size_t>& set1, std::set<size_t>& set2, std::set<size_t>& addedSet){
        for(auto v : set1){
            if(set2.count(v)){
                addedSet.insert(v); // Tile is present in both sets
            }
        }
    }

    void propagateConstraints(size_t x, size_t y){
         std::queue<std::pair<size_t, size_t>> toPropagate;
         toPropagate.push({x,y});

        std::array<std::pair<int, int>, 4> directions = {
            std::make_pair(0, -1),  // UP
            std::make_pair(1, 0),   // RIGHT
            std::make_pair(0, 1),   // DOWN
            std::make_pair(-1, 0)   // LEFT
        };

        while (!toPropagate.empty()){
            auto [cx, cy] = toPropagate.front();
            toPropagate.pop();

            for (size_t direction = 0; direction < 4; direction++){
                size_t nx = cx + directions[direction].first;
                size_t ny = cy + directions[direction].second;
                
                // Check if neighbor is out of bound
                if (ny >= grid.sizeY || nx >= grid.sizeX) continue;

                // Check if the neighbor has only one possible option left
                if(grid(nx, ny).size() <= 1) continue;

                std::set<size_t> newPossibleTiles;

                // Add all possible tiles in that direction
                for (auto tile : grid(cx,cy)){
                    newPossibleTiles.insert(_tileSet[tile].compatibleNeighbors[direction].begin(), _tileSet[tile].compatibleNeighbors[direction].end());
                }
                // Check if the newPossibleTiles is the same as before
                if(newPossibleTiles != grid(nx,ny)){
                    std::set<size_t> addedPossibleTiles;
                    // Add sets to only extract values that are present in both sets
                    addSets(newPossibleTiles, grid(nx,ny), addedPossibleTiles);

                    if(addedPossibleTiles.empty()){
                        std::cout << "ERROR Can't work!!!" << std::endl;
                        std::cout << cx << " " << cy << std::endl;
                        std::cout << nx << " " << ny << std::endl;

                    }

                    if(addedPossibleTiles != grid(nx,ny)){
                        grid(nx,ny) = addedPossibleTiles;
                        toPropagate.push({nx,ny}); // Only add to queue if something has changed
                    }
                }
            }
        }
    }

public:
    DachWFC(unsigned width, size_t height, std::vector<Tile> tileSet) : _tileSet(std::move(tileSet)){
        grid = Grid(height, width);
        for (auto& cell : grid.data) {
            for (size_t i = 0; i < _tileSet.size(); ++i){
                cell.insert(i);
            }
        }
        
        // Set seed
        //srand(time(0));
        srand(10);
    }

    void generate(){
        while (!isFullyCollapsed()){
            // getLowestEntropyCell
            // Store the location of the lowest entropy cell
            auto lowestEntropyCell = getLowestEntropyCell();

            // CollapseCell
            collapseCell(lowestEntropyCell.first, lowestEntropyCell.second);

            // PropagateConstraints
            propagateConstraints(lowestEntropyCell.first, lowestEntropyCell.second);
        }
    }

    void printGrid() {
        for (size_t y = 0; y < grid.sizeY; y++) {
            for (size_t x = 0; x < grid.sizeX; x++) {
                auto cell = grid(x,y);
                if (cell.size() == 1) {
                    std::cout << *grid(x,y).begin() << " ";
                } else {
                    std::cout << "? ";
                }
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
};