#pragma once

#include <vector>
#include <string>
#include <stdexcept>
#include "common/Player.h"
#include "common/TankAlgorithm.h"

class BadRegistrationException : public std::runtime_error {
public:
    explicit BadRegistrationException(const std::string& message)
        : std::runtime_error("Bad registration: " + message) {}
};

class AlgorithmRegistrar {
private:
    class AlgorithmAndPlayerFactories {
    private:
        std::string so_name;
        TankAlgorithmFactory tankAlgorithmFactory;
        PlayerFactory playerFactory;
        bool hasTankAlgorithmFactory = false;
        bool hasPlayerFactory = false;

    public:
        explicit AlgorithmAndPlayerFactories(const std::string& name) : so_name(name) {}

        void setTankAlgorithmFactory(TankAlgorithmFactory&& factory) {
            tankAlgorithmFactory = std::move(factory);
            hasTankAlgorithmFactory = true;
        }

        void setPlayerFactory(PlayerFactory&& factory) {
            playerFactory = std::move(factory);
            hasPlayerFactory = true;
        }

        const std::string& getName() const { return so_name; }
        const TankAlgorithmFactory& getTankAlgorithmFactory() const { return tankAlgorithmFactory; }
        const PlayerFactory& getPlayerFactory() const { return playerFactory; }

        bool isComplete() const {
            return hasTankAlgorithmFactory && hasPlayerFactory;
        }

        bool isEmpty() const {
            return !hasTankAlgorithmFactory && !hasPlayerFactory;
        }
    };

    std::vector<AlgorithmAndPlayerFactories> algorithms;
    static AlgorithmRegistrar registrar;

public:
    ~AlgorithmRegistrar();
    
    static AlgorithmRegistrar& getAlgorithmRegistrar();
    
    void createAlgorithmFactoryEntry(const std::string& name);
    void addPlayerFactoryToLastEntry(PlayerFactory&& factory);
    void addTankAlgorithmFactoryToLastEntry(TankAlgorithmFactory&& factory);
    void validateLastRegistration();
    void removeLast();

    // Iterator support for range-based loops
    using iterator = std::vector<AlgorithmAndPlayerFactories>::const_iterator;
    iterator begin() const { return algorithms.begin(); }
    iterator end() const { return algorithms.end(); }
    
    size_t size() const { return algorithms.size(); }
    bool empty() const { return algorithms.empty(); }

    // For testing purposes
    void clear();
};