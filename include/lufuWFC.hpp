#pragma once

#include <iostream>
#include <vector>
#include <array>
#include <map>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>

struct Tile{
    int index;
    std::string name;
    int weight;

    // Neighbors
    std::array<std::vector<int>, 4> adjacency;

};

class WFC{
public:

    WFC(){}
    ~WFC(){}


    void loadTilesetFromFile(std::string name){
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

            tileset.push_back(current_tile);
        }
        std::cout << "--- Tile loading complete ---\n\n";
        std::cout << "--- Verification: Printing loaded data ---" << std::endl;
        printTileset();
    }

private:
    std::map<std::string, int> name_to_index_map;
    std::vector<Tile> tileset;

    void printTileset() {
    const std::vector<std::string> directions = {"North", "East", "South", "West"};
    for (const auto& tile : tileset) {
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