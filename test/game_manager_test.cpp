#include <gtest/gtest.h>
#include "game_manager.h"
#include "test/mock_player.h"
#include "test/mock_algorithm.h"
#include "objects/shell.h"
#include "utils/point.h"
#include "utils/direction.h"
#include <memory>
#include <fstream>
#include <cstdio> // for std::remove
#include <iostream>

// Test fixture for GameManager
class GameManagerTest : public ::testing::Test {
protected:
    std::unique_ptr<GameManager> manager;
    MockPlayerFactory playerFactory;
    MockAlgorithmFactory algoFactory;

    static void RemoveDestroyedShells(GameManager& manager) {
        manager.removeDestroyedShells();
    }
    static std::vector<Shell>& GetShells(GameManager& manager) {
        return manager.m_shells;
    }
    static void MoveShellsOnce(GameManager& manager) {
        manager.moveShellsOnce();
    }
    static void CreateTanks(GameManager& manager, const std::vector<std::pair<int, Point>>& positions) {
        manager.createTanks(positions);
    }
    static void CreateTankAlgorithms(GameManager& manager) {
        manager.createTankAlgorithms();
    }
    static std::vector<Tank>& GetTanks(GameManager& manager) {
        return manager.m_tanks;
    }
    static std::vector<GameManager::TankWithAlgorithm>& GetTankControllers(GameManager& manager) {
        return manager.m_tankControllers;
    }
    static void ApplyAction(GameManager& manager, GameManager::TankWithAlgorithm& controller) {
        manager.applyAction(controller);
    }
    static GameBoard& GetBoard(GameManager& manager) {
        return manager.m_board;
    }
    static void SetPlayer1(GameManager& manager, std::unique_ptr<Player> player) {
        // Find player 1 in the players vector and replace it
        for (auto& playerWithId : manager.m_players) {
            if (playerWithId.playerId == 1) {
                playerWithId.player = std::move(player);
                return;
            }
        }
        // If player 1 doesn't exist, add it
        manager.m_players.push_back({1, std::move(player)});
    }
    static void SetIsClassic2PlayerGame(GameManager& manager, bool value) {
        manager.m_isClassic2PlayerGame = value;
    }
    static bool GetIsClassic2PlayerGame(GameManager& manager) {
        return manager.m_isClassic2PlayerGame;
    }
    static void LogAction(GameManager& manager) {
        manager.logAction();
    }
    static std::vector<std::string>& GetGameLog(GameManager& manager) {
        return manager.m_gameLog;
    }

    // Helper to access m_outputFilePath for testing
    static std::string& GetOutputFilePath(GameManager& manager) {
        return manager.m_outputFilePath;
    }

    // Helper to call setOutputFilePath for testing
    static void CallSetOutputFilePath(GameManager& manager, const std::string& inputPath) {
        manager.setOutputFilePath(inputPath);
    }

    void SetUp() override {
        manager = std::make_unique<GameManager>(playerFactory, algoFactory);
        // Initialize the board to a 5x5 empty board
        GetBoard(*manager) = GameBoard(5, 5);
    }

    void SetWasKilledInPreviousStep(size_t controllerIndex, bool value) {
        auto& controllers = GetTankControllers(*manager);
        ASSERT_LT(controllerIndex, controllers.size());
        controllers[controllerIndex].wasKilledInPreviousStep = value;
    }

    void CallProcessStep() {
        manager->processStep();
    }

    std::string ActionToString(ActionRequest action) {
        return manager->actionToString(action);
    }

    // ---- checkGameOver helpers ----
    bool CallCheckGameOver() {
        return manager->checkGameOver();
    }
    std::string& GetGameResult() {
        return manager->m_gameResult;
    }
    std::vector<Tank>& Tanks() {
        return GetTanks(*manager);
    }
    void SetCurrentStep(int step) {
        manager->m_currentStep = step;
    }
    int GetCurrentStep() {
        return manager->m_currentStep;
    }
    void SetMaxSteps(int maxSteps) {
        manager->m_maximum_steps = maxSteps;
    }

    void CallSaveResults(const std::string& file) {
        manager->saveResults(file);
    }

    // Helper to access and set remaining steps
    int GetRemainingSteps() {
        return manager->m_remaining_steps;
    }
    
    void SetRemainingSteps(int steps) {
        manager->m_remaining_steps = steps;
    }
};

TEST_F(GameManagerTest, RemoveDestroyedShells_RemovesOnlyDestroyed) {
    // Add shells: 2 active, 1 destroyed
    Shell shell1(1, Point(1, 1), Direction::Right);
    Shell shell2(2, Point(2, 2), Direction::Left);
    Shell shell3(1, Point(3, 3), Direction::Up);
    shell2.destroy(); // Mark shell2 as destroyed
    GetShells(*manager).push_back(shell1);
    GetShells(*manager).push_back(shell2);
    GetShells(*manager).push_back(shell3);
    
    RemoveDestroyedShells(*manager);

    // Only shell1 and shell3 should remain
    const auto& shells = GameManagerTest::GetShells(*manager);
    ASSERT_EQ(shells.size(), 2u);
    EXPECT_EQ(shells[0].getPosition(), Point(1, 1));
    EXPECT_EQ(shells[1].getPosition(), Point(3, 3));
}

TEST_F(GameManagerTest, MoveShellsOnce_MovesAllShellsCorrectly) {
    // Assume a 5x5 board for wrap logic
    // Add shells in different directions
    GetShells(*manager).clear();
    Shell shellRight(1, Point(1, 1), Direction::Right);
    Shell shellLeft(2, Point(0, 0), Direction::Left);
    Shell shellUp(1, Point(2, 2), Direction::Up);
    Shell shellDown(2, Point(4, 4), Direction::Down);
    GetShells(*manager).push_back(shellRight);
    GetShells(*manager).push_back(shellLeft);
    GetShells(*manager).push_back(shellUp);
    GetShells(*manager).push_back(shellDown);

    // Call moveShellsOnce
    MoveShellsOnce(*manager);

    // Check new positions
    // Right: (1,1) -> (2,1)
    EXPECT_EQ(GetShells(*manager)[0].getPosition(), Point(2, 1));
    // Left: (0,0) -> (4,0) (wrap)
    EXPECT_EQ(GetShells(*manager)[1].getPosition(), Point(4, 0));
    // Up: (2,2) -> (2,1)
    EXPECT_EQ(GetShells(*manager)[2].getPosition(), Point(2, 1));
    // Down: (4,4) -> (4,0) (wrap)
    EXPECT_EQ(GetShells(*manager)[3].getPosition(), Point(4, 0));
}

