# Project Review Guidelines

## General Guidelines

### Follow Assignment Requirements
- Verify all specifications are implemented correctly
- Check that game mechanics match the requirements (tank movement, shooting, collision handling)
- Validate error handling for input files

### Code Organization
- Ensure clear separation of concerns between classes
- Maintain consistent file structure (header in include/, implementation in src/)
- Group related functionality together

## Documentation Guidelines
- Verify all classes and public methods have documentation in header files only according to these guidlines:

### Public Methods

Public methods should have comprehensive documentation that explains their purpose, parameters, and return values.

```cpp
/**
 * @brief Calculates the next action based on current game state
 * 
 * Evaluates the current board state, tank positions, and shell trajectories
 * to determine the optimal action for the tank to take.
 * 
 * @param gameBoard The current game board state
 * @param myTank The tank controlled by this algorithm
 * @param enemyTank The opponent's tank
 * @param shells All active shells on the board
 * @return The action to perform
 */
```

#### Requirements for Public Methods:
- Begin with a clear description of the method's purpose
- Describe what the method does, not how it works
- Include information about parameters and return values
- Note any preconditions or postconditions when relevant
- Use complete sentences with proper grammar

### Private Methods

Private methods should have concise documentation that summarizes their purpose.

```cpp
// Updates the path to the target if needed
void updatePathToTarget(const GameBoard& gameBoard, const Point& start, const Point& target);

// Finds shortest path from start to target using BFS
std::vector<Point> findPathBFS(const GameBoard& gameBoard, const Point& start, const Point& target) const;
```

#### Requirements for Private Methods:
- Use a single line summary of the method's purpose
- Only add additional details if absolutely necessary for understanding
- Focus on "what" not "how"

### Class Documentation

Each class should be documented at the top of its header file.

```cpp
/**
 * @class ChaseAlgorithm
 * @brief Algorithm that actively pursues the enemy tank
 * 
 * Implements a pathfinding approach that calculates and follows the shortest
 * path to the enemy tank while avoiding obstacles and dangerous situations.
 */
```

#### Requirements for Class Documentation:
- Place at the top of header files
- Describe the purpose and responsibilities of the class
- Mention any important design patterns or relationships

### Member Variables

Member variables shouldn't have documentation'.

## General Guidelines

- Be consistent in style and format
- Use @brief, @param, @return tags in public method documentation
- Update documentation when changing method behavior
- Document code that may not be immediately obvious to other developers
- Focus on explaining "why" rather than "what" when the code is complex

## C++ Specific Guidelines

### Naming Conventions
- Use consistent naming for variables, methods, and classes
- Follow camelCase for methods/variables, PascalCase for classes
- Prefix member variables with m_

### Memory Management
- Check for potential memory leaks, especially in GameManager
- Ensure proper cleanup in destructors
- Use references/pointers correctly; avoid creating new instances when unnecessary
- Ensure the Rule of Three is followed: if one of copy constructor, destructor, or  assignment operator is defined, all three should be implemented (if a destructor is needed, prefer deleting copy constructor and assignment operator)
- prefer removing code for empty constructor/destructor (use default)

### Const Correctness
- Declare variables as const when they don't change
- Declare methods as const when they don't modify object state
- Use const references for parameters that shouldn't be modified


### Modern C++ Features
- Always use the override keyword for virtual function implementations
- Use constants (e.g., static constexpr) for unchanged values throughout the game

### Implementation Guidelines
- Don't implement methods of over 3 lines in header files
- Avoid comments on clear, self-documenting code
- Keep functions focused with a single responsibility

## Design Principles

### Class Design
- Review class relationships and dependencies
- Ensure classes have a single responsibility
- Check for appropriate encapsulation of implementation details
- Review repeated use of arguments that can be stored as class members.

## Consistency Checks

### Code Style
- Consistent indentation and formatting
- Consistent use of braces, spacing, and line breaks
- Consistent parameter ordering in similar methods

### Method Signatures
- Check for consistent parameter naming across related methods
- Verify consistent return types for similar operations
- Ensure consistent const correctness