#pragma once

#include <map>
#include <string>
#include <vector>

#include "utils/point.h"

namespace UserCommon_318835816_211314471 {

/**
 * @class GameBoard
 * @brief Represents the game board for the tank battle simulation
 * 
 * Manages the board state including walls, mines, and provides utilities
 * for position wrapping, movement validation, and wall health tracking.
 */
class GameBoard {
public:
    /**
     * @brief Constructs an empty game board with zero dimensions
     */
    GameBoard();

    /**
     * @brief Constructs a game board with specified dimensions
     * 
     * Initializes a board of the given size with all cells set to empty.
     * 
     * @param width The width of the board
     * @param height The height of the board
     */
    GameBoard(size_t width, size_t height);
    
    /**
     * @brief Types of cells that can exist on the game board
     */
    enum class CellType {
        Empty,
        Wall,
        Mine
    };

    /**
     * @brief Constant for wall starting health
     */
    static constexpr int WALL_STARTING_HEALTH = 2;
    
    /**
     * @brief Initializes the board from a vector of strings
     * 
     * Parses each line character by character to build the game board,
     * identifies tank positions, and validates board constraints.
     * 
     * @param boardLines Vector of strings representing board rows
     * @param tankPositions Reference to store detected tank positions
     * @return true if initialization succeeded with valid game state, 
     *  false otherwise
     */
    bool initialize(const std::vector<std::string>& boardLines, 
      std::vector<std::pair<int, Point>>& tankPositions);
    
    /**
     * @brief Checks if the cell at the specified position is a wall
     * 
     * @param position The position to check
     * @return true if the cell is a wall, false otherwise
     */
    bool isWall(const Point& position) const;

    /**
     * @brief Checks if the cell at the specified position is a mine
     * 
     * @param position The position to check
     * @return true if the cell is a mine, false otherwise
     */
    bool isMine(const Point& position) const;

    /**
     * @brief Checks if the cell at the specified position is a mine
     * 
     * @param position The position to check
     * @return true if the cell is a mine, false otherwise
     */
    bool damageWall(const Point& position);

    /**
     * @brief Gets the health of a wall at the specified position
     * 
     * @param position The position to check
     * @return The remaining health of the wall, or 0 if there is no wall
     */
    int getWallHealth(const Point& position) const;
    
    /**
     * @brief Wraps a position to ensure it falls within the board boundaries
     * 
     * Handles positions that are outside the board by wrapping them around,
     * where moving off one edge brings you to the opposite edge.
     * Only support positions that can be reached in the game flow.
     * 
     * @param position Position to wrap
     * @return Position wrapped to be within board boundaries
     */
    Point wrapPosition(const Point& position) const;

    /**
     * @brief Checks if a tank can move to the specified position
     * 
     * A position is movable if it is not a wall. Mines and empty spaces
     * are considered movable (though mines will trigger explosions).
     * 
     * @param position The position to check
     * @return true if a tank can move to the position, false otherwise
     */
    bool canMoveTo(const Point& position) const;
    
    /**
     * @brief Converts the game board to a string representation
     * 
     * Creates a text visualization of the board using characters:
     * ' ' for empty spaces, '#' for walls, '@' for mines.
     * 
     * @return A string representing the current board state
     */
    std::string toString() const;

    /**
     * @brief Gets the type of cell at the specified coordinates
     * 
     * Coordinates are automatically wrapped to board dimensions.
     * 
     * @param x The x-coordinate
     * @param y The y-coordinate
     * @return The type of cell at the specified position
     */
    CellType getCellType(int x, int y) const;

    /**
     * @brief Gets the type of cell at the specified position
     * 
     * Coordinates are automatically wrapped to board dimensions.
     * 
     * @param position The position to check
     * @return The type of cell at the specified position
     */
    CellType getCellType(const Point& position) const;

    /**
     * @brief Sets the type of cell at the specified coordinates
     * 
     * Coordinates are automatically wrapped to board dimensions.
     * If setting a wall, its health is initialized to maximum.
     * 
     * @param x The x-coordinate
     * @param y The y-coordinate
     * @param type The cell type to set
     */
    void setCellType(int x, int y, CellType type);

    /**
     * @brief Sets the type of cell at the specified position
     * 
     * Coordinates are automatically wrapped to board dimensions.
     * If setting a wall, its health is initialized to maximum.
     * 
     * @param position The position to modify
     * @param type The cell type to set
     */
    void setCellType(const Point& position, CellType type);

    /**
     * @brief Gets the width of the game board
     * 
     * @return The width in number of cells
     */
    size_t getWidth() const;

    /**
     * @brief Gets the height of the game board
     * 
     * @return The height in number of cells
     */
    size_t getHeight() const;

    /**
     * @brief Calculates the minimum number of steps between two points, considering wrapping.
     *        Diagonal and orthogonal moves both cost 1 step (no rotations).
     *
     * @param a The starting point
     * @param b The destination point
     * @param width The width of the board
     * @param height The height of the board
     * @return The minimum number of steps required
     */
    static int stepDistance(const Point& a, const Point& b, size_t width, size_t height);
private:
    std::vector<std::vector<CellType>> m_board;
    std::map<Point, int> m_wallHealth;
    size_t m_width;
    size_t m_height;
};

} // namespace UserCommon_318835816_211314471