TEST_F(GameManagerTest, CreateTanks_CreatesTanksAtCorrectPositions) {
    std::vector<std::pair<int, Point>> positions = {
        {1, Point(0, 0)},
        {2, Point(4, 4)},
        {1, Point(2, 2)}
    };
    CreateTanks(*manager, positions);
    auto& tanks = GetTanks(*manager);
    ASSERT_EQ(tanks.size(), 3u);
    EXPECT_EQ(tanks[0].getPlayerId(), 1);
    EXPECT_EQ(tanks[0].getPosition(), Point(0, 0));
    EXPECT_EQ(tanks[1].getPlayerId(), 2);
    EXPECT_EQ(tanks[1].getPosition(), Point(4, 4));
    EXPECT_EQ(tanks[2].getPlayerId(), 1);
    EXPECT_EQ(tanks[2].getPosition(), Point(2, 2));
}

TEST_F(GameManagerTest, CreateTankAlgorithms_AssociatesAlgorithmsWithTanks) {
    std::vector<std::pair<int, Point>> positions = {
        {1, Point(0, 0)},
        {2, Point(4, 4)},
        {1, Point(2, 2)}
    };
    CreateTanks(*manager, positions);
    CreateTankAlgorithms(*manager);
    auto& controllers = GetTankControllers(*manager);
    ASSERT_EQ(controllers.size(), 3u);
    for (const auto& controller : controllers) {
        EXPECT_NE(controller.algorithm, nullptr);
    }
}

TEST_F(GameManagerTest, ApplyAction_MoveForward_Success) {
    std::vector<std::pair<int, Point>> positions = { {1, Point(1, 1)} };
    CreateTanks(*manager, positions);
    CreateTankAlgorithms(*manager);
    auto& controller = GetTankControllers(*manager)[0];
    controller.tank.setDirection(Direction::Right);
    controller.nextAction = ActionRequest::MoveForward;
    ApplyAction(*manager, controller);
    EXPECT_EQ(controller.tank.getPosition(), Point(2, 1));
}

TEST_F(GameManagerTest, ApplyAction_MoveForward_BlockedByWall) {
    GetBoard(*manager) = GameBoard(5, 5);
    GetBoard(*manager).setCellType(1, 0, GameBoard::CellType::Wall);
    std::vector<std::pair<int, Point>> positions = { {1, Point(0, 0)} };
    CreateTanks(*manager, positions);
    CreateTankAlgorithms(*manager);
    auto& controller = GetTankControllers(*manager)[0];
    controller.tank.setDirection(Direction::Right);
    controller.nextAction = ActionRequest::MoveForward;
    ApplyAction(*manager, controller);
    EXPECT_EQ(controller.tank.getPosition(), Point(0, 0));
}

TEST_F(GameManagerTest, ApplyAction_RotateLeft45_UpdatesDirection) {
    std::vector<std::pair<int, Point>> positions = { {1, Point(2, 2)} };
    CreateTanks(*manager, positions);
    CreateTankAlgorithms(*manager);
    auto& controller = GetTankControllers(*manager)[0];
    controller.tank.setDirection(Direction::Right);
    controller.nextAction = ActionRequest::RotateLeft45;
    ApplyAction(*manager, controller);
    EXPECT_EQ(controller.tank.getDirection(), Direction::UpRight);
}

TEST_F(GameManagerTest, ApplyAction_Shoot_Success) {
    std::vector<std::pair<int, Point>> positions = { {1, Point(1, 1)} };
    CreateTanks(*manager, positions);
    CreateTankAlgorithms(*manager);
    auto& controller = GetTankControllers(*manager)[0];
    controller.tank.setDirection(Direction::Down);
    controller.nextAction = ActionRequest::Shoot;
    int shellsBefore = GetShells(*manager).size();
    ApplyAction(*manager, controller);
    int shellsAfter = GetShells(*manager).size();
    EXPECT_EQ(shellsAfter, shellsBefore + 1);
    EXPECT_EQ(GetShells(*manager).back().getPosition(), Point(1, 1));
    EXPECT_EQ(GetShells(*manager).back().getDirection(), Direction::Down);
}

TEST_F(GameManagerTest, ApplyAction_Shoot_FailsOnCooldown) {
    std::vector<std::pair<int, Point>> positions = { {1, Point(1, 1)} };
    CreateTanks(*manager, positions);
    CreateTankAlgorithms(*manager);
    auto& controller = GetTankControllers(*manager)[0];
    controller.tank.setDirection(Direction::Down);
    controller.tank.shoot();
    controller.nextAction = ActionRequest::Shoot;
    int shellsBefore = GetShells(*manager).size();
    ApplyAction(*manager, controller);
    int shellsAfter = GetShells(*manager).size();
    EXPECT_EQ(shellsAfter, shellsBefore);
}

TEST_F(GameManagerTest, ApplyAction_GetBattleInfo_CallsPlayer) {
    auto mockPlayer = std::make_unique<MockPlayer>();
    auto* mockPlayerPtr = mockPlayer.get();
    SetPlayer1(*manager, std::move(mockPlayer));
    std::vector<std::pair<int, Point>> positions = { {1, Point(1, 1)} };
    CreateTanks(*manager, positions);
    CreateTankAlgorithms(*manager);
    auto& controller = GetTankControllers(*manager)[0];
    controller.nextAction = ActionRequest::GetBattleInfo;
    EXPECT_CALL(*mockPlayerPtr, updateTankWithBattleInfo(::testing::_, ::testing::_)).Times(1);
    ApplyAction(*manager, controller);
}

TEST_F(GameManagerTest, ApplyAction_DoNothing_OnlyStateUpdated) {
    std::vector<std::pair<int, Point>> positions = { {1, Point(1, 1)} };
    CreateTanks(*manager, positions);
    CreateTankAlgorithms(*manager);
    auto& controller = GetTankControllers(*manager)[0];
    controller.nextAction = ActionRequest::DoNothing;
    EXPECT_NO_THROW(ApplyAction(*manager, controller));
    EXPECT_EQ(controller.tank.getPosition(), Point(1, 1));
}

TEST_F(GameManagerTest, ApplyAction_DestroyedTank_NoAction) {
    std::vector<std::pair<int, Point>> positions = { {1, Point(1, 1)} };
    CreateTanks(*manager, positions);
    CreateTankAlgorithms(*manager);
    auto& controller = GetTankControllers(*manager)[0];
    controller.tank.destroy();
    controller.nextAction = ActionRequest::MoveForward;
    ApplyAction(*manager, controller);
    EXPECT_EQ(controller.tank.getPosition(), Point(1, 1));
}

TEST_F(GameManagerTest, LogAction_AllValidActions) {
    std::vector<std::pair<int, Point>> positions = {
        {1, Point(0, 0)}, {2, Point(1, 0)}, {1, Point(2, 0)}
    };
    CreateTanks(*manager, positions);
    CreateTankAlgorithms(*manager);
    auto& controllers = GetTankControllers(*manager);
    controllers[0].nextAction = ActionRequest::MoveForward;
    controllers[1].nextAction = ActionRequest::Shoot;
    controllers[2].nextAction = ActionRequest::RotateLeft90;
    controllers[0].actionSuccess = true;
    controllers[1].actionSuccess = true;
    controllers[2].actionSuccess = true;
    LogAction(*manager);
    ASSERT_FALSE(GetGameLog(*manager).empty());
    EXPECT_EQ(GetGameLog(*manager).back(), "MoveForward, Shoot, RotateLeft90");
}

