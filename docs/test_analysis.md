# Detailed Analysis of Game Behavior by Class

## Tank Class

- Has a position and direction (one of 8 directions)
- Has an ID/player number (1 or 2)
- Has ammunition (starting with 16 shells)
- Has a cooldown period after shooting (4 steps)
- Can be destroyed
- Can rotate (1/8 or 1/4 turns)
- Can move forward and backward
- Has special backward movement mechanics:
  - Takes 3 steps to initiate (2 steps waiting, then moves on 3rd step)
  - Subsequent backward moves only take 1 step if continuous
  - Can be canceled by forward movement
- Tracks previous position (for collision detection)
- Cannot perform actions while destroyed

## Shell Class

- Has a position and direction
- Has an owner ID (player who fired it)
- Moves at a pace of 2 units per game step
- Can be destroyed (on collision)
- Tracks previous position (for collision detection)
- Has a next position calculation based on direction

## Board/GameBoard Class

- Has defined width and height dimensions
- Manages object placement and retrieval
- Tracks occupied positions
- Implements position wrapping for board edges (tunnel effect)
- Can check if a position is occupied
- Can retrieve an object at a specific position
- Can remove objects from positions
- Wraps object positions automatically when placing objects

## Wall Class

- Has a position
- Has a hit counter (health)
- Gets destroyed after 2 hits
- Can report its destroyed state

## Mine Class

- Has a position
- Can be destroyed
- Destroys tanks that move onto it
- Can report its destroyed state

## Algorithm/AI Classes

- Abstract base class with concrete implementations (Simple, Pathfinding, etc.)
- Makes decisions based on game state
- Prioritizes actions in this order:
  - Avoid immediate danger (incoming shells)
  - Shoot when enemy is in line of sight
  - Navigate toward enemy (pathfinding)
- Can detect direct line of sight (raycasting)
- Can anticipate shell trajectories
- Can find safe positions to evade shells
- Calculates optimal rotation to face enemy
- Does nothing when tank is destroyed

## GameState Class

- Snapshot of current game state (board, tanks, shells)
- Provides helper methods for algorithms:
  - Checking if tanks have line of sight to each other
  - Detecting approaching shells (considering ownership)
  - Finding nearby empty positions
  - Determining safe positions
  - Calculating line of sight in different directions

## GameManager Class

- Core game loop and logic
- Processes game steps
- Manages algorithms for both tanks
- Handles tank actions (movement, rotation, shooting)
- Creates and tracks shells
- Manages all collision detection and resolution:
  - Tank-mine collisions (both destroyed)
  - Tank-tank collisions (blocked movement, not destroyed)
  - Shell-wall collisions (wall damaged, destroyed after 2 hits)
  - Shell-tank collisions (tank destroyed)
  - Shell-shell collisions (both destroyed)
  - Shell-mine collisions (shell unaffected, mine unaffected)
- Manages path crossing collisions (objects swap positions)
- Has special position collisions (objects end up in same position)
- Detects game over conditions:
  - Tank destruction leads to opponent win
  - Both tanks destroyed leads to tie
  - Out of ammunition + 40 steps leads to tie
- Logs step history, including invalid moves
- Tracks cooldowns and ammunition

## InputParser Class

- Parses board layout from text file
- Sets up initial game state:
  - Board dimensions
  - Tank placement and direction (left for tank 1, right for tank 2)
  - Wall placement
  - Mine placement
- Handles various file format errors:
  - Multiple tanks for one player (uses first found)
  - Malformed dimensions
  - Mismatched dimensions (lines too short/long)
  - Unknown characters
- Records and reports errors

## Position and Direction Utilities

- Position class with x,y coordinates
- Equality and inequality operators
- 8 possible directions (UP, UP_RIGHT, RIGHT, etc.)
- Direction delta calculation for movement
- Direction rotation (1/8 or 1/4 turns, clockwise or counter-clockwise)
- Conversion between directions and deltas

## CollisionHandler Class

- Detects and resolves collisions
- Handles path crossing collisions
- Manages position-based collisions
- Tracks and applies explosions
- Handles shell-wall interactions
- Manages tank-mine collisions
- Reports if tanks were destroyed in collisions
- Special handling for board wrapping in collision detection

## Path Considerations

- Shell movement should respect walls (destroyed on contact)
- Calculate midpoint for path crossing collisions
- Handle wrapping at board edges
- No diagonal movement through corners (path must be clear)
- Ships move 2 positions per step
- Tanks move 1 position per step

## Timing and Sequencing

- Shells move twice per game step
- Cooldown after shooting lasts 4 steps
- Backward movement takes 3 steps to initiate
- After ammunition depleted, game continues 40 more steps
- Tank actions are processed in order
- Collisions are checked after each movement phase

## Error Handling

- Report but don't crash on input errors
- Skip multiple tanks of same player
- Handle missing/malformed dimensions
- Process short/long rows appropriately
- Track and report unknown characters
- Continue with warnings for recoverable errors
- Exit gracefully for unrecoverable errors