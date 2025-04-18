#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "algo/chase_algorithm.h"
#include "game_board.h"
#include "tank.h"
#include "shell.h"

class ChaseAlgorithmTest : public ::testing::Test {
protected:
    void SetUp() override {
        algorithm = new ChaseAlgorithm();
    }
    
    void TearDown() override {
        delete algorithm;
    }
    
    ChaseAlgorithm* algorithm;
    
    GameBoard create5X5TestBoard(const std::vector<std::string>& boardLines) {
        GameBoard board(5, 5);
        std::vector<std::string> errors;
        board.initialize(boardLines, errors);
        return board;
    }

    std::vector<Point> testGetValidNeighbors(const Point& current, const GameBoard& gameBoard) {
        return algorithm->getValidNeighbors(current, gameBoard);
    }

    std::vector<Point> testReconstructPath(const std::map<Point, Point>& came_from, const Point& start, const Point& end) {
        return algorithm->reconstructPath(came_from, start, end);
    }

    std::vector<Point> testCalculatePathBFS(const Point& start, const Point& end, const GameBoard& gameBoard) {
        return algorithm->calculatePathBFS(start, end, gameBoard);
    }
};

using ::testing::UnorderedElementsAreArray;

TEST_F(ChaseAlgorithmTest, Constructor) {
    EXPECT_NE(algorithm, nullptr);
}

TEST_F(ChaseAlgorithmTest, GetNextAction_ReturnNoneNotImlemented) {
    std::vector<std::string> boardLines = {
        "#####",
        "#1 2#",
        "#   #",
        "#   #",
        "#####"
    };
    GameBoard board = create5X5TestBoard(boardLines);
    
    Tank myTank(1, Point(1, 1), Direction::Right);
    Tank enemyTank(2, Point(3, 1), Direction::Left);
    std::vector<Shell> shells;
  
    EXPECT_EQ(algorithm->getNextAction(board, myTank, enemyTank, shells), Action::None);
}

TEST_F(ChaseAlgorithmTest, GetValidNeighbors_EmptyBoardCenter) {
  GameBoard board = create5X5TestBoard({
      "     ",
      "     ",
      "     ",
      "     ",
      "     "
  });
  Point center(2, 2);

  std::vector<Point> expectedNeighbors = {
      Point(2, 1), Point(3, 1), Point(3, 2), Point(3, 3),
      Point(2, 3), Point(1, 3), Point(1, 2), Point(1, 1)
  };

  std::vector<Point> actualNeighbors = testGetValidNeighbors(center, board);

  EXPECT_THAT(actualNeighbors, UnorderedElementsAreArray(expectedNeighbors));
}

TEST_F(ChaseAlgorithmTest, GetValidNeighbors_EmptyBoardCornerWrap) {
  // Using the fixture's helper
   GameBoard board = create5X5TestBoard({
      "     ",
      "     ",
      "     ",
      "     ",
      "     "
  });
  Point corner(0, 0);

  std::vector<Point> expectedNeighbors = {
      Point(0, 4), Point(1, 4), Point(1, 0), Point(1, 1),
      Point(0, 1), Point(4, 1), Point(4, 0), Point(4, 4)
  };

  std::vector<Point> actualNeighbors = testGetValidNeighbors(corner, board);
  EXPECT_THAT(actualNeighbors, UnorderedElementsAreArray(expectedNeighbors));
}

TEST_F(ChaseAlgorithmTest, GetValidNeighbors_EmptyBoardEdgeWrap) {
   GameBoard board = create5X5TestBoard({
      "     ",
      "     ",
      "     ",
      "     ",
      "     "
  });
  Point edge(2, 0);

  std::vector<Point> expectedNeighbors = {
       Point(2, 4), Point(3, 4), Point(3, 0), Point(3, 1),
       Point(2, 1), Point(1, 1), Point(1, 0), Point(1, 4)
  };

  std::vector<Point> actualNeighbors = testGetValidNeighbors(edge, board);
  EXPECT_THAT(actualNeighbors, UnorderedElementsAreArray(expectedNeighbors));
}


TEST_F(ChaseAlgorithmTest, GetValidNeighbors_WallBlock) {
  GameBoard board = create5X5TestBoard({
      "     ",
      "  #  ", // Wall at (2, 1)
      "   # ", // Wall at (3, 2)
      "     ",
      "     "
  });
  Point center(2, 2);

  std::vector<Point> expectedNeighbors = {
      /*Point(2, 1),*/ Point(3, 1), /*Point(3, 2),*/ Point(3, 3), // Up and Right are blocked
      Point(2, 3), Point(1, 3), Point(1, 2), Point(1, 1)
  };

  std::vector<Point> actualNeighbors = testGetValidNeighbors(center, board);
  EXPECT_THAT(actualNeighbors, UnorderedElementsAreArray(expectedNeighbors));
}