TEST_F(GameManagerTest, LogAction_SomeActionsIgnored) {
    std::vector<std::pair<int, Point>> positions = {
        {1, Point(0, 0)}, {2, Point(1, 0)}, {1, Point(2, 0)}
    };
    CreateTanks(*manager, positions);
    CreateTankAlgorithms(*manager);
    auto& controllers = GetTankControllers(*manager);
    controllers[0].nextAction = ActionRequest::MoveForward;
    controllers[1].nextAction = ActionRequest::Shoot;
    controllers[2].nextAction = ActionRequest::RotateLeft90;
    controllers[0].actionSuccess = false;
    controllers[1].actionSuccess = true;
    controllers[2].actionSuccess = false;
    LogAction(*manager);
    ASSERT_FALSE(GetGameLog(*manager).empty());
    EXPECT_EQ(GetGameLog(*manager).back(), "MoveForward (ignored), Shoot, RotateLeft90 (ignored)");
}

TEST_F(GameManagerTest, LogAction_KilledThisStep) {
    std::vector<std::pair<int, Point>> positions = {
        {1, Point(0, 0)}, {2, Point(1, 0)}
    };
    CreateTanks(*manager, positions);
    CreateTankAlgorithms(*manager);
    auto& controllers = GetTankControllers(*manager);
    controllers[0].nextAction = ActionRequest::MoveForward;
    controllers[1].nextAction = ActionRequest::Shoot;
    controllers[0].actionSuccess = true;
    controllers[1].actionSuccess = true;
    controllers[0].tank.destroy();
    SetWasKilledInPreviousStep(0, false);
    LogAction(*manager);
    ASSERT_FALSE(GetGameLog(*manager).empty());
    EXPECT_EQ(GetGameLog(*manager).back(), "MoveForward (killed), Shoot");
}

TEST_F(GameManagerTest, LogAction_AlreadyDead) {
    std::vector<std::pair<int, Point>> positions = {
        {1, Point(0, 0)}, {2, Point(1, 0)}
    };
    CreateTanks(*manager, positions);
    CreateTankAlgorithms(*manager);
    auto& controllers = GetTankControllers(*manager);
    controllers[0].nextAction = ActionRequest::MoveForward;
    controllers[1].nextAction = ActionRequest::Shoot;
    controllers[0].actionSuccess = true;
    controllers[1].actionSuccess = true;
    controllers[0].tank.destroy();
    SetWasKilledInPreviousStep(0, true);
    LogAction(*manager);
    ASSERT_FALSE(GetGameLog(*manager).empty());
    EXPECT_EQ(GetGameLog(*manager).back(), "Killed, Shoot");
}

TEST_F(GameManagerTest, LogAction_KilledAndIgnored) {
    std::vector<std::pair<int, Point>> positions = {
        {1, Point(0, 0)}, {2, Point(1, 0)}
    };
    CreateTanks(*manager, positions);
    CreateTankAlgorithms(*manager);
    auto& controllers = GetTankControllers(*manager);
    controllers[0].nextAction = ActionRequest::MoveForward;
    controllers[1].nextAction = ActionRequest::Shoot;
    controllers[0].actionSuccess = true;
    controllers[1].actionSuccess = false;
    controllers[0].tank.destroy();
    SetWasKilledInPreviousStep(0, false);
    LogAction(*manager);
    ASSERT_FALSE(GetGameLog(*manager).empty());
    EXPECT_EQ(GetGameLog(*manager).back(), "MoveForward (killed), Shoot (ignored)");
}

TEST_F(GameManagerTest, LogAction_MultipleKilled) {
    std::vector<std::pair<int, Point>> positions = {
        {1, Point(0, 0)}, {2, Point(1, 0)}, {1, Point(2, 0)}
    };
    CreateTanks(*manager, positions);
    CreateTankAlgorithms(*manager);
    auto& controllers = GetTankControllers(*manager);
    controllers[0].nextAction = ActionRequest::MoveForward;
    controllers[1].nextAction = ActionRequest::Shoot;
    controllers[2].nextAction = ActionRequest::RotateLeft90;
    controllers[0].actionSuccess = true;
    controllers[1].actionSuccess = true;
    controllers[2].actionSuccess = true;
    controllers[0].tank.destroy();
    controllers[1].tank.destroy();
    SetWasKilledInPreviousStep(0, false);
    SetWasKilledInPreviousStep(1, false);
    LogAction(*manager);
    ASSERT_FALSE(GetGameLog(*manager).empty());
    EXPECT_EQ(GetGameLog(*manager).back(), "MoveForward (killed), Shoot (killed), RotateLeft90");
}

TEST_F(GameManagerTest, LogAction_AllKilled) {
    std::vector<std::pair<int, Point>> positions = {
        {1, Point(0, 0)}, {2, Point(1, 0)}, {1, Point(2, 0)}
    };
    CreateTanks(*manager, positions);
    CreateTankAlgorithms(*manager);
    auto& controllers = GetTankControllers(*manager);
    for (size_t i = 0; i < controllers.size(); ++i) {
        controllers[i].tank.destroy();
        SetWasKilledInPreviousStep(i, true);
    }
    controllers[0].nextAction = ActionRequest::MoveForward;
    controllers[1].nextAction = ActionRequest::Shoot;
    controllers[2].nextAction = ActionRequest::RotateLeft90;
    LogAction(*manager);
    ASSERT_FALSE(GetGameLog(*manager).empty());
    EXPECT_EQ(GetGameLog(*manager).back(), "Killed, Killed, Killed");
}

TEST_F(GameManagerTest, LogAction_AllDoNothing) {
    std::vector<std::pair<int, Point>> positions = {
        {1, Point(0, 0)}, {2, Point(1, 0)}, {1, Point(2, 0)}
    };
    CreateTanks(*manager, positions);
    CreateTankAlgorithms(*manager);
    auto& controllers = GetTankControllers(*manager);
    for (auto& c : controllers) {
        c.nextAction = ActionRequest::DoNothing;
        c.actionSuccess = true;
    }
    LogAction(*manager);
    ASSERT_FALSE(GetGameLog(*manager).empty());
    EXPECT_EQ(GetGameLog(*manager).back(), "DoNothing, DoNothing, DoNothing");
}

