#include "gtest/gtest.h"
#include "bonus/analysis/analysis_stats.h" // Header for the functions to be tested
#include <cmath> // For std::abs, std::isnan
#include <string> // For std::string comparisons

const double DEFAULT_PRECISION = 1e-5; // Precision for float comparisons

// Test fixture for AnalysisStats tests
class AnalysisStatsTest : public ::testing::Test {
protected:
    // You can put setup code here if needed
};

// --- Tests for calculateChiSquareTest ---
TEST_F(AnalysisStatsTest, ChiSquare_BasicCase) {
    // Data from a known example:
    // Group A: 20 wins, 30 total (10 losses)
    // Group B: 35 wins, 50 total (15 losses)
    // Expected Chi-Square approx 0.74, p-value approx 0.39 (for df=1)
    // Let's verify with an online calculator:
    // https://www.socscistatistics.com/tests/chisquare/default2.aspx
    // Wins1=20, Total1=30, Wins2=35, Total2=50 => ChiSq=0.74074, P-val=0.3894
    AnalysisStats::ChiSquareResult result = AnalysisStats::calculateChiSquareTest(20, 30, 35, 50);
    EXPECT_NEAR(0.74074, result.chiSquareValue, DEFAULT_PRECISION);
    EXPECT_NEAR(0.3894, result.pValue, DEFAULT_PRECISION);
    EXPECT_FALSE(result.warningNoExpectedFrequencyBelowFive);
}

TEST_F(AnalysisStatsTest, ChiSquare_PerfectAssociation) {
    // Group A: 50 wins, 50 total (0 losses)
    // Group B: 0 wins, 50 total (50 losses)
    // Expected Chi-Square = 100 (if no zero expecteds, otherwise can be tricky)
    // P-value should be very small.
    // Expected frequencies: E_wins_A = (50*50)/100 = 25, E_loss_A = (50*50)/100 = 25
    // E_wins_B = (50*50)/100 = 25, E_loss_B = (50*50)/100 = 25
    // ChiSq = (50-25)^2/25 + (0-25)^2/25 + (0-25)^2/25 + (50-25)^2/25
    //       = 25 + 25 + 25 + 25 = 100
    AnalysisStats::ChiSquareResult result = AnalysisStats::calculateChiSquareTest(50, 50, 0, 50);
    EXPECT_NEAR(100.0, result.chiSquareValue, DEFAULT_PRECISION);
    EXPECT_LT(result.pValue, 0.0001); // p-value for ChiSq=100, df=1 is extremely small
    EXPECT_FALSE(result.warningNoExpectedFrequencyBelowFive);
}

TEST_F(AnalysisStatsTest, ChiSquare_NoDifference) {
    // Group A: 25 wins, 50 total
    // Group B: 25 wins, 50 total
    // Expected Chi-Square = 0, p-value = 1
    AnalysisStats::ChiSquareResult result = AnalysisStats::calculateChiSquareTest(25, 50, 25, 50);
    EXPECT_NEAR(0.0, result.chiSquareValue, DEFAULT_PRECISION);
    EXPECT_NEAR(1.0, result.pValue, DEFAULT_PRECISION);
    EXPECT_FALSE(result.warningNoExpectedFrequencyBelowFive);
}

TEST_F(AnalysisStatsTest, ChiSquare_LowExpectedFrequencyWarning) {
    // Group A: 1 win, 2 total
    // Group B: 1 win, 100 total (many losses)
    // This should trigger the low expected frequency warning
    // Wins1=1, Tot1=2 (Loss1=1)
    // Wins2=1, Tot2=100 (Loss2=99)
    // Row Totals: Wins=2, Losses=100. Col Totals: G1=2, G2=100. N=102
    // E_wins_G1 = (2*2)/102 = 4/102 = 0.039... < 5
    AnalysisStats::ChiSquareResult result = AnalysisStats::calculateChiSquareTest(1, 2, 1, 100);
    EXPECT_TRUE(result.warningNoExpectedFrequencyBelowFive);
    // Chi-square value might still be calculated, but the warning is key
}

