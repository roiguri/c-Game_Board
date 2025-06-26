#pragma once

#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <sstream>
#include <map>

#include "common/GameResult.h"
#include "utils/point.h"

namespace GameManager_098765432_123456789 {

using namespace UserCommon_098765432_123456789;

/**
 * @brief Helper class for validating GameResult structures in tests
 */
class GameResultValidator {
public:
    /**
     * @brief Validate that the winner field matches expected value
     */
    static void validateWinner(const GameResult& result, int expectedWinner) {
        EXPECT_EQ(result.winner, expectedWinner) 
            << "Expected winner " << expectedWinner 
            << " but got " << result.winner;
    }

    /**
     * @brief Validate that the reason field matches expected value
     */
    static void validateReason(const GameResult& result, GameResult::Reason expectedReason) {
        EXPECT_EQ(result.reason, expectedReason)
            << "Expected reason " << reasonToString(expectedReason)
            << " but got " << reasonToString(result.reason);
    }

    /**
     * @brief Validate that the remaining_tanks vector matches expected values
     */
    static void validateRemainingTanks(const GameResult& result, const std::vector<size_t>& expected) {
        ASSERT_EQ(result.remaining_tanks.size(), expected.size())
            << "Expected " << expected.size() << " players but got " << result.remaining_tanks.size();
        
        for (size_t i = 0; i < expected.size(); ++i) {
            EXPECT_EQ(result.remaining_tanks[i], expected[i])
                << "Player " << (i + 1) << ": expected " << expected[i] 
                << " remaining tanks but got " << result.remaining_tanks[i];
        }
    }

    /**
     * @brief Validate complete GameResult against expected values
     */
    static void validateComplete(const GameResult& result, 
                               int expectedWinner,
                               GameResult::Reason expectedReason,
                               const std::vector<size_t>& expectedRemainingTanks) {
        validateWinner(result, expectedWinner);
        validateReason(result, expectedReason);
        validateRemainingTanks(result, expectedRemainingTanks);
    }

    /**
     * @brief Validate that result indicates Player 1 won
     */
    static void validatePlayer1Win(const GameResult& result, const std::vector<size_t>& expectedRemainingTanks) {
        validateWinner(result, 1);
        validateRemainingTanks(result, expectedRemainingTanks);
        // Ensure player 1 has more tanks than player 2
        if (expectedRemainingTanks.size() >= 2) {
            EXPECT_GT(expectedRemainingTanks[0], expectedRemainingTanks[1])
                << "Player 1 should have more remaining tanks than Player 2";
        }
    }

    /**
     * @brief Validate that result indicates Player 2 won
     */
    static void validatePlayer2Win(const GameResult& result, const std::vector<size_t>& expectedRemainingTanks) {
        validateWinner(result, 2);
        validateRemainingTanks(result, expectedRemainingTanks);
        // Ensure player 2 has more tanks than player 1
        if (expectedRemainingTanks.size() >= 2) {
            EXPECT_GT(expectedRemainingTanks[1], expectedRemainingTanks[0])
                << "Player 2 should have more remaining tanks than Player 1";
        }
    }

    /**
     * @brief Validate that result indicates a tie
     */
    static void validateTie(const GameResult& result, 
                          GameResult::Reason expectedReason,
                          const std::vector<size_t>& expectedRemainingTanks) {
        validateWinner(result, 0);
        validateReason(result, expectedReason);
        validateRemainingTanks(result, expectedRemainingTanks);
    }

    /**
     * @brief Validate tie due to all tanks dead
     */
    static void validateTieAllDead(const GameResult& result) {
        validateTie(result, GameResult::Reason::ALL_TANKS_DEAD, {0, 0});
    }

    /**
     * @brief Validate tie due to max steps reached
     */
    static void validateTieMaxSteps(const GameResult& result, const std::vector<size_t>& expectedRemainingTanks) {
        validateTie(result, GameResult::Reason::MAX_STEPS, expectedRemainingTanks);
    }

    /**
     * @brief Validate tie due to zero shells remaining
     */
    static void validateTieZeroShells(const GameResult& result, const std::vector<size_t>& expectedRemainingTanks) {
        validateTie(result, GameResult::Reason::ZERO_SHELLS, expectedRemainingTanks);
    }

