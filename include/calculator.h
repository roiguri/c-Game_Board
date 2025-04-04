#ifndef CALCULATOR_H
#define CALCULATOR_H

/**
 * A simple calculator class to demonstrate testing with Google Test.
 */
class Calculator {
public:
    /**
     * Adds two numbers together.
     * 
     * @param a First number
     * @param b Second number
     * @return Sum of the two numbers
     */
    int add(int a, int b);
    
    /**
     * Subtracts the second number from the first.
     * 
     * @param a First number
     * @param b Number to subtract
     * @return Result of subtraction
     */
    int subtract(int a, int b);
    
    /**
     * Multiplies two numbers.
     * 
     * @param a First number
     * @param b Second number
     * @return Product of the two numbers
     */
    int multiply(int a, int b);
    
    /**
     * Divides the first number by the second.
     * 
     * @param a Dividend
     * @param b Divisor
     * @return Result of division
     * @throws std::invalid_argument if b is zero
     */
    int divide(int a, int b);
    
    /**
     * Checks if a number is prime.
     * 
     * @param n Number to check
     * @return true if the number is prime, false otherwise
     */
    bool isPrime(int n);
};

#endif // CALCULATOR_H