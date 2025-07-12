#pragma once

#include <string>
#include <vector>

#include "common/SatelliteView.h"

/**
 * @brief Simple SatelliteView implementation for raw board data from files
 */
class FileSatelliteView : public SatelliteView {
public:
    FileSatelliteView(const std::vector<std::string>& boardData, size_t rows, size_t cols);
    char getObjectAt(size_t x, size_t y) const override;

    // Validation interface
    bool isValid() const;
    const std::string& getErrorReason() const;
    const std::vector<std::string>& getWarnings() const;

private:
    std::vector<std::string> m_boardData;
    size_t m_rows;
    size_t m_cols;

    // Validation state members
    bool m_isValid;
    std::string m_errorReason;
    std::vector<std::string> m_warnings;

    // Helper methods for validation state management
    void setError(const std::string& errorMessage);
    void addWarning(const std::string& warningMessage);
};