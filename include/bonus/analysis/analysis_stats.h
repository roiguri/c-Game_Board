#pragma once

#include <string>
#include <vector> // Required for potential future use or if struct returns vector
#include <utility> // Required for std::pair if used for intervals

namespace AnalysisStats {

    // Structure to hold the results of a Chi-Square test
    struct ChiSquareResult {
        double chiSquareValue;
        double pValue;
        bool warningNoExpectedFrequencyBelowFive; // Standard warning for Chi-Square
    };

    // Structure for confidence interval
    struct ConfidenceInterval {
        double lowerBound;
        double upperBound;
    };

    /**
     * @brief Calculates the Chi-Square statistic and p-value for a 2x2 contingency table.
     * The table is typically:
     *           Condition 1 | Condition 2
     * Wins      wins1       | wins2
     * Losses    losses1     | losses2
     * where losses1 = total1 - wins1, and losses2 = total2 - wins2.
     * @param wins1 Wins for condition 1.
     * @param total1 Total observations for condition 1.
     * @param wins2 Wins for condition 2.
     * @param total2 Total observations for condition 2.
     * @return ChiSquareResult struct containing the chi-square value, p-value, and warning flag.
     */
    ChiSquareResult calculateChiSquareTest(int wins1, int total1, int wins2, int total2);

    /**
     * @brief Calculates the confidence interval for a binomial proportion.
     * @param wins Number of successful outcomes.
     * @param total Total number of trials.
     * @param confidenceLevel The desired confidence level (e.g., 0.95 for 95%).
     * @return ConfidenceInterval struct with lower and upper bounds.
     *         Returns {0,0} if total is 0 or confidenceLevel is invalid.
     */
    ConfidenceInterval calculateConfidenceInterval(int wins, int total, double confidenceLevel = 0.95);

    /**
     * @brief Determines if a result is statistically significant after applying Bonferroni correction.
     * @param pValue The original p-value from a statistical test.
     * @param alpha The significance level (e.g., 0.05).
     * @param numComparisons The number of comparisons being made (for Bonferroni correction).
     * @return True if the result is statistically significant, false otherwise.
     */
    bool isStatisticallySignificant(double pValue, double alpha, int numComparisons);

    /**
     * @brief Provides a recommendation based on sample sizes.
     * @param sampleSize1 Total observations for condition 1.
     * @param sampleSize2 Total observations for condition 2.
     * @param minRecommendedSize Minimum recommended size per condition (default 30).
     * @return A string with recommendations, or an empty string if sizes are adequate.
     */
    std::string getSampleSizeRecommendation(int sampleSize1, int sampleSize2, int minRecommendedSize = 30);

} // namespace AnalysisStats
