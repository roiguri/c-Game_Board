#include "action.h"

std::string actionToString(Action action) {
    switch (action) {
        case Action::MoveForward:
            return "Move Forward";
        case Action::MoveBackward:
            return "Move Backward";
        case Action::RotateLeftEighth:
            return "Rotate Left 1/8";
        case Action::RotateRightEighth:
            return "Rotate Right 1/8";
        case Action::RotateLeftQuarter:
            return "Rotate Left 1/4";
        case Action::RotateRightQuarter:
            return "Rotate Right 1/4";
        case Action::Shoot:
            return "Shoot";
        case Action::None:
            return "None";
        default:
            return "Unknown Action";
    }
}