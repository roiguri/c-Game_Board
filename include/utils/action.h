#pragma once

#include <string>
#include <ostream>

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
std::string actionToString(Action action);

/**
 * Overloads the << operator to print the Action enum value.
 * 
 * @param os The output stream
 * @param action The action to print
 * @return The output stream
 */
std::ostream& operator<<(std::ostream& os, Action action);