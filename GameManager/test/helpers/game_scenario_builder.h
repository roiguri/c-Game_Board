#pragma once

#include <vector>
#include <tuple>
#include <memory>

#include "utils/point.h"
#include "../mocks/scenario_mock_satellite_view.h"
#include "../mocks/enhanced_mock_player.h"

namespace GameManager_318835816_211314471 {

using namespace UserCommon_318835816_211314471;

/**
 * @brief Builder class for constructing game test scenarios with fluent interface
 */
class GameScenarioBuilder {
public:
    GameScenarioBuilder() 
        : m_width(5)
        , m_height(5)
        , m_maxSteps(100)
        , m_numShells(10) {}

    /**
     * @brief Set board dimensions
     */
    GameScenarioBuilder& withBoard(size_t width, size_t height) {
        m_width = width;
        m_height = height;
        return *this;
    }

    /**
     * @brief Set board dimensions (convenience method)
     */
    GameScenarioBuilder& withBoardSize(size_t width, size_t height) {
        return withBoard(width, height);
    }

    /**
     * @brief Add tanks at specified positions
     */
    GameScenarioBuilder& withTanks(const std::vector<Point>& positions) {
        m_tankPositions = positions;
        return *this;
    }

    /**
     * @brief Add tank at specific position
     */
    GameScenarioBuilder& withTank(const Point& position) {
        m_tankPositions.push_back(position);
        return *this;
    }

    /**
     * @brief Add walls at specified positions
     */
    GameScenarioBuilder& withWalls(const std::vector<Point>& positions) {
        m_wallPositions = positions;
        return *this;
    }

    /**
     * @brief Add wall at specific position
     */
    GameScenarioBuilder& withWall(const Point& position) {
        m_wallPositions.push_back(position);
        return *this;
    }

    /**
     * @brief Add mines at specified positions
     */
    GameScenarioBuilder& withMines(const std::vector<Point>& positions) {
        m_minePositions = positions;
        return *this;
    }

    /**
     * @brief Add mine at specific position
     */
    GameScenarioBuilder& withMine(const Point& position) {
        m_minePositions.push_back(position);
        return *this;
    }

    /**
     * @brief Add shells at specified positions
     */
    GameScenarioBuilder& withShells(const std::vector<Point>& positions) {
        m_shellPositions = positions;
        return *this;
    }

    /**
     * @brief Add shell at specific position
     */
    GameScenarioBuilder& withShell(const Point& position) {
        m_shellPositions.push_back(position);
        return *this;
    }

    /**
     * @brief Set maximum steps for the game
     */
    GameScenarioBuilder& withMaxSteps(size_t steps) {
        m_maxSteps = steps;
        return *this;
    }

    /**
     * @brief Set number of shells per tank
     */
    GameScenarioBuilder& withNumShells(size_t shells) {
        m_numShells = shells;
        return *this;
    }

    /**
     * @brief Set current tank position (marked with '%')
     */
    GameScenarioBuilder& withCurrentTank(const Point& position) {
        m_currentTankPosition = position;
        return *this;
    }

    /**
     * @brief Configure Player 1 behavior
     */
    GameScenarioBuilder& withPlayer1Behavior(std::function<void(TankAlgorithm&, SatelliteView&)> behavior) {
        m_player1Behavior = behavior;
        return *this;
    }

    /**
     * @brief Configure Player 2 behavior
     */
    GameScenarioBuilder& withPlayer2Behavior(std::function<void(TankAlgorithm&, SatelliteView&)> behavior) {
        m_player2Behavior = behavior;
        return *this;
    }

    /**
     * @brief Add two tanks for standard 2-player setup
     */
    GameScenarioBuilder& withTwoPlayerSetup(const Point& player1Pos, const Point& player2Pos) {
        m_tankPositions.clear(); // Clear any existing tanks
        m_tankPositions.push_back(player1Pos);
        m_tankPositions.push_back(player2Pos);
        return *this;
    }

    /**
     * @brief Add a horizontal wall line
     */
    GameScenarioBuilder& withHorizontalWall(int y, int startX, int endX) {
        for (int x = startX; x <= endX; ++x) {
            m_wallPositions.push_back(Point(x, y));
        }
        return *this;
    }

