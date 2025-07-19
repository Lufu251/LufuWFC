#include <raylib.h>

#include <lufuWFC.hpp>

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

void drawGridLandTiles(const lufuWFC::Grid& grid){
    int size = 10;
    Color colors[10] = {YELLOW, BLUE, BROWN, GREEN, GRAY, PURPLE, RED};

    for (size_t x=0; x<grid.mX; x++) {
        for (size_t y=0; y<grid.mY; y++) {
            if(grid(x,y).collapsed){
                DrawRectangle(x*size, y*size, size, size, colors[grid(x,y).possibleTiles[0]]);
            } else {
                DrawRectangle(x*size, y*size, size, size, BLACK);
                std::string text;
                for(auto& tile : grid(x,y).possibleTiles)
                    text += std::to_string(tile).append(" ");

                DrawText(text.c_str(), x*size, y*size + size/2, 3, WHITE);
            }
        }
    }
}

void drawGridStreet(const lufuWFC::Grid& grid){
    int size = 15;

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
                std::string text;
                for(auto& tile : grid(x,y).possibleTiles)
                    text += std::to_string(tile).append(" ");

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
    lufuWFC::TileSet landTiles;
    landTiles.loadFromFile("../../pathtiles.json");

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

            if(GuiButton({screenWidth - 130, 30, 100, 30}, "Initialize")){
                wfc.initialize(48, 48, 1, landTiles);
            }

            if(GuiButton({screenWidth - 130, 70, 100, 30}, "Step")){
                wfc.step();
            }
            if(GuiButton({screenWidth - 130, 110, 100, 30}, "Solve")){
                wfc.solve();
            }

            if(wfc.grid.mData.size() > 0)
                drawGridStreet(wfc.grid);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}