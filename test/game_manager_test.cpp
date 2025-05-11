#include <gtest/gtest.h>
#include "game_manager.h"
#include "test/mock_player.h"
#include "test/mock_algorithm.h"
#include "objects/shell.h"
#include "utils/point.h"
#include "utils/direction.h"
#include <memory>

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