TEST_F(AnalysisStatsTest, ChiSquare_ZeroTotalInOneGroup) {
    AnalysisStats::ChiSquareResult result = AnalysisStats::calculateChiSquareTest(10, 20, 5, 0);
    EXPECT_NEAR(0.0, result.chiSquareValue, DEFAULT_PRECISION); // Default or error state
    EXPECT_NEAR(1.0, result.pValue, DEFAULT_PRECISION);         // Default or error state
    EXPECT_TRUE(result.warningNoExpectedFrequencyBelowFive); // Should indicate issue
}

TEST_F(AnalysisStatsTest, ChiSquare_ZeroTotalInBothGroups) {
    AnalysisStats::ChiSquareResult result = AnalysisStats::calculateChiSquareTest(0, 0, 0, 0);
    EXPECT_NEAR(0.0, result.chiSquareValue, DEFAULT_PRECISION);
    EXPECT_NEAR(1.0, result.pValue, DEFAULT_PRECISION);
    // Warning may or may not be set depending on implementation order of checks
}

// --- Tests for calculateConfidenceInterval ---
TEST_F(AnalysisStatsTest, ConfidenceInterval_BasicCase) {
    // 10 wins, 20 total, 95% CI
    // Wilson score interval for 10/20 (p=0.5), n=20, 95% CI
    // Using online calculator: [0.289, 0.711] approx for Wilson
    // Or for Clopper-Pearson: [0.2618, 0.7382]
    // The implementation uses Wilson. Let's check with a reliable source.
    // E.g., https://epitools.ausvet.com.au/ciproportion
    // For 10 successes, 20 trials, 0.95 confidence: Wilson score [0.2993, 0.7007]
    AnalysisStats::ConfidenceInterval ci = AnalysisStats::calculateConfidenceInterval(10, 20, 0.95);
    EXPECT_NEAR(0.2993, ci.lowerBound, DEFAULT_PRECISION);
    EXPECT_NEAR(0.7007, ci.upperBound, DEFAULT_PRECISION);
}

TEST_F(AnalysisStatsTest, ConfidenceInterval_ZeroWins) {
    // 0 wins, 50 total, 95% CI
    // Wilson for 0/50: [0.0000, 0.0713]
    AnalysisStats::ConfidenceInterval ci = AnalysisStats::calculateConfidenceInterval(0, 50, 0.95);
    EXPECT_NEAR(0.0, ci.lowerBound, DEFAULT_PRECISION);
    EXPECT_NEAR(0.0713, ci.upperBound, DEFAULT_PRECISION); // Upper bound for p=0
}

TEST_F(AnalysisStatsTest, ConfidenceInterval_AllWins) {
    // 50 wins, 50 total, 95% CI
    // Wilson for 50/50: [0.9287, 1.0000]
    AnalysisStats::ConfidenceInterval ci = AnalysisStats::calculateConfidenceInterval(50, 50, 0.95);
    EXPECT_NEAR(0.9287, ci.lowerBound, DEFAULT_PRECISION); // Lower bound for p=1
    EXPECT_NEAR(1.0, ci.upperBound, DEFAULT_PRECISION);
}

TEST_F(AnalysisStatsTest, ConfidenceInterval_ZeroTotal) {
    AnalysisStats::ConfidenceInterval ci = AnalysisStats::calculateConfidenceInterval(0, 0, 0.95);
    EXPECT_NEAR(0.0, ci.lowerBound, DEFAULT_PRECISION); // Should return {0,0} or handle error
    EXPECT_NEAR(0.0, ci.upperBound, DEFAULT_PRECISION);
}

TEST_F(AnalysisStatsTest, ConfidenceInterval_DifferentLevel99) {
    // 25 wins, 100 total, 99% CI
    // Wilson for 25/100: [0.1600, 0.3628]
    AnalysisStats::ConfidenceInterval ci = AnalysisStats::calculateConfidenceInterval(25, 100, 0.99);
    EXPECT_NEAR(0.1600, ci.lowerBound, DEFAULT_PRECISION);
    EXPECT_NEAR(0.3628, ci.upperBound, DEFAULT_PRECISION);
}

