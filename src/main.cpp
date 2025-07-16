#include <raylib.h>

#include <lufuWFC.hpp>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

void drawGrid(const lufuWFC::Grid& grid){
    int size = 50;
    Color colors[10] = {RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE, BROWN};

    for (int x=0; x<grid.mX; x++) {
        for (int y=0; y<grid.mY; y++) {
            if(grid(x,y).collapsed){
                DrawRectangle(x*size, y*size, size, size, colors[grid(x,y).possibleTiles[0]]);
            } else {
                DrawRectangle(x*size, y*size, size, size, BLACK);
                std::string text;
                for(auto& tile : grid(x,y).possibleTiles)
                    text += std::to_string(tile).append(" ");

                DrawText(text.c_str(), x*size, y*size + size/2, 5, WHITE);
            }
        }
    }
}

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------

    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib quick start");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    
    lufuWFC::WFC wfc;
    lufuWFC::TileSet landTiles;
    landTiles.loadFromFile("../../tileset.json");
    landTiles.print();

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------


        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(DARKBLUE);

            if(GuiButton({screenWidth - 130, 30, 100, 30}, "Initialize")){
                wfc.initialize(8, 8, landTiles);
            }

            if(GuiButton({screenWidth - 130, 90, 100, 30}, "Step")){
                wfc.step();
            }

            drawGrid(wfc.grid);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}