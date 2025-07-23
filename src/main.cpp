#include <raylib.h>

#include <lufuWFC.hpp>
#include <chrono>

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

void drawGridLandTiles(const lufuWFC::Grid& grid){
    int size = 15;
    Color colors[10] = {YELLOW, BLUE, BROWN, GREEN, GRAY, PURPLE, RED};

    for (size_t x=0; x<grid.mX; x++) {
        for (size_t y=0; y<grid.mY; y++) {
            if(grid(x,y).collapsed){
                DrawRectangle(x*size, y*size, size, size, colors[grid(x,y).possibleTiles[0]]);
            } else {
                DrawRectangle(x*size, y*size, size, size, BLACK);
                std::string text = std::to_string(grid(x,y).possibleTiles.size());

                DrawText(text.c_str(), x*size, y*size + size/2, 3, WHITE);
            }
        }
    }
}

void drawGridPathTiles(const lufuWFC::Grid& grid){
    int size = 8;

    for (size_t x=0; x<grid.mX; x++) {
        for (size_t y=0; y<grid.mY; y++) {
            if(grid(x,y).collapsed){
                Vector2 up = {static_cast<float>(x*size) +static_cast<float>(size)/2.f, static_cast<float>(y*size)};
                Vector2 down = {static_cast<float>(x*size) +static_cast<float>(size)/2.f, static_cast<float>(y*size + size)};
                Vector2 right = {static_cast<float>(x*size + size), static_cast<float>(y*size) +static_cast<float>(size)/2.f};
                Vector2 left = {static_cast<float>(x*size), static_cast<float>(y*size) +static_cast<float>(size)/2.f};

                if(grid(x,y).possibleTiles[0] == 0){
                    DrawRectangle(x*size, y*size, size, size, GRAY);
                } else if(grid(x,y).possibleTiles[0] == 1){
                    DrawRectangle(x*size, y*size, size, size, WHITE);
                    DrawLineEx(up, down, 2, BLACK);

                } else if(grid(x,y).possibleTiles[0] == 2){
                    DrawRectangle(x*size, y*size, size, size, WHITE);
                    DrawLineEx(right, left, 2, BLACK);
                    
                } else if(grid(x,y).possibleTiles[0] == 3){
                    DrawRectangle(x*size, y*size, size, size, WHITE);
                    DrawLineEx(up, right, 2, BLACK);

                } else if(grid(x,y).possibleTiles[0] == 4){
                    DrawRectangle(x*size, y*size, size, size, WHITE);
                    DrawLineEx(right, down, 2, BLACK);

                } else if(grid(x,y).possibleTiles[0] == 5){
                    DrawRectangle(x*size, y*size, size, size, WHITE);
                    DrawLineEx(down, left, 2, BLACK);

                } else if(grid(x,y).possibleTiles[0] == 6){
                    DrawRectangle(x*size, y*size, size, size, WHITE);
                    DrawLineEx(left, up, 2, BLACK);

                }
            } else {
                DrawRectangle(x*size, y*size, size, size, BLACK);
                std::string text = std::to_string(grid(x,y).possibleTiles.size());

                DrawText(text.c_str(), x*size, y*size + size/2, 3, WHITE);
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

    const int screenWidth = 900;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "WFC Test");

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    
    lufuWFC::WFC wfc;
    lufuWFC::TileSet tileset;
    tileset.loadFromFile("../../examples/pathtiles.json");

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(DARKBLUE);

            if(wfc.grid.mData.size() > 0){
                drawGridPathTiles(wfc.grid);
            }

            DrawRectangleRec({screenWidth - 140, 0, 140, screenHeight}, {200,190,200, 200});
            if(GuiButton({screenWidth - 120, 30, 100, 30}, "Initialize")){
                wfc.initialize(64, 64, 0, tileset);
            }

            if(GuiButton({screenWidth - 120, 70, 100, 30}, "Solve")){
                wfc.solve(-1, 10);
            }

            if(GuiButton({screenWidth - 120, 110, 100, 30}, "Step")){
                wfc.solve(1, 10);
            }
            if(GuiButton({screenWidth - 120, 150, 100, 30}, "Backtrack")){
                wfc.tracker.revert(wfc.grid);
            }

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}