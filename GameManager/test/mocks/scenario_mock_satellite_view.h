#pragma once

#include <vector>
#include <string>
#include <algorithm>

#include "common/SatelliteView.h"
#include "utils/point.h"

namespace GameManager_098765432_123456789 {

using namespace UserCommon_098765432_123456789;

/**
 * @brief Mock satellite view with dynamic board creation and modification capabilities
 */
class MockSatelliteView : public SatelliteView {
public:
    /**
     * @brief Create empty board with specified dimensions
     */
    static MockSatelliteView createEmpty(size_t width, size_t height) {
        std::vector<std::string> grid(height, std::string(width, ' '));
        return MockSatelliteView(grid, width, height);
    }

    /**
     * @brief Constructor with grid data
     */
    MockSatelliteView(const std::vector<std::string>& grid, size_t width, size_t height)
        : m_grid(grid), m_width(width), m_height(height) {
        // Ensure grid has correct dimensions
        m_grid.resize(height);
        for (auto& row : m_grid) {
            row.resize(width, ' ');
        }
    }

    /**
     * @brief Get object at specified position
     */
    char getObjectAt(size_t x, size_t y) const override {
        if (y >= m_height || x >= m_width) {
            return '&'; // Out of bounds
        }
        if (y >= m_grid.size() || x >= m_grid[y].length()) {
            return ' '; // Within bounds but no data
        }
        return m_grid[y][x];
    }

    /**
     * @brief Dynamically set object at position
     */
    void setObjectAt(size_t x, size_t y, char object) {
        if (y < m_height && x < m_width) {
            if (y >= m_grid.size()) {
                m_grid.resize(y + 1);
            }
            if (x >= m_grid[y].length()) {
                m_grid[y].resize(x + 1, ' ');
            }
            m_grid[y][x] = object;
        }
    }

    /**
     * @brief Clear all objects from the board
     */
    void clearBoard() {
        for (auto& row : m_grid) {
            std::fill(row.begin(), row.end(), ' ');
        }
    }

    /**
     * @brief Add tank at specified position
     */
    void addTank(size_t x, size_t y, int playerId) {
        if (playerId >= 1 && playerId <= 2) {
            setObjectAt(x, y, '0' + playerId);
        }
    }

    /**
     * @brief Add current tank marker at position
     */
    void addCurrentTank(size_t x, size_t y) {
        setObjectAt(x, y, '%');
    }

    /**
     * @brief Add wall at specified position
     */
    void addWall(size_t x, size_t y) {
        setObjectAt(x, y, '#');
    }

    /**
     * @brief Add mine at specified position
     */
    void addMine(size_t x, size_t y) {
        setObjectAt(x, y, '@');
    }

    /**
     * @brief Add shell at specified position
     */
    void addShell(size_t x, size_t y) {
        setObjectAt(x, y, '*');
    }

    /**
     * @brief Get board dimensions
     */
    size_t getWidth() const { return m_width; }
    size_t getHeight() const { return m_height; }

    /**
     * @brief Get the entire grid for debugging
     */
    const std::vector<std::string>& getGrid() const { return m_grid; }

    /**
     * @brief Print board to string for debugging
     */
    std::string toString() const {
        std::string result;
        for (const auto& row : m_grid) {
            result += row + "\n";
        }
        return result;
    }

    /**
     * @brief Find all positions of a specific character
     */
    std::vector<Point> findPositions(char character) const {
        std::vector<Point> positions;
        for (size_t y = 0; y < m_grid.size(); ++y) {
            for (size_t x = 0; x < m_grid[y].length() && x < m_width; ++x) {
                if (m_grid[y][x] == character) {
                    positions.emplace_back(static_cast<int>(x), static_cast<int>(y));
                }
            }
        }
        return positions;
    }

    /**
     * @brief Count occurrences of a specific character
     */
    int countCharacter(char character) const {
        int count = 0;
        for (const auto& row : m_grid) {
            count += std::count(row.begin(), row.end(), character);
        }
        return count;
    }

private:
    std::vector<std::string> m_grid;
    size_t m_width;
    size_t m_height;
};

} // namespace GameManager_098765432_123456789 