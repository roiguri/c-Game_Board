#pragma once
#include <vector>
#include "game_board.h"
#include "tank.h"
#include "shell.h"

/**
 * @brief Class that handles collision detection and resolution in the game
 * 
 * This class provides static methods to detect and handle various collision types:
 * - Shell collisions (with walls, other shells, and tanks)
 * - Tank collisions (with other tanks)
 * - Tank-mine collisions
 */
class CollisionHandler {
public:
    /**
     * @brief Check and handle all shell-related collisions
     * 
     * Checks for shell collisions with walls, other shells, and tanks.
     * Updates the game state accordingly (destroys shells/tanks, damages walls).
     * 
     * @param board The game board
     * @param shells Vector of shells in the game
     * @param tanks Vector of tanks in the game
     * @return true if any tank was destroyed during collision handling
     */
    static bool checkShellCollisions(GameBoard& board,
                                   std::vector<Shell>& shells,
                                   std::vector<Tank>& tanks);
      
    /**
     * @brief Check and handle tank-tank collisions
     * 
     * If two tanks occupy the same position, both are destroyed.
     * 
     * @param tanks Vector of tanks in the game
     * @return true if any tank was destroyed during collision handling
     */
    static bool checkTankCollisions(std::vector<Tank>& tanks);
      
    /**
     * @brief Check and handle tank-mine collisions
     * 
     * If a tank moves onto a mine, both the tank and mine are destroyed.
     * 
     * @param board The game board
     * @param tanks Vector of tanks in the game
     * @return true if any tank was destroyed during collision handling
     */
    static bool checkTankMineCollisions(GameBoard& board,
                                      std::vector<Tank>& tanks);
    
    /**
     * @brief Check and handle shell-wall collision
     * 
     * If a shell hits a wall, the wall is damaged and the shell is destroyed.
     * 
     * @param board The game board
     * @param shell The shell to check
     * @return true if the shell hit a wall, false otherwise
     */
    static bool checkShellWallCollision(GameBoard& board, Shell& shell);
    
    /**
     * @brief Check and handle shell-shell collision
     * 
     * If two shells collide, both are destroyed.
     * 
     * @param shell The shell to check
     * @param otherShells Vector of other shells to check against
     * @return true if the shell collided with another shell, false otherwise
     */
    static bool checkShellShellCollision(Shell& shell, std::vector<Shell>& otherShells);
    
    /**
     * @brief Check and handle shell-tank collision
     * 
     * If a shell hits a tank, both the shell and tank are destroyed.
     * 
     * @param shell The shell to check
     * @param tanks Vector of tanks to check against
     * @return true if the shell hit a tank and the tank was destroyed, false otherwise
     */
    static bool checkShellTankCollision(Shell& shell, std::vector<Tank>& tanks);
};