TEST_F(ChaseAlgorithmTest, GetValidNeighbors_MineBlock) {
   GameBoard board = create5X5TestBoard({
      "     ",
      "     ",
      " @   ", // Mine at (1, 2)
      "  @  ", // Mine at (2, 3)
      "     "
  });
  Point center(2, 2);

  std::vector<Point> expectedNeighbors = {
      Point(2, 1), Point(3, 1), Point(3, 2), Point(3, 3),
      /*Point(2, 3),*/ Point(1, 3), /*Point(1, 2),*/ Point(1, 1) // Down and Left are blocked
  };

  std::vector<Point> actualNeighbors = testGetValidNeighbors(center, board);
  EXPECT_THAT(actualNeighbors, UnorderedElementsAreArray(expectedNeighbors));
}

TEST_F(ChaseAlgorithmTest, GetValidNeighbors_MixedBlock) {
  GameBoard board = create5X5TestBoard({
      "     ",
      " #   ", // Wall at (1, 1)
      "     ",
      "   @ ", // Mine at (3, 3)
      "     "
  });
  Point center(2, 2);

   std::vector<Point> expectedNeighbors = {
      Point(2, 1), Point(3, 1), Point(3, 2), /*Point(3, 3),*/ // Mine DR blocked
      Point(2, 3), Point(1, 3), Point(1, 2) /* Point(1, 1) */ // Wall UL blocked
  };

  std::vector<Point> actualNeighbors = testGetValidNeighbors(center, board);
  EXPECT_THAT(actualNeighbors, UnorderedElementsAreArray(expectedNeighbors));
}

TEST_F(ChaseAlgorithmTest, GetValidNeighbors_WrapRightEdge) {
  GameBoard board = create5X5TestBoard({ // width = 5
      "     ",
      "     ",
      "     ",
      "     ",
      "     "
  });
  Point rightEdge(4, 2);

  std::vector<Point> expectedNeighbors = {
      Point(4, 1), Point(0, 1), Point(0, 2), Point(0, 3), // Wrapped points have x=0
      Point(4, 3), Point(3, 3), Point(3, 2), Point(3, 1)
  };

  std::vector<Point> actualNeighbors = testGetValidNeighbors(rightEdge, board);
  EXPECT_THAT(actualNeighbors, UnorderedElementsAreArray(expectedNeighbors));
}

TEST_F(ChaseAlgorithmTest, GetValidNeighbors_WrapBottomEdge) {
  GameBoard board = create5X5TestBoard({ // height = 5
      "     ",
      "     ",
      "     ",
      "     ",
      "     "
  });
  Point bottomEdge(2, 4);

  std::vector<Point> expectedNeighbors = {
      Point(2, 3), Point(3, 3), Point(3, 4), Point(3, 0), // Wrapped points have y=0
      Point(2, 0), Point(1, 0), Point(1, 4), Point(1, 3)
  };

  std::vector<Point> actualNeighbors = testGetValidNeighbors(bottomEdge, board);
  EXPECT_THAT(actualNeighbors, UnorderedElementsAreArray(expectedNeighbors));
}

TEST_F(ChaseAlgorithmTest, GetValidNeighbors_WrapCornerBottomRight) {
  GameBoard board = create5X5TestBoard({ // width=5, height=5
      "     ",
      "     ",
      "     ",
      "     ",
      "     "
  });
  Point corner(4, 4); // Bottom right

  std::vector<Point> expectedNeighbors = {
      Point(4, 3), Point(0, 3), Point(0, 4), Point(0, 0), // 0,0 is wrap of 5,5 -> DR
      Point(4, 0), Point(3, 0), Point(3, 4), Point(3, 3)
  };

  std::vector<Point> actualNeighbors = testGetValidNeighbors(corner, board);
  EXPECT_THAT(actualNeighbors, UnorderedElementsAreArray(expectedNeighbors));
}

TEST_F(ChaseAlgorithmTest, ReconstructPath_StraightLine) {
  Point start(1, 1);
  Point end(1, 4);
  std::map<Point, Point> came_from;

  // Path: (1,1) -> (1,2) -> (1,3) -> (1,4)
  came_from[Point(1, 2)] = Point(1, 1);
  came_from[Point(1, 3)] = Point(1, 2);
  came_from[Point(1, 4)] = Point(1, 3);

  std::vector<Point> expectedPath = { Point(1, 2), Point(1, 3), Point(1, 4) };
  std::vector<Point> actualPath = testReconstructPath(came_from, start, end);

  EXPECT_EQ(actualPath, expectedPath);
}

