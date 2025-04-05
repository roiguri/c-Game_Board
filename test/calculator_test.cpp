#include "gtest/gtest.h"
#include "calculator.h"

TEST(CalculatorTest, Addition) {
    Calculator calc;
    ASSERT_EQ(5, calc.add(2, 3));
    ASSERT_EQ(-1, calc.add(-3, 2));
}

TEST(CalculatorTest, Subtraction) {
    Calculator calc;
    ASSERT_EQ(1, calc.subtract(3, 2));
    ASSERT_EQ(-5, calc.subtract(-2, 3));
}