#pragma once

#include <string>
#include <ostream>

/**
 * @class Action
 * @brief Represents possible actions a tank can perform
 * 
 * Defines all available actions in the game that can be commanded
 * to tanks, including movement, rotation, and shooting.
 */
enum class Action {
    MoveForward,
    MoveBackward,
    RotateLeftEighth,
    RotateRightEighth,
    RotateLeftQuarter,
    RotateRightQuarter,
    Shoot,
    None
};

/**
 * Converts an Action enum value to its string representation.
 * 
 * @param action The action to convert
 * @return A string representation of the action
 */
std::string actionToString(const Action action);

/**
 * Overloads the << operator to print the Action enum value.
 * 
 * @param os The output stream
 * @param action The action to print
 * @return The output stream
 */
std::ostream& operator<<(std::ostream& os, const Action action);