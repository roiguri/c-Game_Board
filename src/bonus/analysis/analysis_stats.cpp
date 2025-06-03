#include "bonus/analysis/analysis_stats.h"
#include <cmath>        // For std::sqrt, std::pow, std::abs, std::log, std::erfc
#include <limits>       // For std::numeric_limits
#include <sstream>      // For std::ostringstream
#include <stdexcept>    // For std::invalid_argument (optional, for p-value if needed)
#include <algorithm>    // For std::min, std::max (used in Wilson score interval)

// For p-value calculation from chi-square, we might need a library or an approximation.
// Using a common approximation for chi-square CDF (Cumulative Distribution Function)
// or relying on a simpler approach if an external library like Boost is not available.
// For a 1-degree-of-freedom chi-square test, the p-value can be derived from the
// normal distribution's CDF: p = 2 * (1 - CDF_normal(sqrt(chiSquareValue))).
// Or, more directly, p = 1 - CDF_chisq(chiSquareValue, df=1).
// Let's use a standard library function if available, otherwise an approximation.
// std::erfc (complementary error function) is related to the normal CDF.
// CDF_normal(x) = 0.5 * erfc(-x / sqrt(2))

namespace AnalysisStats {

    // Internal helper to calculate p-value from Z-score (sqrt of chi-square for df=1)
    // This gives a two-tailed p-value.
    static double pValueFromZ(double z) {
        return std::erfc(std::abs(z) / std::sqrt(2.0));
    }

    // More general p-value for chi-square (approximation for df=1 from gamma function)
    // This is essentially equivalent to pValueFromZ for df=1.
    // For df > 1, a more complex gamma function or library (like Boost) would be needed.
    // Since we expect df=1 for 2x2 tables, this is acceptable.
    static double chiSquarePValueDF1(double chiSquareValue) {
        if (chiSquareValue < 0) return 1.0; // Or throw error
        // P-value for chi-square with df=1 is P(X > chiSquareValue)
        // This is 2 * (1 - NormalCDF(sqrt(chiSquareValue)))
        // Or directly using erfc: erfc(sqrt(chiSquareValue / 2.0))
        return std::erfc(std::sqrt(chiSquareValue / 2.0));
    }


    ChiSquareResult calculateChiSquareTest(int wins1, int total1, int wins2, int total2) {
        ChiSquareResult result = {0.0, 1.0, false}; // Default to no significance

        if (total1 == 0 || total2 == 0) {
            // Cannot perform test if any group has zero total observations
            result.warningNoExpectedFrequencyBelowFive = true; // Or a different type of warning/error
            return result;
        }

        int losses1 = total1 - wins1;
        int losses2 = total2 - wins2;

        if (wins1 < 0 || losses1 < 0 || wins2 < 0 || losses2 < 0) {
            // Invalid input (e.g., wins > total)
            // Consider throwing an error or returning a specific status
            return result;
        }

        // Contingency table:
        //         Cond1 | Cond2 | Row Total
        // Wins    a     | b     | a+b
        // Losses  c     | d     | c+d
        // Col Tot a+c   | b+d   | N = a+b+c+d
        double a = static_cast<double>(wins1);
        double b = static_cast<double>(wins2);
        double c = static_cast<double>(losses1);
        double d = static_cast<double>(losses2);

        double N = a + b + c + d;
        if (N == 0) {
            return result;
        }

        // Calculate expected frequencies
        // E_ij = (RowTotal_i * ColTotal_j) / N
        double e_a = ((a + b) * (a + c)) / N;
        double e_b = ((a + b) * (b + d)) / N;
        double e_c = ((c + d) * (a + c)) / N;
        double e_d = ((c + d) * (b + d)) / N;

        // Check for expected frequencies < 5 (standard warning for chi-square)
        if (e_a < 5.0 || e_b < 5.0 || e_c < 5.0 || e_d < 5.0) {
            result.warningNoExpectedFrequencyBelowFive = true;
        }

        // Check for any expected frequency being zero, which makes chi-square undefined
        if (e_a == 0.0 || e_b == 0.0 || e_c == 0.0 || e_d == 0.0) {
            // If observed is also 0 for that cell, term is 0.
            // If observed is non-zero, term is infinite.
            // For simplicity, if an expected value is zero, the test might not be appropriate
            // or needs Yates' correction (though Yates' is often debated).
            // We'll proceed, but this highlights a limitation.
            // A more robust implementation might use Fisher's Exact Test for small/zero expecteds.
        }

        // Chi-square statistic: Sum of (Observed - Expected)^2 / Expected
        double chiSquare = 0.0;
        if (e_a > 0) chiSquare += std::pow(a - e_a, 2) / e_a;
        if (e_b > 0) chiSquare += std::pow(b - e_b, 2) / e_b;
        if (e_c > 0) chiSquare += std::pow(c - e_c, 2) / e_c;
        if (e_d > 0) chiSquare += std::pow(d - e_d, 2) / e_d;

        result.chiSquareValue = chiSquare;
        // Degrees of freedom for a 2x2 table = (rows-1)*(cols-1) = 1*1 = 1
        result.pValue = chiSquarePValueDF1(chiSquare);

        return result;
    }

