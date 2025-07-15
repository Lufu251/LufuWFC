#pragma once

#include <iostream>
#include <random>
#include <vector>
#include <array>
#include <map>
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
            std::cout << "--- Pass 1: Building name-to-index map ---\n";
            for (size_t i=0; i < data.size(); i++){
                std::string name = data[i]["name"];

                // Create mapping entry
                name_to_index_map[name] = i;
                std::cout << "Mapped \"" << name << "\" -> " << i << "\n";
            }

            // PASS 2: Build the final Tile objects using the map
            std::cout << "--- Pass 2: Loading full tile data --\n";
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
            std::cout << "--- Tile loading complete ---\n\n";
            std::cout << "--- Verification: Printing loaded data ---" << std::endl;
            print();
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
    };

    struct Grid{
        int mX, mY;
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

        int getX(){ return mX; }
        int getY(){ return mY; }

        int index(size_t x, size_t y) const { return mX * y + x; }

        Cell& operator()(size_t x, size_t y){ return mData[index(x, y)]; }
        const Cell& operator()(size_t x, size_t y) const { return mData[index(x, y)]; }
    };

    class WFC{
    public:

        WFC(){}
        ~WFC(){}

        void initialize(int width, int height, TileSet& tileset){
            // Set tileset
            mTileset = tileset;

            // Is not collapsed
            mCollapsed = false;

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

        // This function performs one "Observe & Propagate" cycle.
        void step(){
            // If already done, do nothing
            if(mCollapsed) return;

            // --- Observation ---
            // Find the cell with the lowest entropy > 1
            Cell* targetCell = findLowestEntropyCell();

            // Check if cell is null if true everything is collapsed
            if(targetCell == nullptr){
                mCollapsed = true;
                std::cout << "WFC complete" << std::endl;
                return;
            }

            // --- Collapse ---
            collapseCell(targetCell->x, targetCell->y);

            // --- Propagation ---
            propagate(targetCell->x, targetCell->y);
        }

    private:
        bool mCollapsed;
        TileSet mTileset;
        Grid grid;
        
        // Finds the uncollapsed cell with the fewest options.
        Cell* findLowestEntropyCell() {
            size_t minEntropy = SIZE_MAX;
            std::vector<Cell*> candidateCells;

            // Search lowest entropy cells
            for (auto& cell : grid.mData) {

                // Skip if cell is collapsed
                if(cell.collapsed){
                    std::cout << "Cell collapsed" << std::endl;
                    continue;
                }
                
                if(cell.possibleTiles.size() < minEntropy){
                    // Found new minimum, discard previous results and add cell
                    minEntropy = cell.possibleTiles.size();
                    candidateCells.clear();
                    candidateCells.push_back(&cell);
                } else if (cell.possibleTiles.size() == minEntropy) {
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
            std::cout << "Collapsed cell x:" << x << "y: " << y <<  " New state: " << tileID << std::endl;
            cell.possibleTiles.clear();
            cell.possibleTiles.push_back(tileID);
            cell.collapsed = true;
        }

        // Propagate the wave from a starting point
        void propagate(size_t x, size_t y){

        }

        // Generate a random int from zero to n -1
        size_t generateRandomInt(size_t start, size_t end) {
            // The generator is initialized only once, on the first call
            static std::mt19937 generator = [] {
                std::random_device rd;
                return std::mt19937(rd());
            }();

            // The distribution is created on each call, which is cheap.
            // This allows the range [0, n] to be different for each call.
            std::uniform_int_distribution<size_t> distribution(start, end);

            return distribution(generator);
        }

    };
}