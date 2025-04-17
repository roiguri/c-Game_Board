#include "algo/algorithm.h"

// TODO: not implemented
Algorithm::Algorithm() {
}

Algorithm::~Algorithm() {
}

Algorithm* Algorithm::createAlgorithm(const std::string& type) {
    if (type == "chase" || type == "defensive") {
        return nullptr;
    }
    
    return nullptr;
}