#pragma once

#include <string>
#include "base_game_mode.h"
#include "game_runner.h"
#include "utils/file_loader.h"
#include "common/GameResult.h"

class BasicGameMode : public BaseGameMode {
public:
    BasicGameMode();
    ~BasicGameMode();

    struct GameParameters : public BaseParameters {
        std::string gameManagerLib;
        std::string algorithm1Lib;
        std::string algorithm2Lib;
        
        GameParameters() : BaseParameters() {}
    };

    /**
     * Execute a basic game mode with the given parameters
     * Uses the base class template method for execution flow
     * @param params Game parameters for basic mode
     * @return GameResult containing game outcome
     */
    GameResult execute(const GameParameters& params) {
        return BaseGameMode::execute(params);
    }

protected:
    // Override abstract methods from BaseGameMode
    bool loadLibraries(const BaseParameters& params) override;
    bool loadMap(const std::string& mapFile) override;
    GameResult executeGameLogic(const BaseParameters& params) override;
    void displayResults(const GameResult& result) override;
    
    // Override cleanup to handle BasicGameMode-specific cleanup
    void cleanup() override;

private:
    // Helper methods specific to BasicGameMode
    bool loadLibrariesImpl(const GameParameters& params);
    
    // BasicGameMode-specific data members
    FileLoader::BoardInfo m_boardInfo;
    std::string m_gameManagerName;
    std::string m_algorithm1Name;
    std::string m_algorithm2Name;
};