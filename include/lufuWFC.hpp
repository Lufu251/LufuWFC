#pragma once

#include <vector>
#include <queue>
#include <set>
#include <array>
#include <map>

#include <iostream>
#include <random>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>

namespace lufuWFC{

    struct Tile{
        int index;
        std::string name;
        int weight;

        // Neighbors
        std::array<std::vector<int>, 4> adjacency;
    };

    struct TileSet{
        std::map<std::string, int> name_to_index_map;
        std::vector<Tile> tiles;

        void loadFromFile(std::string name){
            std::cout << "\n--- Start Loading tileset from file ---\n";
            // Open and parse the JSON file
            std::ifstream file(name);
            if (!file.is_open()) {
                std::cerr << "Error: Could not open tileset.json" << std::endl;
            }

            // Parse file to json data
            nlohmann::json data;
            try {
                file >> data;
            } catch (nlohmann::json::parse_error& e) {
                std::cerr << "JSON parse error: " << e.what() << std::endl;
            }
            
            // PASS 1: Build the name-to-index map
            std::cout << "  Pass 1: Building name-to-index map\n";
            for (size_t i=0; i < data.size(); i++){
                std::string name = data[i]["name"];

                // Create mapping entry
                name_to_index_map[name] = i;
                std::cout << "      Mapped \"" << name << "\" -> " << i << "\n";
            }

            // PASS 2: Build the final Tile objects using the map
            std::cout << "  Pass 2: Loading full tile data\n\n";
            const std::vector<std::string> directions = {"north", "east", "south", "west"};

            for (const auto& tile_json : data) {
                Tile current_tile;
                current_tile.name = tile_json["name"];
                current_tile.weight = tile_json["weight"];

                // Process adjacency rules
                const auto& adjacency_json = tile_json["adjacency"];
                for (size_t i = 0; i < directions.size(); ++i) {
                    const std::string& dir = directions[i];

                    // Get the array of names for the current direction (e.g., ["sand", "water"])
                    const auto& adjacent_names = adjacency_json[dir];

                    // Convert each name to its index using our map
                    for (const auto& name_json : adjacent_names) {
                        int adjacent_index = name_to_index_map.at(name_json.get<std::string>());
                        current_tile.adjacency[i].push_back(adjacent_index);
                    }
                }

                tiles.push_back(current_tile);
            }
            
            std::cout << "--- Verification: Printing loaded data ---" << std::endl;
            print();

            std::cout << "--- Tile loading complete ---\n\n";
        }

        void print() {
            const std::vector<std::string> directions = {"North", "East", "South", "West"};
            for (const auto& tile : tiles) {
                std::cout << "===================================\n";
                std::cout << "Tile: " << tile.name << " (Index: " << name_to_index_map.at(tile.name)  << ")\n";
                std::cout << "Weight: " << tile.weight << "\n";
                std::cout << "Adjacency Rules (by index):\n";
                for (size_t i = 0; i < directions.size(); ++i) {
                    std::cout << "  - " << directions[i] << ": [ ";
                    for (int adj_index : tile.adjacency[i]) {
                        std::cout << adj_index << " ";
                    }
                    std::cout << "]\n";
                }
            }
            std::cout << "===================================\n";
        }
    };

    struct Cell{
        size_t x;
        size_t y;
        bool collapsed;
        std::vector<int> possibleTiles;

        size_t getEntropy(){ return possibleTiles.size(); }
    };

    struct Grid{
        size_t mX, mY;
        std::vector<Cell> mData;

        Grid(){}
        Grid(size_t& x, size_t& y){
            reserve(x, y);
        }
        
        void reserve(const size_t& x, const size_t& y){
            mData.reserve(x * y);
            mX = x;
            mY = y;
        }

        void resize(const size_t& x, const size_t& y){
            mData.resize(x * y);
            mX = x;
            mY = y;
        }

        bool isOutOfBound(const size_t& x, const size_t& y){
            return x < 0 or x >= mX or y < 0 or y >= mY;
        }

        int getX(){ return mX; }
        int getY(){ return mY; }

        int index(size_t x, size_t y) const { return mX * y + x; }

        Cell& operator()(size_t x, size_t y){ return mData[index(x, y)]; }
        const Cell& operator()(size_t x, size_t y) const { return mData[index(x, y)]; }
    };

    struct Neighbor{
        size_t di;
        size_t x;
        size_t y;
        Neighbor(const size_t xPos, const size_t yPos, const size_t directionIndex): di(directionIndex), x(xPos), y(yPos){}
    };

    class WFC{
    public:
        Grid grid;

        WFC(){}
        ~WFC(){}

