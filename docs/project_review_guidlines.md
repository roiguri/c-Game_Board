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

### Documentation
- Verify all classes and public methods have meaningful documentation
- Ensure the High Level Design (HLD) document accurately reflects implementation
- Use a consistent documentation style for header files

## C++ Specific Guidelines

### Naming Conventions
- Use consistent naming for variables, methods, and classes
- Follow camelCase for methods/variables, PascalCase for classes
- Prefix member variables with m_ (as seen in your code)

### Memory Management
- Check for potential memory leaks, especially in GameManager
- Ensure proper cleanup in destructors
- Use references/pointers correctly; avoid creating new instances when unnecessary
- Ensure the Rule of Three is followed: if one of copy constructor, destructor, or copy assignment operator is defined, all three should be implemented

### Const Correctness
- Declare variables as const when they don't change
- Declare methods as const when they don't modify object state
- Use const references for parameters that shouldn't be modified

### Error Handling
- Verify robust error handling for recoverable errors
- Ensure unrecoverable errors are appropriately logged
- Check that the program never crashes, as specified

### Modern C++ Features
- Follow C++17 standards consistently
- Use appropriate containers and algorithms
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

### Algorithm Implementation
- Verify both required algorithms are correctly implemented
- Check that algorithms don't modify game state directly
- Validate algorithm behavior for edge cases
- Verify that similar functionalities are implemented in base class

### Testing Coverage
- Ensure comprehensive test coverage for all classes
- Verify edge cases are covered in tests
- Check that tests validate both normal operation and error handling

## Consistency Checks

### Code Style
- Consistent indentation and formatting
- Consistent use of braces, spacing, and line breaks
- Consistent parameter ordering in similar methods

### Method Signatures
- Check for consistent parameter naming across related methods
- Verify consistent return types for similar operations
- Ensure consistent const correctness

### Documentation Style
- Verify consistent format for class and method documentation
- Check consistent style for TODO comments
- Ensure consistent level of detail in explanations