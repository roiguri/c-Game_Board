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

// Test fixture for GameManager
class GameManagerTest : public ::testing::Test {
protected:
    std::unique_ptr<GameManager> manager;

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
        manager.m_player1 = std::move(player);
    }
    static void LogAction(GameManager& manager) {
        manager.logAction();
    }
    static std::vector<std::string>& GetGameLog(GameManager& manager) {
        return manager.m_gameLog;
    }

    void SetUp() override {
        auto playerFactory = std::make_unique<MockPlayerFactory>();
        auto algoFactory = std::make_unique<MockAlgorithmFactory>();
        manager = std::make_unique<GameManager>(std::move(playerFactory), std::move(algoFactory));

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
    void SetMaxSteps(int maxSteps) {
        manager->m_maximum_steps = maxSteps;
    }

    void CallSaveResults(const std::string& file) {
        manager->saveResults(file);
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

// ---- processStep tests ----

// 1. One tank moves forward
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

// 2. One tank shoots
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

// 3. Shell hits tank, tank destroyed and log killed
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

// 4. Tank on cooldown, shoot ignored
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

// 5. Tank killed this step, logs killed this step
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

// 6. Already dead tank, logs killed
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

// 7. Mixed actions, logs all scenarios
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

// 8. All shells destroyed, removes shells
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

// 9. MoveBackward only moves on 3rd step
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

// 10. Shoot cooldown resets after 4 steps
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

// 11. Cannot shoot more than max shells in a row
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

// ---- checkGameOver tests ----

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
    for (auto& t : Tanks()) t.destroy();
    bool over = CallCheckGameOver();
    EXPECT_TRUE(over);
    EXPECT_EQ(GetGameResult(), "Tie, both players have zero tanks");
}

TEST_F(GameManagerTest, CheckGameOver_TieMaxSteps) {
    std::vector<std::pair<int, Point>> positions = { {1, Point(0,0)}, {2, Point(1,0)} };
    CreateTanks(*manager, positions);
    CreateTankAlgorithms(*manager);
    SetMaxSteps(5);
    SetCurrentStep(6);
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