TEST_F(GameManagerTest, LogAction_MixedKilledIgnoredValid) {
    std::vector<std::pair<int, Point>> positions = {
        {1, Point(0, 0)}, {2, Point(1, 0)}, {1, Point(2, 0)}
    };
    CreateTanks(*manager, positions);
    CreateTankAlgorithms(*manager);
    auto& controllers = GetTankControllers(*manager);
    controllers[0].nextAction = ActionRequest::MoveForward;
    controllers[1].nextAction = ActionRequest::Shoot;
    controllers[2].nextAction = ActionRequest::RotateLeft90;
    controllers[0].actionSuccess = true;
    controllers[1].actionSuccess = false;
    controllers[2].actionSuccess = true;
    controllers[0].tank.destroy();
    SetWasKilledInPreviousStep(0, false);
    LogAction(*manager);
    ASSERT_FALSE(GetGameLog(*manager).empty());
    EXPECT_EQ(GetGameLog(*manager).back(), "MoveForward (killed), Shoot (ignored), RotateLeft90");
}

TEST_F(GameManagerTest, LogAction_OrderMatchesTanksOnBoard) {
    // Tanks in order: (2,0), (0,0), (1,0)
    std::vector<std::pair<int, Point>> positions = {
        {1, Point(2, 0)}, {2, Point(0, 0)}, {1, Point(1, 0)}
    };
    CreateTanks(*manager, positions);
    CreateTankAlgorithms(*manager);
    auto& controllers = GetTankControllers(*manager);
    controllers[0].nextAction = ActionRequest::MoveForward;
    controllers[1].nextAction = ActionRequest::Shoot;
    controllers[2].nextAction = ActionRequest::RotateLeft90;
    controllers[0].actionSuccess = true;
    controllers[1].actionSuccess = true;
    controllers[2].actionSuccess = true;
    LogAction(*manager);
    ASSERT_FALSE(GetGameLog(*manager).empty());
    // The log should match the order of tanks as found in m_tankControllers
    EXPECT_EQ(GetGameLog(*manager).back(), "MoveForward, Shoot, RotateLeft90");
}

TEST_F(GameManagerTest, ProcessStep_OneTankMoves_UpdatesPositionAndLogs) {
    std::vector<std::pair<int, Point>> positions = { {1, Point(1, 1)} };
    CreateTanks(*manager, positions);
    CreateTankAlgorithms(*manager);
    auto& controller = GetTankControllers(*manager)[0];
    controller.tank.setDirection(Direction::Right);
    auto* mockAlgo = dynamic_cast<MockAlgorithm*>(controller.algorithm.get());
    ASSERT_NE(mockAlgo, nullptr);
    mockAlgo->setConstantAction(ActionRequest::MoveForward);
    CallProcessStep();
    EXPECT_EQ(controller.tank.getPosition(), Point(2, 1));
    EXPECT_EQ(GetGameLog(*manager).back(), "MoveForward");
}

TEST_F(GameManagerTest, ProcessStep_OneTankShoots_AddsShellAndLogs) {
    std::vector<std::pair<int, Point>> positions = { {1, Point(1, 1)} };
    CreateTanks(*manager, positions);
    CreateTankAlgorithms(*manager);
    auto& controller = GetTankControllers(*manager)[0];
    controller.tank.setDirection(Direction::Down);
    auto* mockAlgo = dynamic_cast<MockAlgorithm*>(controller.algorithm.get());
    ASSERT_NE(mockAlgo, nullptr);
    mockAlgo->setConstantAction(ActionRequest::Shoot);
    int shellsBefore = GetShells(*manager).size();
    CallProcessStep();
    int shellsAfter = GetShells(*manager).size();
    EXPECT_EQ(shellsAfter, shellsBefore + 1);
    EXPECT_EQ(GetShells(*manager).back().getPosition(), Point(1, 2));
    EXPECT_EQ(GetShells(*manager).back().getDirection(), Direction::Down);
    EXPECT_EQ(GetGameLog(*manager).back(), "Shoot");
}

TEST_F(GameManagerTest, ProcessStep_ShellHitsTank_TankDestroyedAndLogKilled) {
    std::vector<std::pair<int, Point>> positions = { {1, Point(1, 1)}, {2, Point(3, 1)} };
    CreateTanks(*manager, positions);
    CreateTankAlgorithms(*manager);
    auto& shooter = GetTankControllers(*manager)[0];
    auto& target = GetTankControllers(*manager)[1];
    shooter.tank.setDirection(Direction::Right);
    auto* mockAlgoShooter = dynamic_cast<MockAlgorithm*>(shooter.algorithm.get());
    auto* mockAlgoTarget = dynamic_cast<MockAlgorithm*>(target.algorithm.get());
    ASSERT_NE(mockAlgoShooter, nullptr);
    ASSERT_NE(mockAlgoTarget, nullptr);
    mockAlgoShooter->setActionSequence({ActionRequest::Shoot, ActionRequest::DoNothing});
    mockAlgoTarget->setConstantAction(ActionRequest::DoNothing);
    // Step 1: shoot, shell moves 4 cells right (from (1,1) to (5,1) with wrap)
    CallProcessStep();
    // Target not yet destroyed
    EXPECT_FALSE(target.tank.isDestroyed());
    // Step 2: shell moves again, should hit the target at (5,1)
    CallProcessStep();
    EXPECT_TRUE(target.tank.isDestroyed());
    // Log should show MoveForward (killed) for target
    EXPECT_EQ(GetGameLog(*manager).back(), "DoNothing, DoNothing (killed)");
}

TEST_F(GameManagerTest, ProcessStep_TankOnCooldown_ShootIgnoredAndLogs) {
    std::vector<std::pair<int, Point>> positions = { {1, Point(1, 1)} };
    CreateTanks(*manager, positions);
    CreateTankAlgorithms(*manager);
    auto& controller = GetTankControllers(*manager)[0];
    controller.tank.setDirection(Direction::Right);
    auto* mockAlgo = dynamic_cast<MockAlgorithm*>(controller.algorithm.get());
    ASSERT_NE(mockAlgo, nullptr);
    mockAlgo->setConstantAction(ActionRequest::Shoot);
    // Simulate tank just shot (cooldown active)
    controller.tank.shoot();
    CallProcessStep();
    EXPECT_EQ(GetGameLog(*manager).back(), "Shoot (ignored)");
}

TEST_F(GameManagerTest, ProcessStep_TankKilledThisStep_LogsKilledThisStep) {
    std::vector<std::pair<int, Point>> positions = { {1, Point(1, 1)}, {2, Point(2, 1)} };
    CreateTanks(*manager, positions);
    CreateTankAlgorithms(*manager);
    auto& shooter = GetTankControllers(*manager)[0];
    auto& target = GetTankControllers(*manager)[1];
    shooter.tank.setDirection(Direction::Right);
    auto* mockAlgoShooter = dynamic_cast<MockAlgorithm*>(shooter.algorithm.get());
    auto* mockAlgoTarget = dynamic_cast<MockAlgorithm*>(target.algorithm.get());
    ASSERT_NE(mockAlgoShooter, nullptr);
    ASSERT_NE(mockAlgoTarget, nullptr);
    mockAlgoShooter->setConstantAction(ActionRequest::Shoot);
    mockAlgoTarget->setConstantAction(ActionRequest::DoNothing);
    // Step 1: shoot, shell moves 4 cells right (from (1,1) to (5,1)), hits target at (5,1)
    CallProcessStep();
    EXPECT_TRUE(target.tank.isDestroyed());
    EXPECT_EQ(GetGameLog(*manager).back(), "Shoot, DoNothing (killed)");
}