        // Initialize grid width, height, seed and tileset. Seed 0 is a random seed
        void initialize(int width, int height, int seed, TileSet& tileset){
            std::cout << "\n--- Initialize WFC ---" << std::endl;
            // Set stepCount to zero
            stepCount = 0;

            // Set tileset
            mTileset = tileset;

            // Is not collapsed and no error
            mCollapsed = false;
            mError = false;

            // Random generator
            if(seed >= 0){
                gen = std::mt19937(seed);
            } else {
                gen = std::mt19937(rd());
            }

            // Create template vector with all tiles
            std::vector<int> templateTiles;
            templateTiles.reserve(tileset.tiles.size());
            for (size_t i=0; i < tileset.tiles.size(); i++) {
                templateTiles.push_back(i);
            }

            // Create the grid
            grid.resize(width, height);

            // Initialize each cell to be in superposition
            for (int x=0; x < grid.getX(); x++) {
                for (int y=0; y < grid.getY(); y++) {
                    grid(x,y).x = x;
                    grid(x,y).y = y;
                    grid(x,y).possibleTiles = templateTiles;
                    grid(x,y).collapsed = false;
                }
            }
        }

        // Solve the grid for n steps -1 means solve until done
        void solve(int count, int backtrack){
            while (count > 0 || count == -1) {
                Grid last;

                // Done set count to zero to end
                if(mCollapsed){
                    count = 0;
                }

                // Error occured do something
                if(mError){
                    if(backtrack == true){
                        // Go back to last state and try again
                        mError = false;
                        grid = last;
                        backtrack --;
                    } else {
                        // End
                        count = 0;
                        std::cout << "Unsolvable backtrack count zero" << std::endl;
                    }
                } else {
                    // Step
                    step();
                }

                // Reduce count
                if(count > 0){
                    count --;
                }
            }
        }

        // Place and propagate one tile manually
        void manualSetCell(size_t x, size_t y, std::string tileName){

            if(grid(x,y).collapsed){ std::cout << "Cell is already collapsed. Can't manually set cell" << std::endl; return; }

            // If already done, do nothing
            if(mCollapsed){ std::cout << "Nothing to do" << std::endl; return;}

            // --- Observation ---d
            // Find the cell with the lowest entropy > 1
            Cell* targetCell = &grid(x,y);

            // --- Collapse ---
            grid(x,y).possibleTiles.clear();
            grid(x,y).possibleTiles.push_back(mTileset.name_to_index_map[tileName]);
            grid(x,y).collapsed = true;

            // --- Propagation ---
            propagate(targetCell->x, targetCell->y);
        }

    private:
        std::mt19937 gen;
        std::random_device rd;

        int stepCount;
        bool mCollapsed = false;
        bool mError = false;
        TileSet mTileset;
        // 0: North (Y-1), 1: East (X+1), 2: South (Y+1), 3: West (X-1)
        std::array<std::pair<int, int>, 4> directions = {{{0,-1}, {1,0}, {0,1}, {-1,0}}};
        
        // This function performs one "Observe & Propagate" cycle.
        void step(){
            std::cout << "--- Step " << stepCount << " ---" << std::endl;
            stepCount ++;

            // If already done, do nothing
            if(mCollapsed){ std::cout << "Nothing to do" << std::endl;return;}

            // --- Observation ---
            // Find the cell with the lowest entropy > 1
            Cell* targetCell = findLowestEntropyCell();
            
            // Check if cell is null if true everything is collapsed
            if(targetCell == nullptr){
                mCollapsed = true;
                std::cout << "WFC complete" << std::endl;
                return;
            }
            std::cout << "- Observe\n";
            std::cout << "  Cell x:" << targetCell->x << " y:" << targetCell->y << std::endl;

            // --- Collapse ---
            std::cout << "- Collapse\n";
            std::string before = "";
            for(auto& tile : targetCell->possibleTiles)
                before += std::to_string(tile);

            collapseCell(targetCell->x, targetCell->y);
            std::cout << "  Old:" << before << " New:" << targetCell->possibleTiles[0] << std::endl;

            // --- Propagation ---
            std::cout << "- Propagate\n";
            propagate(targetCell->x, targetCell->y);
        }

        // Finds the uncollapsed cell with the fewest options.
        Cell* findLowestEntropyCell() {
            size_t minEntropy = SIZE_MAX;
            std::vector<Cell*> candidateCells;

            // Search lowest entropy cells
            for (auto& cell : grid.mData) {

                // Skip if cell is collapsed
                if(cell.collapsed){
                    continue;
                }
                
                if(cell.getEntropy() < minEntropy){
                    // Found new minimum, discard previous results and add cell
                    minEntropy = cell.getEntropy();
                    candidateCells.clear();
                    candidateCells.push_back(&cell);
                } else if (cell.getEntropy() == minEntropy) {
                    // Same minimum add to candidateCells
                    candidateCells.push_back(&cell);
                }
                // If element > current_min, do nothing
            }
            
            // If all cells are collapsed, return nullptr
            if(candidateCells.empty()){
                return nullptr;
            }

            if(candidateCells.size() > 1){
                // Multiple lowest entropy cells pick one randomly
                return candidateCells[generateRandomInt(0, candidateCells.size() -1)];
            } else {
                // Return the only lowest entropy cell
                return candidateCells[0];
            }
        }

