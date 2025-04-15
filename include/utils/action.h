#pragma once

#include <string>

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