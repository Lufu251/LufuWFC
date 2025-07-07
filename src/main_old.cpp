#include <raylib.h>
#include <chrono>

#include <dachWFC.h>

uint64_t nanoTime(){
    using namespace std;
    using namespace chrono;
    return duration_cast<nanoseconds>(high_resolution_clock::now().time_since_epoch()).count();
}

int main(void){

    const int screenWidth = 1200;
    const int screenHeight = 1200;
    InitWindow(screenWidth, screenHeight, "wfc");
    SetTargetFPS(60);

    // Create and Define a new tileSet
    std::vector<Tile> tileSet = {
        Tile(0), Tile(1), Tile(2), Tile(3), Tile(4), Tile(5), Tile(6), Tile(7)
    };
    /*
    // Define Tile 0
    tileSet[0].compatibleNeighbors[UP] = {0, 2, 5, 7};
    tileSet[0].compatibleNeighbors[RIGHT] = {0, 4, 5, 6};
    tileSet[0].compatibleNeighbors[DOWN] = {0, 3, 4, 7};
    tileSet[0].compatibleNeighbors[LEFT] = {0, 2, 3, 6};
    
    // Define Tile 1
    tileSet[1].compatibleNeighbors[UP] = {1, 3, 4, 6};
    tileSet[1].compatibleNeighbors[RIGHT] = {1, 2, 3, 7};
    tileSet[1].compatibleNeighbors[DOWN] = {1, 2, 5, 6};
    tileSet[1].compatibleNeighbors[LEFT] = {1, 4, 5, 7};

    // Define Tile 2
    tileSet[2].compatibleNeighbors[UP] = {1, 3, 4, 6};
    tileSet[2].compatibleNeighbors[RIGHT] = {0, 4, 5, 6};
    tileSet[2].compatibleNeighbors[DOWN] = {0, 3, 4, 7};
    tileSet[2].compatibleNeighbors[LEFT] = {1, 4, 5, 7};

    // Define Tile 3
    tileSet[3].compatibleNeighbors[UP] = {0, 2, 5, 7};
    tileSet[3].compatibleNeighbors[RIGHT] = {0, 4, 5, 6};
    tileSet[3].compatibleNeighbors[DOWN] = {1, 2, 5, 6};
    tileSet[3].compatibleNeighbors[LEFT] = {1, 4, 5, 7};

    // Define Tile 4
    tileSet[4].compatibleNeighbors[UP] = {0, 2, 5, 7};
    tileSet[4].compatibleNeighbors[RIGHT] = {1, 2, 3, 7};
    tileSet[4].compatibleNeighbors[DOWN] = {1, 2, 5, 6};
    tileSet[4].compatibleNeighbors[LEFT] = {0, 2, 3, 6};

    // Define Tile 5
    tileSet[5].compatibleNeighbors[UP] = {1, 3, 4, 6};
    tileSet[5].compatibleNeighbors[RIGHT] = {1, 2, 3, 7};
    tileSet[5].compatibleNeighbors[DOWN] = {0, 3, 4, 7};
    tileSet[5].compatibleNeighbors[LEFT] = {0, 2, 3, 6};

    // Define Tile 6
    tileSet[6].compatibleNeighbors[UP] = {1, 3, 4};
    tileSet[6].compatibleNeighbors[RIGHT] = {0, 4, 5};
    tileSet[6].compatibleNeighbors[DOWN] = {1, 2, 5};
    tileSet[6].compatibleNeighbors[LEFT] = {0, 2, 3};

    // Define Tile 7
    tileSet[7].compatibleNeighbors[UP] = {0, 2, 5, 7};
    tileSet[7].compatibleNeighbors[RIGHT] = {1, 2, 3, 7};
    tileSet[7].compatibleNeighbors[DOWN] = {0, 3, 4, 7};
    tileSet[7].compatibleNeighbors[LEFT] = {1, 4, 5, 7};
    */

   // Define Tile 0
    tileSet[0].compatibleNeighbors[UP] = {0, 2, 5, 7};
    tileSet[0].compatibleNeighbors[RIGHT] = {0, 4, 5, 6};
    tileSet[0].compatibleNeighbors[DOWN] = {0, 3, 4, 7};
    tileSet[0].compatibleNeighbors[LEFT] = {0, 2, 3, 6};
    
    // Define Tile 1
    tileSet[1].compatibleNeighbors[UP] = {1, 3, 4, 6};
    tileSet[1].compatibleNeighbors[RIGHT] = {1, 2, 3, 7};
    tileSet[1].compatibleNeighbors[DOWN] = {1, 2, 5, 6};
    tileSet[1].compatibleNeighbors[LEFT] = {1, 4, 5, 7};

    // Define Tile 2
    tileSet[2].compatibleNeighbors[UP] = {1, 6};
    tileSet[2].compatibleNeighbors[RIGHT] = {0, 6};
    tileSet[2].compatibleNeighbors[DOWN] = {0, 7};
    tileSet[2].compatibleNeighbors[LEFT] = {1, 7};

    // Define Tile 3
    tileSet[3].compatibleNeighbors[UP] = {0, 7};
    tileSet[3].compatibleNeighbors[RIGHT] = {0, 6};
    tileSet[3].compatibleNeighbors[DOWN] = {1, 6};
    tileSet[3].compatibleNeighbors[LEFT] = {1, 7};

    // Define Tile 4
    tileSet[4].compatibleNeighbors[UP] = {0, 7};
    tileSet[4].compatibleNeighbors[RIGHT] = {1, 7};
    tileSet[4].compatibleNeighbors[DOWN] = {1, 6};
    tileSet[4].compatibleNeighbors[LEFT] = {0, 6};

    // Define Tile 5
    tileSet[5].compatibleNeighbors[UP] = {1, 6};
    tileSet[5].compatibleNeighbors[RIGHT] = {1, 7};
    tileSet[5].compatibleNeighbors[DOWN] = {0, 7};
    tileSet[5].compatibleNeighbors[LEFT] = {0, 6};

    // Define Tile 6
    tileSet[6].compatibleNeighbors[UP] = {1};
    tileSet[6].compatibleNeighbors[RIGHT] = {0};
    tileSet[6].compatibleNeighbors[DOWN] = {1};
    tileSet[6].compatibleNeighbors[LEFT] = {0};

    // Define Tile 7
    tileSet[7].compatibleNeighbors[UP] = {0, 2, 5, 7};
    tileSet[7].compatibleNeighbors[RIGHT] = {1, 2, 3, 7};
    tileSet[7].compatibleNeighbors[DOWN] = {0, 3, 4, 7};
    tileSet[7].compatibleNeighbors[LEFT] = {1, 4, 5, 7};

    // Create dachWFC generator
    DachWFC dwfc(400,400, std::move(tileSet));

    std::cout << "START GENERATION:" << std::endl;

    uint64_t t1 = nanoTime();
    // Generate the grid
    dwfc.generate();
    uint64_t t2 = nanoTime();
    std::cout << (float)(t2 - t1) / 1000000000.0 << " seconds\n";

    while (!WindowShouldClose())
    {
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(GRAY);
        int size = 10;
        for (size_t x = 0; x < dwfc.grid.sizeX; x++){
            for (size_t y = 0; y < dwfc.grid.sizeY; y++){
                size_t i = *dwfc.grid(x,y).begin();
                // line horizontal
                if(i == 0){
                    DrawLine(x*size, y*size + size/2, x*size + size, y*size + size/2, GREEN);
                }
                // line vertical
                else if(i == 1){
                    DrawLine(x*size + size/2, y*size, x*size + size/2, y*size + size, GREEN);
                }
                // line Top Right
                else if(i == 2){
                    DrawLine(x*size + size/2, y*size + size/2, x*size +size/2, y*size, GREEN);
                    DrawLine(x*size + size/2, y*size + size/2, x*size +size, y*size + size/2, GREEN);
                }
                // line Bottom Right
                else if(i == 3){
                    DrawLine(x*size + size/2, y*size + size/2, x*size +size/2, y*size + size, GREEN);
                    DrawLine(x*size + size/2, y*size + size/2, x*size +size, y*size + size/2, GREEN);
                }
                // line Left Bottom
                else if(i == 4){
                    DrawLine(x*size + size/2, y*size + size/2, x*size +size/2, y*size + size, GREEN);
                    DrawLine(x*size + size/2, y*size + size/2, x*size, y*size + size/2, GREEN);
                }
                // line Left Top
                else if(i == 5){
                    DrawLine(x*size + size/2, y*size + size/2, x*size, y*size + size/2, GREEN);
                    DrawLine(x*size + size/2, y*size + size/2, x*size +size/2, y*size, GREEN);
                }
                // Line Cross
                else if(i == 6){
                    DrawLine(x*size, y*size + size/2, x*size + size, y*size + size/2, GREEN);
                    DrawLine(x*size + size/2, y*size, x*size + size/2, y*size + size, GREEN);
                    
                }
                // Blank
                else if(i == 7){
                    DrawRectangle(x*size,y*size, size,size,GRAY);
                }
            }
        }
    EndDrawing();
    //----------------------------------------------------------------------------------
    }
    return 0;
}