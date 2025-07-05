#include "AlgorithmRegistrar.h"
#include <iostream>

AlgorithmRegistrar AlgorithmRegistrar::registrar;

AlgorithmRegistrar::~AlgorithmRegistrar() {
    clear();
}

AlgorithmRegistrar& AlgorithmRegistrar::getAlgorithmRegistrar() {
    return registrar;
}

void AlgorithmRegistrar::createAlgorithmFactoryEntry(const std::string& name) {
    algorithms.emplace_back(name);
}

void AlgorithmRegistrar::addPlayerFactoryToLastEntry(PlayerFactory&& factory) {
    if (algorithms.empty()) {
        throw BadRegistrationException("No algorithm entry to add player factory to");
    }
    algorithms.back().setPlayerFactory(std::move(factory));
}

void AlgorithmRegistrar::addTankAlgorithmFactoryToLastEntry(TankAlgorithmFactory&& factory) {
    if (algorithms.empty()) {
        throw BadRegistrationException("No algorithm entry to add tank algorithm factory to");
    }
    algorithms.back().setTankAlgorithmFactory(std::move(factory));
}

void AlgorithmRegistrar::validateLastRegistration() {
    if (algorithms.empty()) {
        throw BadRegistrationException("No algorithm entry to validate");
    }
    
    if (!algorithms.back().isComplete()) {
        std::string message = "Incomplete registration for algorithm '" + algorithms.back().getName() + "'";
        removeLast();
        throw BadRegistrationException(message);
    }
}

void AlgorithmRegistrar::removeLast() {
    if (!algorithms.empty()) {
        algorithms.pop_back();
    }
}

void AlgorithmRegistrar::clear() {
    algorithms.clear();
}