    /**
     * @brief Validate that remaining tanks are consistent with winner
     */
    static void validateConsistency(const GameResult& result) {
        if (result.remaining_tanks.size() < 2) {
            ADD_FAILURE() << "GameResult must have at least 2 players";
            return;
        }

        size_t player1Tanks = result.remaining_tanks[0];
        size_t player2Tanks = result.remaining_tanks[1];

        switch (result.winner) {
            case 0: // Tie
                if (result.reason == GameResult::Reason::ALL_TANKS_DEAD) {
                    EXPECT_EQ(player1Tanks, 0u) << "Tie with all tanks dead: Player 1 should have 0 tanks";
                    EXPECT_EQ(player2Tanks, 0u) << "Tie with all tanks dead: Player 2 should have 0 tanks";
                } else {
                    EXPECT_EQ(player1Tanks, player2Tanks) 
                        << "Tie result should have equal remaining tanks for both players";
                }
                break;
            case 1: // Player 1 wins
                EXPECT_GT(player1Tanks, player2Tanks)
                    << "Player 1 win should have more tanks than Player 2";
                break;
            case 2: // Player 2 wins
                EXPECT_GT(player2Tanks, player1Tanks)
                    << "Player 2 win should have more tanks than Player 1";
                break;
            default:
                ADD_FAILURE() << "Invalid winner value: " << result.winner;
        }
    }

    /**
     * @brief Create a string representation of GameResult for debugging
     */
    static std::string toString(const GameResult& result) {
        std::ostringstream oss;
        oss << "GameResult{";
        oss << "winner=" << result.winner;
        oss << ", reason=" << reasonToString(result.reason);
        oss << ", remaining_tanks=[";
        for (size_t i = 0; i < result.remaining_tanks.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << result.remaining_tanks[i];
        }
        oss << "]}";
        return oss.str();
    }

    /**
     * @brief Validate that the rounds field matches expected value
     */
    static void validateRounds(const GameResult& result, size_t expectedRounds) {
        EXPECT_EQ(result.rounds, expectedRounds)
            << "Expected " << expectedRounds << " rounds but got " << result.rounds;
    }

    /**
     * @brief Validate that the rounds field is within expected range
     */
    static void validateRoundsRange(const GameResult& result, size_t minRounds, size_t maxRounds) {
        EXPECT_GE(result.rounds, minRounds)
            << "Expected at least " << minRounds << " rounds but got " << result.rounds;
        EXPECT_LE(result.rounds, maxRounds)
            << "Expected at most " << maxRounds << " rounds but got " << result.rounds;
    }

    /**
     * @brief Validate that gameState is not null
     */
    static void validateGameStateExists(const GameResult& result) {
        ASSERT_NE(result.gameState.get(), nullptr)
            << "GameResult gameState should not be null";
    }

    /**
     * @brief Validate object at specific position in gameState
     */
    static void validateGameStateObjectAt(const GameResult& result, const Point& position, char expectedObject) {
        validateGameStateExists(result);
        char actualObject = result.gameState->getObjectAt(position.getX(), position.getY());
        EXPECT_EQ(actualObject, expectedObject)
            << "Expected object '" << expectedObject 
            << "' at position (" << position.getX() << "," << position.getY() 
            << ") but got '" << actualObject << "'";
    }

    /**
     * @brief Validate that position contains empty space
     */
    static void validateGameStateEmpty(const GameResult& result, const Point& position) {
        validateGameStateObjectAt(result, position, ' ');
    }

    /**
     * @brief Validate that position contains a wall
     */
    static void validateGameStateWall(const GameResult& result, const Point& position) {
        validateGameStateObjectAt(result, position, '#');
    }

    /**
     * @brief Validate that position contains a mine
     */
    static void validateGameStateMine(const GameResult& result, const Point& position) {
        validateGameStateObjectAt(result, position, '@');
    }

    /**
     * @brief Validate that position contains a shell
     */
    static void validateGameStateShell(const GameResult& result, const Point& position) {
        validateGameStateObjectAt(result, position, '*');
    }

    /**
     * @brief Validate that position contains a tank for specific player
     */
    static void validateGameStateTank(const GameResult& result, const Point& position, int playerId) {
        ASSERT_GE(playerId, 1) << "Player ID must be >= 1";
        ASSERT_LE(playerId, 9) << "Player ID must be <= 9";
        char expectedTank = '0' + playerId;
        validateGameStateObjectAt(result, position, expectedTank);
    }

    /**
     * @brief Validate that gameState has no tanks remaining (for ALL_TANKS_DEAD scenarios)
     * @param result The GameResult to validate
     * @param boardWidth The width of the game board
     * @param boardHeight The height of the game board
     */
    static void validateGameStateNoTanks(const GameResult& result, size_t boardWidth, size_t boardHeight) {
        validateGameStateExists(result);
        
        size_t totalTanks = 0;
        for (size_t x = 0; x < boardWidth; ++x) {
            for (size_t y = 0; y < boardHeight; ++y) {
                char obj = result.gameState->getObjectAt(x, y);
                if (obj >= '1' && obj <= '9') {
                    totalTanks++;
                }
            }
        }
        
        EXPECT_EQ(totalTanks, 0u)
            << "Expected no tanks remaining in gameState for ALL_TANKS_DEAD scenario, but found " << totalTanks;
    }

