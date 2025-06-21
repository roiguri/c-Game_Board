#pragma once

#include <string>

#include "UserCommon/bonus/analysis/analysis_tool.h" // For Winner enum
#include "UserCommon/bonus/board_generator.h" // For BoardConfig

// Stateless helper functions for analysis
Winner ParseGameResult(const std::string& resultLine);
std::string GenerateKey(const BoardConfig& config); 