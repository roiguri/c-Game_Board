#include "gtest/gtest.h"
#include "shell.h"
#include "utils/point.h"
#include "utils/direction.h"

// Basic test to verify constructor compiles
TEST(ShellTest, Constructor) {
    ASSERT_NO_THROW(Shell shell(1, Point(5, 5), Direction::Right));
}