        // Collapse a specific cell
        void collapseCell(size_t x, size_t y){
            Cell& cell = grid(x,y);

            // --- Random choice ---
            std::vector<int> cumulativeWeights;
            // Reserve space to avoid reallocations
            cumulativeWeights.reserve(cell.possibleTiles.size());

            // Calculate partial sums
            int totalWeight = 0;
            for (const auto& tile : cell.possibleTiles) {
                totalWeight += mTileset.tiles[tile].weight;
                cumulativeWeights.push_back(totalWeight);
            } 

            // Find the first element in range > randomWeight
            auto it = std::upper_bound(cumulativeWeights.begin(), cumulativeWeights.end(), generateRandomInt(0, totalWeight-1));
            int index = std::distance(cumulativeWeights.begin(), it);
            int tileID = cell.possibleTiles[index];
            
            // Set cell state permanently
            cell.possibleTiles.clear();
            cell.possibleTiles.push_back(tileID);
            cell.collapsed = true;
        }

        // Propagate the wave from a starting point
        void propagate(size_t x, size_t y){
            // Use a stack to keep track of cells whose possibilities have been reduced.
            // Start with the cell that just collapsed.
            std::queue<std::pair<int, int>> queue;
            queue.push({x,y});

            int count = 0;
            while (!queue.empty()) {
                std::pair<int, int> pos = queue.front();
                Cell& currentCell = grid(pos.first, pos.second);
                queue.pop();
                
                
                // Get all valid neighbors (up, right, down, left)
                for(auto neighborPos : getNeighbors(currentCell.x, currentCell.y)){
                    Cell& neighborCell = grid(neighborPos.x, neighborPos.y);
                    std::cout << "  Cell " << count << " / x:" << neighborCell.x << " y:" << neighborCell.y << std::endl;
                    count ++;

                    // Get the list of tiles in the neighbor that are still possible
                    std::set<int> validTiles = getValidTilesInDirection(currentCell, neighborPos.di);
                    std::vector<int> newTiles = getIntersectingTiles(neighborCell, validTiles);

                    std::string o, n;
                    for(auto& tile : neighborCell.possibleTiles)
                        o += std::to_string(tile);
                    for(auto& tile : newTiles)
                        n += std::to_string(tile);
                    

                    // If the state of the neighbor will be changed
                    if(neighborCell.possibleTiles.size() != newTiles.size()){
                        // If the cell has 0 possibilities, we have a contradiction!
                        if(newTiles.size() == 0){
                            std::cerr << "No Possible Tiles! Generation failed" <<std::endl;
                            mError = true;
                            return;
                        }
                        // Update the neighbor's state
                        std::cout << "      Add Old:" << o << " New:" << n << std::endl;
                        neighborCell.possibleTiles = newTiles;

                        // Add this neighbor to the queue
                        queue.push({neighborPos.x, neighborPos.y});
                    }
                }
            }
        }

        //---------------- Helpers ----------------
        // Generate a random int from zero to n -1
        size_t generateRandomInt(size_t start, size_t end) {
            // The distribution is created on each call, which is cheap.
            // This allows the range [0, n] to be different for each call.
            std::uniform_int_distribution<size_t> distribution(start, end);

            return distribution(gen);
        }

        // Get all neighbors
        std::vector<Neighbor> getNeighbors(int x, int y){
            // Vector that will contain all valid neighbors
            std::vector<Neighbor> neighbors;
            
            for(size_t i=0; i < directions.size(); i++){
                int nX = x + directions[i].first, nY = y + directions[i].second;

                if(grid.isOutOfBound(nX, nY)) continue; // Skip if cell is out of bound
                if(grid(nX,nY).collapsed) continue; // Skip if cell is collapse

                neighbors.push_back(Neighbor(nX, nY, i));
            }
            
            return neighbors;
        }

        std::set<int> getValidTilesInDirection(Cell& cell, size_t direction){
            std::set<int> validTiles;

            // Add all rules from every possibleTile from the cell in the direction together
            for(auto& tile : cell.possibleTiles){
                for(auto& rule : mTileset.tiles[tile].adjacency[direction]){
                    validTiles.insert(rule);
                }
            }

            return validTiles;
        }

        std::vector<int> getIntersectingTiles(Cell& cell, std::set<int>& validTiles){
            std::vector<int> intersections;
            for(auto& neighborTile : cell.possibleTiles){
                if(validTiles.count(neighborTile)){
                    intersections.push_back(neighborTile);
                }
            }
            return intersections;
        }
    };
}