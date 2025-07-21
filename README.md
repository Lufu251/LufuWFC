# LufuWFC
This is my own implementation of the WaveFunctionCollapse Alogrithm, that generates similar output from an input bitmap.

* [Features](#features)
* [Notes](#notes)
* [Building](#building)
* [Media](#media)
* [License](#license)

## Features
* (Add a features of your project here)

## Notes
### Analogy: Quantum Mechanics

Imagine every single spot (or "cell") in your final output grid is in a state of superposition. This means it hasn't decided what it is yet; it is simultaneously all possible tiles at once.

1. Observation: You pick one cell that is still in superposition and "observe" it. This forces it to make a choice.

2. Collapse: The observation causes the cell's "wave function" to collapse. It randomly chooses one single state (one tile) from its list of possibilities, becoming fixed.

3. Propagation (Entanglement): This is the magic. The choice made by the collapsed cell has consequences for its neighbors. If you place a "beach" tile, its neighbor can no longer be a "mountain peak." This information "propagates" outwards, reducing the possibilities for neighboring cells. This process continues until the system is stable again.

Repeat this cycle—Observe, Collapse, Propagate—until every cell in the grid has collapsed into a single, determined state.

## Building

### Prerequisites
* **CMake** (3.15 or newer)
* **C++ Compiler** (e.g.,GCC, Clang, MSVC)
* **Git** (for fetching dependencies)
* **Emscripten SDK** (only for web builds)
* **Python 3.8+** (only for web builds)

### Desktop Build (Windows, macOS, Linux)
1. **Configure and build with CMake:**
    ```bash
    mkdir build
    cd build
    cmake ..
    cmake --build .
    ```
    The example binaries will be located in the `build/bin` directory.

### Web Build (WebAssembly)
1.  **Configure and build with Emscripten**
    ```bash
    cd tools
    ./build_web.sh
    ```
    Now there should be a HTTP Server running under port 8000 to test your project. The URL should be visible in the active terminal.

## Media
### Example solve with landtiles tileset:
![landtiles_example_solve](media/land_example.png)

### Example solve with pathtiles tileset:
![pathtiles_example_solve](media/street_example.png)


## License
This project is licensed under the [LICENSE_NAME] License - see the [LICENSE](LICENSE) file for details.