    /**
     * @brief Add a vertical wall line
     */
    GameScenarioBuilder& withVerticalWall(int x, int startY, int endY) {
        for (int y = startY; y <= endY; ++y) {
            m_wallPositions.push_back(Point(x, y));
        }
        return *this;
    }

    /**
     * @brief Add border walls around the entire board
     */
    GameScenarioBuilder& withBorderWalls() {
        // Top and bottom borders
        for (size_t x = 0; x < m_width; ++x) {
            m_wallPositions.push_back(Point(x, 0));
            m_wallPositions.push_back(Point(x, m_height - 1));
        }
        // Left and right borders
        for (size_t y = 1; y < m_height - 1; ++y) {
            m_wallPositions.push_back(Point(0, y));
            m_wallPositions.push_back(Point(m_width - 1, y));
        }
        return *this;
    }

    /**
     * @brief Clear all previously added objects
     */
    GameScenarioBuilder& clear() {
        m_tankPositions.clear();
        m_wallPositions.clear();
        m_minePositions.clear();
        m_shellPositions.clear();
        m_currentTankPosition = Point(-1, -1);
        return *this;
    }

    /**
     * @brief Build the scenario and return all components
     */
    std::tuple<MockSatelliteView, std::unique_ptr<ConfigurableMockPlayer>, std::unique_ptr<ConfigurableMockPlayer>, size_t, size_t, size_t, size_t> build() {
        // Create satellite view
        auto satelliteView = MockSatelliteView::createEmpty(m_width, m_height);

        // Add tanks (assign player IDs automatically, only support 2 players)
        int playerId = 1;
        for (const auto& pos : m_tankPositions) {
            satelliteView.addTank(static_cast<size_t>(pos.getX()), static_cast<size_t>(pos.getY()), playerId);
            playerId = (playerId % 2) + 1; // Cycle through player IDs 1-2
        }

        // Add walls
        for (const auto& pos : m_wallPositions) {
            satelliteView.addWall(static_cast<size_t>(pos.getX()), static_cast<size_t>(pos.getY()));
        }

        // Add mines
        for (const auto& pos : m_minePositions) {
            satelliteView.addMine(static_cast<size_t>(pos.getX()), static_cast<size_t>(pos.getY()));
        }

        // Add shells
        for (const auto& pos : m_shellPositions) {
            satelliteView.addShell(static_cast<size_t>(pos.getX()), static_cast<size_t>(pos.getY()));
        }

        // Add current tank if specified
        if (m_currentTankPosition.getX() != -1 && m_currentTankPosition.getY() != -1) {
            satelliteView.addCurrentTank(static_cast<size_t>(m_currentTankPosition.getX()), 
                                       static_cast<size_t>(m_currentTankPosition.getY()));
        }

        // Create mock players
        auto player1 = std::make_unique<ConfigurableMockPlayer>();
        auto player2 = std::make_unique<ConfigurableMockPlayer>();

        // Configure player behaviors
        if (m_player1Behavior) {
            player1->setUpdateBehavior(m_player1Behavior);
        } else {
            player1->setDefaultBehavior();
        }

        if (m_player2Behavior) {
            player2->setUpdateBehavior(m_player2Behavior);
        } else {
            player2->setDefaultBehavior();
        }

        return std::make_tuple(
            std::move(satelliteView),
            std::move(player1),
            std::move(player2),
            m_width,
            m_height,
            m_maxSteps,
            m_numShells
        );
    }

    /**
     * @brief Quick build for simple scenarios (returns only satellite view)
     */
    MockSatelliteView buildSatelliteView() {
        auto [satelliteView, player1, player2, width, height, maxSteps, numShells] = build();
        return std::move(satelliteView);
    }



private:
    size_t m_width;
    size_t m_height;
    size_t m_maxSteps;
    size_t m_numShells;

    std::vector<Point> m_tankPositions;
    std::vector<Point> m_wallPositions;
    std::vector<Point> m_minePositions;
    std::vector<Point> m_shellPositions;
    Point m_currentTankPosition{-1, -1}; // Invalid position initially

    std::function<void(TankAlgorithm&, SatelliteView&)> m_player1Behavior;
    std::function<void(TankAlgorithm&, SatelliteView&)> m_player2Behavior;
};

} // namespace GameManager_318835816_211314471 