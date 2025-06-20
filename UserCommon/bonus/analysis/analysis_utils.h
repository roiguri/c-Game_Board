#pragma once

#include <string>
#include "bonus/board_generator.h" // For BoardConfig
#include "bonus/analysis/analysis_tool.h" // For Winner enum

// Stateless helper functions for analysis
Winner ParseGameResult(const std::string& resultLine);
std::string GenerateKey(const BoardConfig& config); 