    /**
     * @brief Validate total tank count across entire gameState matches expected
     * @param result The GameResult to validate
     * @param expectedTotal Expected total number of tanks
     * @param boardWidth The width of the game board
     * @param boardHeight The height of the game board
     */
    static void validateGameStateTotalTanks(const GameResult& result, size_t expectedTotal, size_t boardWidth, size_t boardHeight) {
        validateGameStateExists(result);
        
        size_t totalTanks = 0;
        for (size_t x = 0; x < boardWidth; ++x) {
            for (size_t y = 0; y < boardHeight; ++y) {
                char obj = result.gameState->getObjectAt(x, y);
                if (obj >= '1' && obj <= '9') {
                    totalTanks++;
                }
            }
        }
        
        EXPECT_EQ(totalTanks, expectedTotal)
            << "Expected " << expectedTotal << " total tanks in gameState but found " << totalTanks;
    }

    /**
     * @brief Validate that gameState tank counts match remaining_tanks vector
     * @param result The GameResult to validate
     * @param boardWidth The width of the game board
     * @param boardHeight The height of the game board
     */
    static void validateGameStateConsistentWithRemainingTanks(const GameResult& result, size_t boardWidth, size_t boardHeight) {
        validateGameStateExists(result);
        
        // Count tanks by player in gameState
        std::map<int, size_t> gameStateTankCounts;
        
        for (size_t x = 0; x < boardWidth; ++x) {
            for (size_t y = 0; y < boardHeight; ++y) {
                char obj = result.gameState->getObjectAt(x, y);
                if (obj >= '1' && obj <= '9') {
                    int playerId = obj - '0';
                    gameStateTankCounts[playerId]++;
                }
            }
        }
        
        // Compare with remaining_tanks vector
        for (size_t i = 0; i < result.remaining_tanks.size(); ++i) {
            int playerId = i + 1; // Convert 0-based index to 1-based player ID
            size_t expectedCount = result.remaining_tanks[i];
            size_t actualCount = gameStateTankCounts[playerId];
            
            EXPECT_EQ(actualCount, expectedCount)
                << "Player " << playerId << ": gameState has " << actualCount 
                << " tanks but remaining_tanks indicates " << expectedCount;
        }
    }

    /**
     * @brief Validate complete GameResult including all fields
     */
    static void validateCompleteWithGameState(const GameResult& result,
                                            int expectedWinner,
                                            GameResult::Reason expectedReason,
                                            const std::vector<size_t>& expectedRemainingTanks,
                                            size_t expectedRounds,
                                            size_t boardWidth,
                                            size_t boardHeight) {
        validateComplete(result, expectedWinner, expectedReason, expectedRemainingTanks);
        validateRounds(result, expectedRounds);
        validateGameStateExists(result);
        validateGameStateConsistentWithRemainingTanks(result, boardWidth, boardHeight);
    }

    /**
     * @brief Convert GameResult::Reason to string for debugging
     */
    static std::string reasonToString(GameResult::Reason reason) {
        switch (reason) {
            case GameResult::Reason::ALL_TANKS_DEAD: return "ALL_TANKS_DEAD";
            case GameResult::Reason::MAX_STEPS: return "MAX_STEPS";
            case GameResult::Reason::ZERO_SHELLS: return "ZERO_SHELLS";
            default: return "UNKNOWN";
        }
    }

    /**
     * @brief Create a string representation of GameResult including gameState info
     */
    static std::string toStringWithGameState(const GameResult& result) {
        std::ostringstream oss;
        oss << "GameResult{";
        oss << "winner=" << result.winner;
        oss << ", reason=" << reasonToString(result.reason);
        oss << ", remaining_tanks=[";
        for (size_t i = 0; i < result.remaining_tanks.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << result.remaining_tanks[i];
        }
        oss << "], rounds=" << result.rounds;
        
        if (result.gameState) {
            oss << ", gameState=available";
        } else {
            oss << ", gameState=null";
        }
        oss << "}";
        return oss.str();
    }

private:
    GameResultValidator() = delete; // Static class only
};

// Note: Google Test MATCHER macros can be defined in individual test files as needed:
//
// MATCHER_P3(GameResultEq, expectedWinner, expectedReason, expectedRemainingTanks, 
//            "GameResult matches expected values") {
//     GameResultValidator::validateComplete(arg, expectedWinner, expectedReason, expectedRemainingTanks);
//     return !::testing::Test::HasFailure();
// }
//
// MATCHER_P(WinnerEq, expectedWinner, "GameResult winner matches") {
//     GameResultValidator::validateWinner(arg, expectedWinner);
//     return !::testing::Test::HasFailure();
// }
//
// MATCHER_P2(GameStateObjectEq, position, expectedObject, "GameResult gameState object matches") {
//     GameResultValidator::validateGameStateObjectAt(arg, position, expectedObject);
//     return !::testing::Test::HasFailure();
// }
//
// ... and so on for other validation methods

} // namespace GameManager_098765432_123456789 