TEST_F(GameManagerTest, ProcessStep_AlreadyDeadTank_LogsKilled) {
    std::vector<std::pair<int, Point>> positions = { {1, Point(1, 1)} };
    CreateTanks(*manager, positions);
    CreateTankAlgorithms(*manager);
    auto& controller = GetTankControllers(*manager)[0];
    controller.tank.destroy();
    controller.wasKilledInPreviousStep = true;
    auto* mockAlgo = dynamic_cast<MockAlgorithm*>(controller.algorithm.get());
    ASSERT_NE(mockAlgo, nullptr);
    mockAlgo->setConstantAction(ActionRequest::MoveForward);
    CallProcessStep();
    EXPECT_EQ(GetGameLog(*manager).back(), "Killed");
}

TEST_F(GameManagerTest, ProcessStep_MixedActions_LogsAllScenarios) {
    std::vector<std::pair<int, Point>> positions = {
        {2, Point(0, 0)}, {2, Point(1, 0)}, {2, Point(2, 0)}
    };
    CreateTanks(*manager, positions);
    CreateTankAlgorithms(*manager);
    auto& c0 = GetTankControllers(*manager)[0];
    auto& c1 = GetTankControllers(*manager)[1];
    auto& c2 = GetTankControllers(*manager)[2];
    auto* mockAlgo0 = dynamic_cast<MockAlgorithm*>(c0.algorithm.get());
    auto* mockAlgo1 = dynamic_cast<MockAlgorithm*>(c1.algorithm.get());
    auto* mockAlgo2 = dynamic_cast<MockAlgorithm*>(c2.algorithm.get());
    ASSERT_NE(mockAlgo0, nullptr);
    ASSERT_NE(mockAlgo1, nullptr);
    ASSERT_NE(mockAlgo2, nullptr);
    mockAlgo0->setConstantAction(ActionRequest::MoveForward);
    mockAlgo1->setConstantAction(ActionRequest::Shoot);
    mockAlgo2->setConstantAction(ActionRequest::RotateLeft90);
    c0.tank.destroy();
    c0.wasKilledInPreviousStep = false;
    CallProcessStep();
    EXPECT_EQ(GetGameLog(*manager).back(), "DoNothing (killed), Shoot, RotateLeft90 (killed)");
}

TEST_F(GameManagerTest, ProcessStep_AllShellsDestroyed_RemovesShells) {
    // Add two destroyed shells
    GetShells(*manager).push_back(Shell(1, Point(1, 1), Direction::Right));
    GetShells(*manager).push_back(Shell(2, Point(2, 2), Direction::Left));
    GetShells(*manager)[0].destroy();
    GetShells(*manager)[1].destroy();
    // Add a tank so processStep can run
    std::vector<std::pair<int, Point>> positions = { {1, Point(0, 0)} };
    CreateTanks(*manager, positions);
    CreateTankAlgorithms(*manager);
    auto& controller = GetTankControllers(*manager)[0];
    auto* mockAlgo = dynamic_cast<MockAlgorithm*>(controller.algorithm.get());
    ASSERT_NE(mockAlgo, nullptr);
    mockAlgo->setConstantAction(ActionRequest::DoNothing);
    CallProcessStep();
    EXPECT_TRUE(GetShells(*manager).empty());
}

TEST_F(GameManagerTest, ProcessStep_MoveBackward_OnlyMovesOnThirdStep) {
    std::vector<std::pair<int, Point>> positions = { {1, Point(2, 2)} };
    CreateTanks(*manager, positions);
    CreateTankAlgorithms(*manager);
    auto& controller = GetTankControllers(*manager)[0];
    controller.tank.setDirection(Direction::Up);
    auto* mockAlgo = dynamic_cast<MockAlgorithm*>(controller.algorithm.get());
    ASSERT_NE(mockAlgo, nullptr);
    mockAlgo->setConstantAction(ActionRequest::MoveBackward);
    // Step 1
    CallProcessStep();
    EXPECT_EQ(controller.tank.getPosition(), Point(2, 2));
    EXPECT_EQ(GetGameLog(*manager).back(), "MoveBackward");
    // Step 2
    CallProcessStep();
    EXPECT_EQ(controller.tank.getPosition(), Point(2, 2));
    EXPECT_EQ(GetGameLog(*manager).back(), "MoveBackward (ignored)");
    // Step 3
    CallProcessStep();
    EXPECT_EQ(controller.tank.getPosition(), Point(2, 3));
    EXPECT_EQ(GetGameLog(*manager).back(), "MoveBackward (ignored)");
    
    // Step 4  - continous backward
    CallProcessStep();
    EXPECT_EQ(controller.tank.getPosition(), Point(2, 4));
    EXPECT_EQ(GetGameLog(*manager).back(), "MoveBackward");
}

TEST_F(GameManagerTest, ProcessStep_MoveForward_cancelMoveBackward) {
    Point initialPosition = Point(2, 2);
    std::vector<std::pair<int, Point>> positions = { {1, initialPosition} };
    CreateTanks(*manager, positions);
    CreateTankAlgorithms(*manager);
    auto& controller = GetTankControllers(*manager)[0];
    controller.tank.setDirection(Direction::Up);
    auto* mockAlgo = dynamic_cast<MockAlgorithm*>(controller.algorithm.get());
    ASSERT_NE(mockAlgo, nullptr);
    mockAlgo->setActionSequence({ActionRequest::MoveBackward, ActionRequest::MoveForward, ActionRequest::MoveForward});
    // Step 1
    CallProcessStep();
    EXPECT_EQ(controller.tank.getPosition(), initialPosition);
    EXPECT_EQ(GetGameLog(*manager).back(), "MoveBackward");
    // Step 2
    CallProcessStep();
    EXPECT_EQ(controller.tank.getPosition(), initialPosition);
    EXPECT_EQ(GetGameLog(*manager).back(), "MoveForward");
    // Step 3
    CallProcessStep();
    EXPECT_EQ(controller.tank.getPosition(), Point(2, 1));
    EXPECT_EQ(GetGameLog(*manager).back(), "MoveForward");
}