TEST_F(ChaseAlgorithmTest, ReconstructPath_DiagonalLine) {
  Point start(0, 0);
  Point end(3, 3);
  std::map<Point, Point> came_from;

  // Path: (0,0) -> (1,1) -> (2,2) -> (3,3)
  came_from[Point(1, 1)] = Point(0, 0);
  came_from[Point(2, 2)] = Point(1, 1);
  came_from[Point(3, 3)] = Point(2, 2);

  std::vector<Point> expectedPath = { Point(1, 1), Point(2, 2), Point(3, 3) };
  std::vector<Point> actualPath = testReconstructPath(came_from, start, end);
  EXPECT_EQ(actualPath, expectedPath);
}

TEST_F(ChaseAlgorithmTest, ReconstructPath_LShape) {
  Point start(1, 1);
  Point end(3, 3);
  std::map<Point, Point> came_from;

  // Path: (1,1) -> (1,2) -> (1,3) -> (2,3) -> (3,3)
  came_from[Point(1, 2)] = Point(1, 1);
  came_from[Point(1, 3)] = Point(1, 2);
  came_from[Point(2, 3)] = Point(1, 3);
  came_from[Point(3, 3)] = Point(2, 3);

  std::vector<Point> expectedPath = { Point(1, 2), Point(1, 3), Point(2, 3), Point(3, 3) };
  std::vector<Point> actualPath = testReconstructPath(came_from, start, end);

  EXPECT_EQ(actualPath, expectedPath);
}

TEST_F(ChaseAlgorithmTest, ReconstructPath_StartEqualsEnd) {
  Point start(2, 2);
  Point end(2, 2);
  std::map<Point, Point> came_from;


  std::vector<Point> expectedPath = {};
  std::vector<Point> actualPath = testReconstructPath(came_from, start, end);

  EXPECT_EQ(actualPath, expectedPath);
}

TEST_F(ChaseAlgorithmTest, ReconstructPath_EndNotReached) {
  Point start(1, 1);
  Point end(5, 5);
  std::map<Point, Point> came_from;

  // Path: (1,1) -> (1,2) -> (2,1)
  came_from[Point(1, 2)] = Point(1, 1);
  came_from[Point(2, 1)] = Point(1, 1);

  std::vector<Point> expectedPath = {};
  std::vector<Point> actualPath = testReconstructPath(came_from, start, end);

  EXPECT_EQ(actualPath, expectedPath);
}

TEST_F(ChaseAlgorithmTest, ReconstructPath_EmptyCameFrom) {
  Point start(1, 1);
  Point end(5, 5);
  std::map<Point, Point> came_from;

  std::vector<Point> expectedPath = {};
  std::vector<Point> actualPath = testReconstructPath(came_from, start, end);

  EXPECT_EQ(actualPath, expectedPath);
}

TEST_F(ChaseAlgorithmTest, CalculatePathBFS_EmptyBoardDirectPath) {
  GameBoard board = create5X5TestBoard({
      "     ",
      "     ",
      "     ",
      "     ",
      "     "
  });
  Point start(1, 1);
  Point end(1, 3);

  std::vector<Point> expectedPath = { Point(2, 2), Point(1, 3) };
  size_t expectedLength = 2;

  std::vector<Point> actualPath = testCalculatePathBFS(start, end, board);
  EXPECT_EQ(actualPath.size(), expectedLength);
  EXPECT_EQ(actualPath, expectedPath);
}

TEST_F(ChaseAlgorithmTest, CalculatePathBFS_EmptyBoardDiagonalPath) {
  GameBoard board = create5X5TestBoard({
      "     ",
      "     ",
      "     ",
      "     ",
      "     "
  });
  Point start(1, 1);
  Point end(3, 3);
  // Shortest path is diagonal
  std::vector<Point> expectedPath = { Point(2, 2), Point(3, 3) }; // Path: (1,1)->(2,2)->(3,3)
  size_t expectedLength = 2;

  std::vector<Point> actualPath = testCalculatePathBFS(start, end, board);
  EXPECT_EQ(actualPath.size(), expectedLength);
  EXPECT_EQ(actualPath, expectedPath);
}


