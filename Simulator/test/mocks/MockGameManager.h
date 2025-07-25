#pragma once

#include "common/AbstractGameManager.h"

// Mock GameManager for testing
class MockGameManager : public AbstractGameManager {
public:
    MockGameManager(bool verbose) : verbose_(verbose) {}
    
    GameResult run(size_t, size_t, const SatelliteView&,
                  string,
                  size_t, size_t,
                  Player&, string,
                  Player&, string,
                  TankAlgorithmFactory,
                  TankAlgorithmFactory) override {
        return GameResult{}; // Return dummy result
    }
    
    bool isVerbose() const { return verbose_; }
    
private:
    bool verbose_;
};