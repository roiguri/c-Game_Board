#pragma once

#include <string>
#include "common/GameResult.h"
#include "utils/file_loader.h"

class GameRunner {
public:
    static GameResult runSingleGame(
        const FileLoader::BoardInfo& boardInfo,
        const std::string& gameManagerName,
        const std::string& algorithm1Name,
        const std::string& algorithm2Name,
        bool verbose = false
    );

private:
    GameRunner() = delete;
    GameRunner(const GameRunner&) = delete;
    GameRunner& operator=(const GameRunner&) = delete;
};