TEST_F(ChaseAlgorithmTest, CalculatePathBFS_PathAroundWall) {
  GameBoard board = create5X5TestBoard({
      "     ",
      " S   ", // S = Start (1,1)
      "  #  ", // Wall y=2
      "   E ", // E = End (3,3)
      "     "
  });
  Point start(1, 1);
  Point end(3, 3);

  // Path: (1,0)->(2,4)->(3,3)
  std::vector<Point> expectedPath = {
      Point(1,0), Point(2,4), Point(3, 3)
  };
  size_t expectedLength = 3; // Shortest path length

  std::vector<Point> actualPath = testCalculatePathBFS(start, end, board);
  EXPECT_EQ(actualPath.size(), expectedLength);
  EXPECT_EQ(actualPath, expectedPath);
}

TEST_F(ChaseAlgorithmTest, CalculatePathBFS_PathAroundMine) {
   GameBoard board = create5X5TestBoard({
      "     ",
      " S   ", // S = Start (1,1)
      "  @  ", // Mine at (2,2)
      "   E ", // E = End (3,3)
      "     "
  });
  Point start(1, 1);
  Point end(3, 3);

  std::vector<Point> expectedPath = { Point(1,0), Point(2,4), Point(3, 3) };
  size_t expectedLength = 3;

  std::vector<Point> actualPath = testCalculatePathBFS(start, end, board);
  EXPECT_EQ(actualPath.size(), expectedLength);
  EXPECT_EQ(actualPath, expectedPath);
}


TEST_F(ChaseAlgorithmTest, CalculatePathBFS_TargetUnreachableWalled) {
  GameBoard board = create5X5TestBoard({
      "#####",
      "#S#E#", // S=Start(1,1), E=End(3,1), separated by wall
      "#####",
      "     ",
      "     "
  });
  Point start(1, 1);
  Point end(3, 1);

  std::vector<Point> expectedPath = {};
  size_t expectedLength = 0;

  std::vector<Point> actualPath = testCalculatePathBFS(start, end, board);
  EXPECT_EQ(actualPath.size(), expectedLength);
  EXPECT_EQ(actualPath, expectedPath);
}

TEST_F(ChaseAlgorithmTest, CalculatePathBFS_TargetUnreachableMined) {
   GameBoard board = create5X5TestBoard({
      " @@@ ",
      " @S@ ", // S=Start(2,1)
      " @@@ ", 
      " @E@ ", // E=End(2,3) surrounded by mines
      " @@@ "
  });
  Point start(2, 1);
  Point end(2, 3);

  std::vector<Point> expectedPath = {};
  size_t expectedLength = 0;

  std::vector<Point> actualPath = testCalculatePathBFS(start, end, board);
  EXPECT_EQ(actualPath.size(), expectedLength);
  EXPECT_EQ(actualPath, expectedPath);
}


TEST_F(ChaseAlgorithmTest, CalculatePathBFS_StartEqualsEnd) {
  GameBoard board = create5X5TestBoard({
      "     ",
      "     ",
      "     ",
      "     ",
      "     "
  });
  Point start(2, 2);
  Point end(2, 2);
  std::vector<Point> expectedPath = {};
  size_t expectedLength = 0;

  std::vector<Point> actualPath = testCalculatePathBFS(start, end, board);
  EXPECT_EQ(actualPath.size(), expectedLength);
  EXPECT_EQ(actualPath, expectedPath);
}

TEST_F(ChaseAlgorithmTest, CalculatePathBFS_WrapPathX) {
  GameBoard board = create5X5TestBoard({ // Width 5
      "     ",
      "E   S", // E at (0,1), S at (4,1). Wrap is shorter
      "     ",
      "     ",
      "     "
  });
  Point start(4, 1);
  Point end(0, 1);
  // Wrap path: (4,1) -> (0,1). Length 1.
  std::vector<Point> expectedPath = { Point(0, 1) };
  size_t expectedLength = 1;

  std::vector<Point> actualPath = testCalculatePathBFS(start, end, board);
  EXPECT_EQ(actualPath.size(), expectedLength);
  EXPECT_EQ(actualPath, expectedPath);
}

TEST_F(ChaseAlgorithmTest, CalculatePathBFS_WrapPathY) {
   GameBoard board = create5X5TestBoard({ // Height 5
      "  E  ", // E at (2,0)
      "     ", 
      "     ",
      "     ",
      "  S  "  // S at (2,4). Wrap is shorter
  });
  Point start(2, 4);
  Point end(2, 0);
  // Wrap path: (2,0)->(2,1). Length 1.
   std::vector<Point> expectedPath = { Point(2, 0) };
   size_t expectedLength = 1;

  std::vector<Point> actualPath = testCalculatePathBFS(start, end, board);
  EXPECT_EQ(actualPath.size(), expectedLength);
  EXPECT_EQ(actualPath, expectedPath);
}