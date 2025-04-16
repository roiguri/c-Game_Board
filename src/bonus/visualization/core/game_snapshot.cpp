#include "bonus/visualization/core/game_snapshot.h"
#include <sstream>
#include <iomanip>

TankState::TankState(const Tank& tank) 
    : playerId(tank.getPlayerId()),
      position(tank.getPosition()),
      direction(tank.getDirection()),
      remainingShells(tank.getRemainingShells()),
      destroyed(tank.isDestroyed()) {
}

ShellState::ShellState(const Shell& shell)
    : playerId(shell.getPlayerId()),
      position(shell.getPosition()),
      direction(shell.getDirection()),
      destroyed(shell.isDestroyed()) {
}

GameSnapshot::GameSnapshot(
    int step,
    const GameBoard& board,
    const std::vector<Tank>& tanks,
    const std::vector<Shell>& shells,
    int countdown,
    const std::string& message
) : m_stepNumber(step), m_countdown(countdown), m_message(message) {
    // Copy board state
    int width = board.getWidth();
    int height = board.getHeight();
    m_boardState.resize(height);
    
    for (int y = 0; y < height; ++y) {
        m_boardState[y].resize(width);
        for (int x = 0; x < width; ++x) {
            m_boardState[y][x] = board.getCellType(x, y);

            if (board.getCellType(x, y) == GameBoard::CellType::Wall) {
              Point pos(x, y);
              m_wallHealth[pos] = board.getWallHealth(pos);
            }
        }
    }
    
    // Copy tank states
    m_tanks.reserve(tanks.size());
    for (const auto& tank : tanks) {
        m_tanks.emplace_back(tank);
    }
    
    // Copy shell states
    m_shells.reserve(shells.size());
    for (const auto& shell : shells) {
        m_shells.emplace_back(shell);
    }
}

std::string GameSnapshot::toJson() const {
    std::ostringstream json;
    
    // Start the JSON object
    json << "{" << std::endl;
    
    // Step number
    json << "  \"step\": " << m_stepNumber << "," << std::endl;
    
    // Message
    json << "  \"message\": \"" << m_message << "\"," << std::endl;
    
    // Board dimensions
    int height = m_boardState.size();
    int width = height > 0 ? m_boardState[0].size() : 0;
    
    json << "  \"board\": {" << std::endl;
    json << "    \"width\": " << width << "," << std::endl;
    json << "    \"height\": " << height << "," << std::endl;
    
    // Board cells
    json << "    \"cells\": [" << std::endl;
    for (int y = 0; y < height; ++y) {
        json << "      [";
        for (int x = 0; x < width; ++x) {
            json << static_cast<int>(m_boardState[y][x]);
            if (x < width - 1) {
                json << ", ";
            }
        }
        json << "]";
        if (y < height - 1) {
            json << ",";
        }
        json << std::endl;
    }
    json << "    ]" << std::endl;
    json << "  }," << std::endl;
    
    // Tanks
    json << "  \"tanks\": [" << std::endl;
    for (size_t i = 0; i < m_tanks.size(); ++i) {
        const auto& tank = m_tanks[i];
        json << "    {" << std::endl;
        json << "      \"playerId\": " << tank.playerId << "," << std::endl;
        json << "      \"position\": {" << std::endl;
        json << "        \"x\": " << tank.position.x << "," << std::endl;
        json << "        \"y\": " << tank.position.y << std::endl;
        json << "      }," << std::endl;
        json << "      \"direction\": " << static_cast<int>(tank.direction) << "," << std::endl;
        json << "      \"remainingShells\": " << tank.remainingShells << "," << std::endl;
        json << "      \"destroyed\": " << (tank.destroyed ? "true" : "false") << std::endl;
        json << "    }";
        if (i < m_tanks.size() - 1) {
            json << ",";
        }
        json << std::endl;
    }
    json << "  ]," << std::endl;
    
    // Shells
    json << "  \"shells\": [" << std::endl;
    for (size_t i = 0; i < m_shells.size(); ++i) {
        const auto& shell = m_shells[i];
        json << "    {" << std::endl;
        json << "      \"playerId\": " << shell.playerId << "," << std::endl;
        json << "      \"position\": {" << std::endl;
        json << "        \"x\": " << shell.position.x << "," << std::endl;
        json << "        \"y\": " << shell.position.y << std::endl;
        json << "      }," << std::endl;
        json << "      \"direction\": " << static_cast<int>(shell.direction) << "," << std::endl;
        json << "      \"destroyed\": " << (shell.destroyed ? "true" : "false") << std::endl;
        json << "    }";
        if (i < m_shells.size() - 1) {
            json << ",";
        }
        json << std::endl;
    }
    json << "  ]" << std::endl;
    
    // Close the JSON object
    json << "}" << std::endl;
    
    return json.str();
}

GameSnapshot GameSnapshot::fromJson(const std::string& json) {
    // NOTE: In a real implementation, you would use a proper JSON parser
    // library like nlohmann/json, rapidjson, or simdjson.
    // For this educational example, we're skipping the actual parsing.
    
    // This is a placeholder implementation
    GameSnapshot snapshot;
    
    // In a real implementation, you would:
    // 1. Parse the JSON string into a JSON object
    // 2. Extract step number, message, board state, tanks and shells
    // 3. Construct and return a valid GameSnapshot
    
    return snapshot;
}