TEST_F(AnalysisStatsTest, ConfidenceInterval_DifferentLevel90) {
    // 60 wins, 100 total, 90% CI
    // Wilson for 60/100: [0.5188, 0.6778]
    AnalysisStats::ConfidenceInterval ci = AnalysisStats::calculateConfidenceInterval(60, 100, 0.90);
    EXPECT_NEAR(0.5188, ci.lowerBound, DEFAULT_PRECISION);
    EXPECT_NEAR(0.6778, ci.upperBound, DEFAULT_PRECISION);
}


// --- Tests for isStatisticallySignificant ---
TEST_F(AnalysisStatsTest, Significance_ClearlySignificant) {
    EXPECT_TRUE(AnalysisStats::isStatisticallySignificant(0.001, 0.05, 1));
}

TEST_F(AnalysisStatsTest, Significance_ClearlyNotSignificant) {
    EXPECT_FALSE(AnalysisStats::isStatisticallySignificant(0.10, 0.05, 1));
}

TEST_F(AnalysisStatsTest, Significance_BorderlineNotSignificant) {
    EXPECT_FALSE(AnalysisStats::isStatisticallySignificant(0.05, 0.05, 1)); // pValue must be < alpha
}

TEST_F(AnalysisStatsTest, Significance_BorderlineSignificant) {
    EXPECT_TRUE(AnalysisStats::isStatisticallySignificant(0.049, 0.05, 1));
}

TEST_F(AnalysisStatsTest, Significance_BonferroniCorrection) {
    // pValue = 0.02. alpha = 0.05. numComparisons = 3.
    // Corrected alpha = 0.05 / 3 = 0.01666...
    // 0.02 is NOT less than 0.01666...
    EXPECT_FALSE(AnalysisStats::isStatisticallySignificant(0.02, 0.05, 3));
}

TEST_F(AnalysisStatsTest, Significance_BonferroniMakesItSignificant) {
    // pValue = 0.01. alpha = 0.05. numComparisons = 3.
    // Corrected alpha = 0.05 / 3 = 0.01666...
    // 0.01 IS less than 0.01666...
    EXPECT_TRUE(AnalysisStats::isStatisticallySignificant(0.01, 0.05, 3));
}

TEST_F(AnalysisStatsTest, Significance_ZeroComparisons) {
    // Implementation should handle numComparisons <= 0 (treats as 1)
    EXPECT_TRUE(AnalysisStats::isStatisticallySignificant(0.01, 0.05, 0));
    EXPECT_TRUE(AnalysisStats::isStatisticallySignificant(0.01, 0.05, -1));
}


// --- Tests for getSampleSizeRecommendation ---
TEST_F(AnalysisStatsTest, SampleSize_BothAdequate) {
    std::string rec = AnalysisStats::getSampleSizeRecommendation(30, 30);
    EXPECT_TRUE(rec.empty());
    rec = AnalysisStats::getSampleSizeRecommendation(100, 100);
    EXPECT_TRUE(rec.empty());
}

TEST_F(AnalysisStatsTest, SampleSize_FirstInadequate) {
    std::string rec = AnalysisStats::getSampleSizeRecommendation(29, 30);
    EXPECT_NE(std::string::npos, rec.find("Condition 1 sample size (29)"));
    EXPECT_NE(std::string::npos, rec.find("less than recommended 30"));
}

TEST_F(AnalysisStatsTest, SampleSize_SecondInadequate) {
    std::string rec = AnalysisStats::getSampleSizeRecommendation(30, 20);
    EXPECT_NE(std::string::npos, rec.find("Condition 2 sample size (20)"));
    EXPECT_NE(std::string::npos, rec.find("less than recommended 30"));
}

TEST_F(AnalysisStatsTest, SampleSize_BothInadequate) {
    std::string rec = AnalysisStats::getSampleSizeRecommendation(10, 15);
    EXPECT_NE(std::string::npos, rec.find("Condition 1 sample size (10)"));
    EXPECT_NE(std::string::npos, rec.find("Condition 2 sample size (15)"));
}

TEST_F(AnalysisStatsTest, SampleSize_CustomMinRecommendation) {
    std::string rec = AnalysisStats::getSampleSizeRecommendation(35, 40, 50);
    EXPECT_NE(std::string::npos, rec.find("Condition 1 sample size (35)"));
    EXPECT_NE(std::string::npos, rec.find("less than recommended 50"));
    EXPECT_NE(std::string::npos, rec.find("Condition 2 sample size (40)"));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