TEST_F(GameManagerTest, ProcessStep_Shoot_CooldownResetsAfter4Steps) {
    std::vector<std::pair<int, Point>> positions = { {1, Point(0, 0)} };
    GetBoard(*manager) = GameBoard(20, 20);

    CreateTanks(*manager, positions);
    CreateTankAlgorithms(*manager);
    auto& controller = GetTankControllers(*manager)[0];
    controller.tank.setDirection(Direction::Right);
    auto* mockAlgo = dynamic_cast<MockAlgorithm*>(controller.algorithm.get());
    ASSERT_NE(mockAlgo, nullptr);
    mockAlgo->setConstantAction(ActionRequest::Shoot);
    // Step 1: Shoot (should succeed)
    CallProcessStep();
    EXPECT_EQ(GetGameLog(*manager).back(), "Shoot");
    // Steps 2-4: Shoot (should be ignored due to cooldown)
    for (int i = 0; i < Tank::SHOOT_COOLDOWN; ++i) {
        CallProcessStep();
        EXPECT_EQ(GetGameLog(*manager).back(), "Shoot (ignored)");
    }
    // Step 5: Shoot (should succeed again)
    CallProcessStep();
    EXPECT_EQ(GetGameLog(*manager).back(), "Shoot");
}

TEST_F(GameManagerTest, ProcessStep_Shoot_CannotShootMoreThanMaxShells) {
    std::vector<std::pair<int, Point>> positions = { {1, Point(1, 1)} };
    int maxShells = Tank::INITIAL_SHELLS;
    int shootCooldown = Tank::SHOOT_COOLDOWN;
    GetBoard(*manager) = GameBoard(2 * maxShells * (shootCooldown + 1), 1);

    CreateTanks(*manager, positions);
    CreateTankAlgorithms(*manager);
    auto& controller = GetTankControllers(*manager)[0];
    controller.tank.setDirection(Direction::Right);
    auto* mockAlgo = dynamic_cast<MockAlgorithm*>(controller.algorithm.get());
    ASSERT_NE(mockAlgo, nullptr);
    mockAlgo->setConstantAction(ActionRequest::Shoot);
    for (int i = 0; i < maxShells * (shootCooldown + 1); ++i) {
        CallProcessStep();
        if (i % (shootCooldown + 1) == 0) {
            EXPECT_EQ(GetGameLog(*manager).back(), "Shoot");
        } else {
            EXPECT_EQ(GetGameLog(*manager).back(), "Shoot (ignored)");
        }
    }
    CallProcessStep();
    EXPECT_EQ(GetGameLog(*manager).back(), "Shoot (ignored)");
}

TEST_F(GameManagerTest, GetBattleInfoIgnoredDuringBackwardMovement) {
    // Setup: Create tanks
    std::vector<std::pair<int, Point>> positions = { {1, Point(2, 2)} };
    CreateTanks(*manager, positions);
    CreateTankAlgorithms(*manager);
    
    auto& controller = GetTankControllers(*manager)[0];
    controller.tank.setDirection(Direction::Up);
    
    // Setup mock algorithm to request backward movement then GetBattleInfo
    auto* mockAlgo = dynamic_cast<MockAlgorithm*>(controller.algorithm.get());
    ASSERT_NE(mockAlgo, nullptr);
    
    std::vector<ActionRequest> actions = {
        ActionRequest::MoveBackward,    // Step 1: Request backward movement
        ActionRequest::GetBattleInfo,   // Step 2: Try to get battle info (should be ignored)
        ActionRequest::GetBattleInfo,   // Step 3: Try again (should be ignored, backward executes)
        ActionRequest::DoNothing        // Step 4: Do nothing
    };
    mockAlgo->setActionSequence(actions);
    
    // Track initial state
    Point initialPos = controller.tank.getPosition();
    
    // Step 1: Request backward movement
    CallProcessStep();
    EXPECT_TRUE(controller.tank.isMovingBackward());
    EXPECT_EQ(controller.tank.getBackwardCounter(), 1);
    EXPECT_EQ(controller.tank.getPosition(), initialPos); // No movement yet
    EXPECT_TRUE(controller.actionSuccess); // Backward request accepted
    
    // Step 2: Try GetBattleInfo during backward movement (should be ignored)
    CallProcessStep();
    EXPECT_TRUE(controller.tank.isMovingBackward());
    EXPECT_EQ(controller.tank.getBackwardCounter(), 2);
    EXPECT_EQ(controller.tank.getPosition(), initialPos); // Still no movement
    EXPECT_FALSE(controller.actionSuccess); // GetBattleInfo was ignored
    
    // Step 3: Try GetBattleInfo again (should be ignored, backward should execute)
    CallProcessStep();
    EXPECT_FALSE(controller.tank.isMovingBackward()); // Backward movement completed
    EXPECT_EQ(controller.tank.getBackwardCounter(), 0);
    
    // Tank should have moved backward (down one position since facing Up)
    Point expectedPos = Point(initialPos.getX(), initialPos.getY() + 1);
    EXPECT_EQ(controller.tank.getPosition(), expectedPos);
    EXPECT_FALSE(controller.actionSuccess); // GetBattleInfo was ignored
    
    // Verify the logs show the ignored actions
    auto& gameLog = GetGameLog(*manager);
    EXPECT_GE(gameLog.size(), 3u);
    EXPECT_EQ(gameLog[gameLog.size()-3], "MoveBackward"); // Step 1
    EXPECT_EQ(gameLog[gameLog.size()-2], "GetBattleInfo (ignored)"); // Step 2
    EXPECT_EQ(gameLog[gameLog.size()-1], "GetBattleInfo (ignored)"); // Step 3
}

TEST_F(GameManagerTest, CheckGameOver_Player1Wins) {
    std::vector<std::pair<int, Point>> positions = { {1, Point(0,0)}, {1, Point(1,0)} };
    CreateTanks(*manager, positions);
    CreateTankAlgorithms(*manager);
    bool over = CallCheckGameOver();
    EXPECT_TRUE(over);
    EXPECT_EQ(GetGameResult(), "Player 1 won with 2 tanks still alive");
}

TEST_F(GameManagerTest, CheckGameOver_Player2Wins) {
    std::vector<std::pair<int, Point>> positions = { {2, Point(0,0)}, {2, Point(1,0)} };
    CreateTanks(*manager, positions);
    CreateTankAlgorithms(*manager);
    bool over = CallCheckGameOver();
    EXPECT_TRUE(over);
    EXPECT_EQ(GetGameResult(), "Player 2 won with 2 tanks still alive");
}

TEST_F(GameManagerTest, CheckGameOver_TieZeroTanks) {
    std::vector<std::pair<int, Point>> positions = { {1, Point(0,0)}, {2, Point(1,0)} };
    CreateTanks(*manager, positions);
    CreateTankAlgorithms(*manager);
    SetIsClassic2PlayerGame(*manager, true); // Set for test environment
    for (auto& t : Tanks()) t.destroy();
    bool over = CallCheckGameOver();
    EXPECT_TRUE(over);
    EXPECT_EQ(GetGameResult(), "Tie, both players have zero tanks");
}

