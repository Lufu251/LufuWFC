# The Core Analogy: Quantum Mechanics

Imagine every single spot (or "cell") in your final output grid is in a state of superposition. This means it hasn't decided what it is yet; it is simultaneously all possible tiles at once.

1. Observation: You pick one cell that is still in superposition and "observe" it. This forces it to make a choice.

2. Collapse: The observation causes the cell's "wave function" to collapse. It randomly chooses one single state (one tile) from its list of possibilities, becoming fixed.

3. Propagation (Entanglement): This is the magic. The choice made by the collapsed cell has consequences for its neighbors. If you place a "beach" tile, its neighbor can no longer be a "mountain peak." This information "propagates" outwards, reducing the possibilities for neighboring cells. This process continues until the system is stable again.

You repeat this cycle—Observe, Collapse, Propagate—until every cell in the grid has collapsed into a single, determined state.

# Core Components (Data Structures)

Before we talk about functions, let's define the data you'll be working with.

1. Tiles (or Modules): These are your building blocks.
    - Examples: "grass", "sand", "water", "forest". Or, for a dungeon, "wall", "floor", "door".
    - Each tile should have a unique ID.

2. Rules (Adjacency Constraints): This is the most crucial part. The rules define which tiles can be placed next to which other tiles. A good way to store this is a lookup table.

    - For Tile A, what tiles are allowed to its RIGHT? [Tile B, Tile C]
    - For Tile A, what tiles are allowed to its LEFT? [Tile D]
    - ...and so on for UP and DOWN.
    - You typically pre-calculate these rules by analyzing an example input image or defining them manually.

3. The Grid: A 2D array (or 3D) representing your output space.

4. Cell: This is the most important data structure in your implementation. Each position in the grid is a Cell object. A Cell needs to store:
    - possible_tiles: A list/set of the IDs of tiles that could still potentially go in this spot. At the start, this list contains all tiles.
    - collapsed: A boolean flag, false by default. Becomes true when a final choice has been made.
    - entropy: An integer representing the number of possible_tiles. A lower entropy means fewer choices, making it a "more certain" cell. A cell with an entropy of 1 is fully collapsed.

# General Structure and Functions
Here is a blueprint for a class-based WFC solver that you can increment step-by-step.

