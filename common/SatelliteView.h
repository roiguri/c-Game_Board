#pragma once

#include <cstddef>

/**
 * @class SatelliteView
 * @brief Interface for satellite view of the game board
 * 
 * Provides a read-only view of the game board from a satellite perspective.
 * Algorithms can use this to observe the current state of the battlefield
 * without direct access to the actual game state objects.
 */
class SatelliteView {  
public:  
    /**
     * @brief Virtual destructor
     */
    virtual ~SatelliteView() {}  

    /**
     * @brief Gets the object at a specific board position
     * 
     * @param x The x-coordinate (column) to check
     * @param y The y-coordinate (row) to check
     * @return char Character representing the object at specified position
     */
    virtual char getObjectAt(size_t x, size_t y) const = 0;  
};