TEST_F(GameManagerTest, CheckGameOver_TieMaxSteps) {
    std::vector<std::pair<int, Point>> positions = { {1, Point(0,0)}, {2, Point(1,0)} };
    CreateTanks(*manager, positions);
    CreateTankAlgorithms(*manager);
    SetIsClassic2PlayerGame(*manager, true); // Set for test environment
    SetMaxSteps(5);
    SetCurrentStep(5);
    bool over = CallCheckGameOver();
    EXPECT_TRUE(over);
    EXPECT_EQ(GetGameResult(), "Tie, reached max steps = 5, player 1 has 1 tanks, player 2 has 1 tanks");
}

TEST_F(GameManagerTest, CheckGameOver_GameContinues) {
    std::vector<std::pair<int, Point>> positions = { {1, Point(0,0)}, {2, Point(1,0)} };
    CreateTanks(*manager, positions);
    CreateTankAlgorithms(*manager);
    SetMaxSteps(10);
    SetCurrentStep(3);
    bool over = CallCheckGameOver();
    EXPECT_FALSE(over);
}

TEST_F(GameManagerTest, CheckGameOver_TieZeroShellsForExactly40Steps) {
    // Arrange: Create tanks for both players with zero shells
    std::vector<std::pair<int, Point>> positions = { {1, Point(0,0)}, {2, Point(1,0)} };
    CreateTanks(*manager, positions);
    CreateTankAlgorithms(*manager);
    SetIsClassic2PlayerGame(*manager, true); // Set for test environment
    
    // Drain all shells from tanks
    for (auto& tank : Tanks()) {
        for (int i = 0; i < Tank::INITIAL_SHELLS; i++) {
            tank.decrementShells();
        }
        EXPECT_EQ(tank.getRemainingShells(), 0);
    }
    
    // Set remaining steps to exactly 0 (the condition for tie)
    SetRemainingSteps(0);
    SetMaxSteps(1000); // High enough to not trigger max steps condition
    SetCurrentStep(100); // Far from max steps
    
    // Act: Check if game ends when remaining steps reaches 0
    bool over = CallCheckGameOver();
    
    // Assert: Game should end with tie message mentioning exactly 40 steps
    EXPECT_TRUE(over);
    EXPECT_EQ(GetGameResult(), "Tie, both players have zero shells for " + std::to_string(GameManager::DEFAULT_NO_SHELLS_STEPS) + " steps");
}

TEST_F(GameManagerTest, CheckGameOver_ZeroShellsButStillRemaining39Steps) {
    // Arrange: Create tanks for both players with zero shells
    std::vector<std::pair<int, Point>> positions = { {1, Point(0,0)}, {2, Point(1,0)} };
    CreateTanks(*manager, positions);
    CreateTankAlgorithms(*manager);
    
    // Drain all shells from tanks
    for (auto& tank : Tanks()) {
        for (int i = 0; i < Tank::INITIAL_SHELLS; i++) {
            tank.decrementShells();
        }
        EXPECT_EQ(tank.getRemainingShells(), 0);
    }
    
    // Set remaining steps to 1 (still has steps left before 40 step limit)
    SetRemainingSteps(1);
    SetMaxSteps(1000); // High enough to not trigger max steps condition
    SetCurrentStep(100); // Far from max steps
    
    // Act: Check if game continues when still has remaining steps
    bool over = CallCheckGameOver();
    
    // Assert: Game should continue
    EXPECT_FALSE(over);
}

TEST_F(GameManagerTest, SaveResults_WritesAllLogLines) {
    std::string testFile = "test_output.txt";
    // Fill log with known lines
    GetGameLog(*manager).clear();
    GetGameLog(*manager).push_back("Step 1 completed");
    GetGameLog(*manager).push_back("Step 2 completed");
    GetGameLog(*manager).push_back("Game ended after 2 steps");
    GetGameLog(*manager).push_back("Result: Player 1 won with 1 tanks still alive");

    // Call saveResults
    CallSaveResults(testFile);

    // Read file back
    std::ifstream in(testFile);
    ASSERT_TRUE(in.is_open());
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(in, line)) {
        lines.push_back(line);
    }
    in.close();

    // Check all lines
    ASSERT_EQ(lines.size(), 4u);
    EXPECT_EQ(lines[0], "Step 1 completed");
    EXPECT_EQ(lines[1], "Step 2 completed");
    EXPECT_EQ(lines[2], "Game ended after 2 steps");
    EXPECT_EQ(lines[3], "Result: Player 1 won with 1 tanks still alive");

    // Clean up
    std::remove(testFile.c_str());
}

TEST_F(GameManagerTest, Run_DoesNotExceedMaximumSteps_Integration) {
    // Arrange: set up a board with two tanks and a small max steps
    std::vector<std::pair<int, Point>> positions = { {1, Point(0,0)}, {2, Point(1,0)} };
    CreateTanks(*manager, positions);
    CreateTankAlgorithms(*manager);
    SetMaxSteps(5); // Set a small maximum for the test

    // Act
    manager->run();

    // Assert
    EXPECT_LE(GetCurrentStep(), 6);
    EXPECT_TRUE(GetGameResult().find("Tie, reached max steps") != std::string::npos);
}

TEST_F(GameManagerTest, SetOutputFilePath_FilenameOnly) {
    CallSetOutputFilePath(*manager, "board.txt");
    EXPECT_EQ(GetOutputFilePath(*manager), "output_board.txt");
}

TEST_F(GameManagerTest, SetOutputFilePath_WithRelativeDirectory) {
    CallSetOutputFilePath(*manager, "examples/board.txt");
    EXPECT_EQ(GetOutputFilePath(*manager), "examples/output_board.txt");
}

TEST_F(GameManagerTest, SetOutputFilePath_WithAbsoluteDirectory) {
    CallSetOutputFilePath(*manager, "/home/user/boards/board.txt");
    EXPECT_EQ(GetOutputFilePath(*manager), "/home/user/boards/output_board.txt");
}

TEST_F(GameManagerTest, SetOutputFilePath_WithNestedDirectories) {
    CallSetOutputFilePath(*manager, "test/data/boards/complex_board.txt");
    EXPECT_EQ(GetOutputFilePath(*manager), "test/data/boards/output_complex_board.txt");
}

TEST_F(GameManagerTest, SetOutputFilePath_WithDifferentExtension) {
    CallSetOutputFilePath(*manager, "my_board.board");
    EXPECT_EQ(GetOutputFilePath(*manager), "output_my_board.board");
}

TEST_F(GameManagerTest, SetOutputFilePath_WithNoExtension) {
    CallSetOutputFilePath(*manager, "examples/boardfile");
    EXPECT_EQ(GetOutputFilePath(*manager), "examples/output_boardfile");
}

TEST_F(GameManagerTest, SetOutputFilePath_EmptyString) {
    // Test edge case with empty string
    CallSetOutputFilePath(*manager, "");
    EXPECT_EQ(GetOutputFilePath(*manager), "output_");
}

TEST_F(GameManagerTest, SetOutputFilePath_CurrentDirectory) {
    CallSetOutputFilePath(*manager, "./board.txt");
    EXPECT_EQ(GetOutputFilePath(*manager), "./output_board.txt");
}