```c++
class WaveFunctionCollapseSolver:
// --- PROPERTIES ---
grid: 2D array of Cell objects
width: integer
height: integer
rules: AdjacencyRules object
is_fully_collapsed: boolean

// --- MAIN METHODS ---

// 1. CONSTRUCTOR / INITIALIZATION
constructor(width, height, rules):
    // Store dimensions and rules
    this.width = width
    this.height = height
    this.rules = rules

    // Create the grid
    this.grid = new Grid(width, height)

    // Initialize each cell to be in superposition
    for every cell in this.grid:
        cell.possible_tiles = [all possible tile IDs from rules]
        cell.collapsed = false
        cell.recalculate_entropy() // Entropy = number of possible tiles

    this.is_fully_collapsed = false

// 2. THE INCREMENTAL STEP (THE HEART OF THE ALGORITHM)
// This function performs one "Observe & Propagate" cycle.
// Call this repeatedly in your main loop to see the process.
function step():
    // If already done, do nothing
    if this.is_fully_collapsed:
        return

    // === A. OBSERVATION PHASE ===
    // Find the cell with the lowest entropy > 1
    let target_cell = find_lowest_entropy_cell()

    // If no such cell exists, it means everything is collapsed. We're done!
    if target_cell == null:
        this.is_fully_collapsed = true
        print("WFC complete!")
        return

    // === B. COLLAPSE PHASE ===
    // Force the chosen cell to pick one state
    collapse_cell_at(target_cell.x, target_cell.y)

    // === C. PROPAGATION PHASE ===
    // The collapse creates a "wave" of new information. Propagate it.
    propagate(target_cell.x, target_cell.y)


// --- HELPER FUNCTIONS ---

// Finds the uncollapsed cell with the fewest options.
function find_lowest_entropy_cell():
    let min_entropy = infinity
    let candidate_cells = []

    for every cell in this.grid:
        if not cell.collapsed and cell.entropy < min_entropy:
            min_entropy = cell.entropy
            candidate_cells = [cell]
        else if not cell.collapsed and cell.entropy == min_entropy:
            candidate_cells.add(cell)

    // If all cells are collapsed, min_entropy will remain infinity
    if candidate_cells is empty:
        return null

    // If there's a tie for lowest entropy, pick one randomly
    return random_choice(candidate_cells)

// Collapses a specific cell.
function collapse_cell_at(x, y):
    let cell = this.grid[x][y]
    
    // Randomly pick one tile from its remaining possibilities
    // (You can use weighted random choice based on tile frequency)
    let chosen_tile_id = random_choice(cell.possible_tiles)
    
    // Set its state permanently
    cell.possible_tiles = [chosen_tile_id]
    cell.collapsed = true
    cell.recalculate_entropy() // Entropy is now 1

// The "wave" part. This is the most complex helper.
function propagate(start_x, start_y):
    // Use a stack to keep track of cells whose possibilities have been reduced.
    // Start with the cell that just collapsed.
    let propagation_stack = new Stack()
    propagation_stack.push({x: start_x, y: start_y})

    while propagation_stack is not empty:
        let current_pos = propagation_stack.pop()
        let current_cell = this.grid[current_pos.x][current_pos.y]
        
        // Get all valid neighbors (up, down, left, right)
        for each neighbor_pos in get_neighbors(current_pos.x, current_pos.y):
            let neighbor_cell = this.grid[neighbor_pos.x][neighbor_pos.y]
            
            // Get the list of tiles in the neighbor that are still possible
            let original_neighbor_possibilities = neighbor_cell.possible_tiles.copy()
            let new_neighbor_possibilities = []

            // Now, check which of the neighbor's tiles are still compatible
            // with the *current* state of `current_cell`.
            let valid_neighbor_tiles = get_valid_neighbors_for(current_cell, direction_to_neighbor)
            
            // The new possibilities are the intersection of what the neighbor
            // *had* and what the current cell *allows*.
            for tile_id in original_neighbor_possibilities:
                if tile_id is in valid_neighbor_tiles:
                    new_neighbor_possibilities.add(tile_id)
            
            // If we have reduced the neighbor's possibilities...
            if new_neighbor_possibilities.length < original_neighbor_possibilities.length:
                // ...update the neighbor's state
                neighbor_cell.possible_tiles = new_neighbor_possibilities
                neighbor_cell.recalculate_entropy()

                // If a cell has 0 possibilities, we have a contradiction!
                // The algorithm has failed. You can either restart or backtrack.
                if neighbor_cell.entropy == 0:
                    throw new Error("Contradiction found! Generation failed.")

                // Since this neighbor has changed, it might affect *its* neighbors.
                // So, add it to the stack to be processed.
                propagation_stack.push(neighbor_pos)

// A helper for the helper.
// Based on a cell's current `possible_tiles`, what are all the tiles that could
// legally be placed in a certain direction (e.g., "right")?
function get_valid_neighbors_for(cell, direction):
    let all_valid_tiles = new Set()
    for tile_id in cell.possible_tiles:
        // Look up the rules for this tile_id and direction
        let valid_for_this_tile = this.rules.get_valid(tile_id, direction)
        all_valid_tiles.add_all(valid_for_this_tile)
        return all_valid_tiles
```

# How to Use the Incremental Structure

Your main application loop would look like this:
Generated code

```c++
// 1. Define your tiles and analyze an image to get the rules.
let my_rules = generate_rules_from("sample.png")

// 2. Create the solver instance.
let solver = new WaveFunctionCollapseSolver(50, 50, my_rules)

// 3. Run the simulation step-by-step.
while not solver.is_fully_collapsed:
    // Perform one observation and propagation cycle
    solver.step()

    // After each step, you can render the current state of the grid.
    // For uncollapsed cells, you could color them based on their entropy.
    // For collapsed cells, draw the chosen tile.
    render_grid(solver.grid)

    // Add a small delay so you can watch it happen.
    wait(50 milliseconds)

// 4. Get the final result.
let final_grid = solver.grid
```