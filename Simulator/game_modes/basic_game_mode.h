#pragma once

#include <string>
#include "game_runner.h"
#include "utils/file_loader.h"

class BasicGameMode {
public:
    BasicGameMode();
    ~BasicGameMode();

    struct GameParameters {
        std::string mapFile;
        std::string gameManagerLib;
        std::string algorithm1Lib;
        std::string algorithm2Lib;
        bool verbose = true;
    };

    /**
     * Execute a basic game mode with the given parameters
     * @param params Game parameters for basic mode
     * @return GameResult containing game outcome
     */
    GameResult execute(const GameParameters& params);

private:
    bool loadLibraries(const GameParameters& params);
    bool loadMap(const std::string& mapFile);
    void cleanup();
    void displayResults(const GameResult& result);

    FileLoader::BoardInfo m_boardInfo;
    std::string m_gameManagerName;
    std::string m_algorithm1Name;
    std::string m_algorithm2Name;
};