TEST_F(GameManagerTest, Run_TieAfter40StepsWithZeroShells_Integration) {
    // Arrange: Create tanks that will shoot at walls
    std::vector<std::pair<int, Point>> positions = { {1, Point(19,19)}, {2, Point(0,0)} };
    
    // Set up a large board with walls in front of each tank
    GetBoard(*manager) = GameBoard(20, 20);
    
    // Place walls in front of tank 1 (facing right by default for player 1)
    // Tank 1 is at (5,5) facing Left, so place walls to the left
    for (int i = 0; i < 16; ++i) {
        GetBoard(*manager).setCellType(18-i, 19, GameBoard::CellType::Wall);
    }
    
    // Place walls in front of tank 2 (facing right by default for player 2) 
    // Tank 2 is at (10,10) facing Right, so place walls to the right
    for (int i = 0; i < 16; ++i) {
        GetBoard(*manager).setCellType(1+i, 0, GameBoard::CellType::Wall);
    }
    
    CreateTanks(*manager, positions);
    CreateTankAlgorithms(*manager);
    SetIsClassic2PlayerGame(*manager, true); // Set for test environment
    
    // Verify tanks have full shells initially
    for (const auto& tank : Tanks()) {
        EXPECT_EQ(tank.getRemainingShells(), Tank::INITIAL_SHELLS);
    }
    
    // Set up mock algorithms to always shoot
    auto& controllers = GetTankControllers(*manager);
    auto* mockAlgo1 = dynamic_cast<MockAlgorithm*>(controllers[0].algorithm.get());
    auto* mockAlgo2 = dynamic_cast<MockAlgorithm*>(controllers[1].algorithm.get());
    ASSERT_NE(mockAlgo1, nullptr);
    ASSERT_NE(mockAlgo2, nullptr);
    
    // Both tanks always shoot
    mockAlgo1->setConstantAction(ActionRequest::Shoot);
    mockAlgo2->setConstantAction(ActionRequest::Shoot);
    
    // Calculate expected steps: 1 (initial shoot) + (cooldown+1) * (num_shells - 1) + 40
    int expectedShootingSteps = 1 + (Tank::SHOOT_COOLDOWN + 1) * (Tank::INITIAL_SHELLS - 1);
    int expectedTotalSteps = expectedShootingSteps + GameManager::DEFAULT_NO_SHELLS_STEPS;
    
    SetMaxSteps(expectedTotalSteps + 10); // Give some buffer to avoid max steps limit
    
    // Act: Run the game
    manager->run();
    
    // Assert: Game should end with zero shells tie condition
    EXPECT_TRUE(GetGameResult().find("Tie, both players have zero shells for " + std::to_string(GameManager::DEFAULT_NO_SHELLS_STEPS) + " steps") != std::string::npos);
    
    // Verify the timing is exactly correct
    EXPECT_EQ(GetCurrentStep(), expectedTotalSteps);
    
    // Verify tanks are still alive (not destroyed)
    int player1Alive = 0;
    int player2Alive = 0;
    for (const auto& tank : Tanks()) {
        if (!tank.isDestroyed()) {
            if (tank.getPlayerId() == 1) player1Alive++;
            else if (tank.getPlayerId() == 2) player2Alive++;
        }
    }
    EXPECT_EQ(player1Alive, 1);
    EXPECT_EQ(player2Alive, 1);
    
    // Verify both tanks have 0 shells
    for (const auto& tank : Tanks()) {
        EXPECT_EQ(tank.getRemainingShells(), 0);
    }
}

// Tests for isClassic2PlayerGame detection
TEST_F(GameManagerTest, IsClassic2PlayerGame_DetectionTests) {
    // Test 1: Classic 2-player game (players 1 and 2 only)
    {
        // Create a simple board with only players 1 and 2
        std::string boardContent = R"(Test Board
MaxSteps = 100
NumShells = 10
Rows = 3
Cols = 3
1 2
   
   )";
        
        // Write to temporary file
        std::ofstream tempFile("test_2player.txt");
        tempFile << boardContent;
        tempFile.close();
        
        GameManager testManager(playerFactory, algoFactory);
        EXPECT_TRUE(testManager.readBoard("test_2player.txt"));
        EXPECT_TRUE(GetIsClassic2PlayerGame(testManager));
        
        // Clean up
        std::remove("test_2player.txt");
    }
    
    // Test 2: Multi-player game (players 1, 2, and 3)
    {
        std::string boardContent = R"(Multi Player Board
MaxSteps = 100
NumShells = 10
Rows = 3
Cols = 3
1 2
3  
   )";
        
        std::ofstream tempFile("test_3player.txt");
        tempFile << boardContent;
        tempFile.close();
        
        GameManager testManager(playerFactory, algoFactory);
        EXPECT_TRUE(testManager.readBoard("test_3player.txt"));
        EXPECT_FALSE(GetIsClassic2PlayerGame(testManager));
        
        std::remove("test_3player.txt");
    }
    
    // Test 3: Only player 1 (should be false)
    {
        std::string boardContent = R"(Single Player Board
MaxSteps = 100
NumShells = 10
Rows = 3
Cols = 3
1  
   
   )";
        
        std::ofstream tempFile("test_1player.txt");
        tempFile << boardContent;
        tempFile.close();
        
        GameManager testManager(playerFactory, algoFactory);
        EXPECT_TRUE(testManager.readBoard("test_1player.txt"));
        EXPECT_FALSE(GetIsClassic2PlayerGame(testManager));
        
        std::remove("test_1player.txt");
    }
    
    // Test 4: Players 2 and 3 (missing player 1, should be false)
    {
        std::string boardContent = R"(No Player 1 Board
MaxSteps = 100
NumShells = 10
Rows = 3
Cols = 3
2 3
   
   )";
        
        std::ofstream tempFile("test_2and3player.txt");
        tempFile << boardContent;
        tempFile.close();
        
        GameManager testManager(playerFactory, algoFactory);
        EXPECT_TRUE(testManager.readBoard("test_2and3player.txt"));
        EXPECT_FALSE(GetIsClassic2PlayerGame(testManager));
        
        std::remove("test_2and3player.txt");
    }
    
    // Test 5: 9-player game (should be false)
    {
        std::string boardContent = R"(Max Players Board
MaxSteps = 100
NumShells = 10
Rows = 3
Cols = 9
123456789)";
        
        std::ofstream tempFile("test_9player.txt");
        tempFile << boardContent;
        tempFile.close();
        
        GameManager testManager(playerFactory, algoFactory);
        EXPECT_TRUE(testManager.readBoard("test_9player.txt"));
        EXPECT_FALSE(GetIsClassic2PlayerGame(testManager));
        
        std::remove("test_9player.txt");
    }
    
    // Test 6: Default initialization (should be false)
    {
        GameManager testManager(playerFactory, algoFactory);
        EXPECT_FALSE(GetIsClassic2PlayerGame(testManager));
    }
}