#include <gtest/gtest.h>
#include "Calculator.h"

// Test fixture for Calculator tests
class CalculatorTest : public ::testing::Test {
protected:
    // Set up for each test
    void SetUp() override {
        // Nothing special needed for this simple test
    }
    
    // Clean up after each test
    void TearDown() override {
        // Nothing special needed for this simple test
    }
    
    // Object we'll use in tests
    Calculator calc;
};

// Test addition
TEST_F(CalculatorTest, Addition) {
    EXPECT_EQ(calc.add(1, 1), 2);
    EXPECT_EQ(calc.add(-1, 1), 0);
    EXPECT_EQ(calc.add(-1, -1), -2);
    EXPECT_EQ(calc.add(1000, 1000), 2000);
}

// Test subtraction
TEST_F(CalculatorTest, Subtraction) {
    EXPECT_EQ(calc.subtract(1, 1), 0);
    EXPECT_EQ(calc.subtract(5, 3), 2);
    EXPECT_EQ(calc.subtract(1, 5), -4);
    EXPECT_EQ(calc.subtract(-5, -3), -2);
}

// Test multiplication
TEST_F(CalculatorTest, Multiplication) {
    EXPECT_EQ(calc.multiply(1, 1), 1);
    EXPECT_EQ(calc.multiply(2, 3), 6);
    EXPECT_EQ(calc.multiply(-2, 3), -6);
    EXPECT_EQ(calc.multiply(-2, -3), 6);
    EXPECT_EQ(calc.multiply(0, 5), 0);
}

// Test division
TEST_F(CalculatorTest, Division) {
    EXPECT_EQ(calc.divide(6, 3), 2);
    EXPECT_EQ(calc.divide(5, 2), 2); // Integer division
    EXPECT_EQ(calc.divide(-6, 3), -2);
    EXPECT_EQ(calc.divide(-6, -3), 2);
    
    // Test division by zero exception
    EXPECT_THROW(calc.divide(5, 0), std::invalid_argument);
}

// Test for prime number detection
TEST_F(CalculatorTest, PrimeNumber) {
    // Test known primes
    EXPECT_TRUE(calc.isPrime(2));
    EXPECT_TRUE(calc.isPrime(3));
    EXPECT_TRUE(calc.isPrime(5));
    EXPECT_TRUE(calc.isPrime(7));
    EXPECT_TRUE(calc.isPrime(11));
    EXPECT_TRUE(calc.isPrime(13));
    EXPECT_TRUE(calc.isPrime(17));
    EXPECT_TRUE(calc.isPrime(19));
    EXPECT_TRUE(calc.isPrime(23));
    
    // Test known non-primes
    EXPECT_FALSE(calc.isPrime(0));
    EXPECT_FALSE(calc.isPrime(1));
    EXPECT_FALSE(calc.isPrime(4));
    EXPECT_FALSE(calc.isPrime(6));
    EXPECT_FALSE(calc.isPrime(8));
    EXPECT_FALSE(calc.isPrime(9));
    EXPECT_FALSE(calc.isPrime(10));
    EXPECT_FALSE(calc.isPrime(12));
    EXPECT_FALSE(calc.isPrime(15));
    
    // Test some larger numbers
    EXPECT_TRUE(calc.isPrime(97));
    EXPECT_FALSE(calc.isPrime(100));
}