    // Wilson score interval for binomial proportion
    ConfidenceInterval calculateConfidenceInterval(int wins, int total, double confidenceLevel) {
        if (total == 0 || confidenceLevel <= 0.0 || confidenceLevel >= 1.0) {
            return {0.0, 0.0};
        }

        double p_hat = static_cast<double>(wins) / total;
        double z_alpha_half; // Z-score for (1 - confidenceLevel) / 2

        // Simplified Z-score lookup for common confidence levels
        // For arbitrary confidenceLevel, use inverse normal CDF (probit function)
        // std::erfcinv could be used: z = sqrt(2) * erfcinv(alpha_for_z_calc)
        // where alpha_for_z_calc for two-tailed is (1-confidenceLevel)
        if (confidenceLevel == 0.95) z_alpha_half = 1.95996; // Approx 1.96
        else if (confidenceLevel == 0.99) z_alpha_half = 2.57583; // Approx 2.58
        else if (confidenceLevel == 0.90) z_alpha_half = 1.64485; // Approx 1.645
        else {
            // Fallback for other levels using erfc inverse relation:
            // For a given tail probability `p_tail = (1 - confidenceLevel) / 2`,
            // z = sqrt(2) * erfcinv(2 * p_tail)
            double p_tail = (1.0 - confidenceLevel) / 2.0;
            if (p_tail <= 0.0) p_tail = std::numeric_limits<double>::epsilon(); // Avoid log(0) or erfcinv(0)
            // erfcinv is not standard C++11/14. Use approximation or normal_quantile from another source.
            // For simplicity, let's stick to common ones or return {0,0} if not common.
            // This is a limitation without a full stats library.
            // A simple approximation for normal_quantile (inverse normal CDF):
            auto normal_quantile = [](double p) {
                if (p < 0 || p > 1) return 0.0; // Should throw
                if (p == 0.5) return 0.0;

                // Abramowitz and Stegun formula 26.2.23 (approximation)
                double t = std::sqrt(-2.0 * std::log(p < 0.5 ? p : 1.0 - p));
                double c[] = {2.515517, 0.802853, 0.010328};
                double d[] = {1.432788, 0.189269, 0.001308};
                double num = c[0] + t * (c[1] + t * c[2]);
                double den = 1.0 + t * (d[0] + t * (d[1] + t * d[2]));
                double z = t - num / den;
                return (p < 0.5 ? -z : z);
            };
            z_alpha_half = normal_quantile(1.0 - p_tail);
            if (z_alpha_half == 0.0 && confidenceLevel != 0.5) return {0,0,}; // Approx failed or invalid CL
        }

        double n = static_cast<double>(total);
        double z_sq = z_alpha_half * z_alpha_half;

        double center = p_hat + z_sq / (2.0 * n);
        double term = z_alpha_half * std::sqrt((p_hat * (1.0 - p_hat) / n) + (z_sq / (4.0 * n * n)));
        double denominator = 1.0 + z_sq / n;

        double lower = (center - term) / denominator;
        double upper = (center + term) / denominator;

        return {std::max(0.0, lower), std::min(1.0, upper)};
    }


    bool isStatisticallySignificant(double pValue, double alpha, int numComparisons) {
        if (numComparisons <= 0) {
            numComparisons = 1; // Or throw error
        }
        double correctedAlpha = alpha / static_cast<double>(numComparisons);
        return pValue < correctedAlpha;
    }

    std::string getSampleSizeRecommendation(int sampleSize1, int sampleSize2, int minRecommendedSize) {
        std::ostringstream oss;
        bool recommendationNeeded = false;

        if (sampleSize1 < minRecommendedSize) {
            oss << "Condition 1 sample size (" << sampleSize1 << ") is less than recommended " << minRecommendedSize << ". ";
            recommendationNeeded = true;
        }
        if (sampleSize2 < minRecommendedSize) {
            oss << "Condition 2 sample size (" << sampleSize2 << ") is less than recommended " << minRecommendedSize << ". ";
            recommendationNeeded = true;
        }

        if (recommendationNeeded) {
            // General advice for Chi-Square
            oss << "Consider increasing sample sizes for more reliable Chi-Square test results. ";
            oss << "Low sample sizes can also affect confidence interval accuracy. ";
            oss << "Also, ensure no expected cell frequency is less than 5 for Chi-Square validity.";
            return oss.str();
        }
        return ""; // Empty string if sizes are adequate
    }

} // namespace AnalysisStats
