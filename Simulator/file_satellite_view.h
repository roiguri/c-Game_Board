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

private:
    std::vector<std::string> m_boardData;
    size_t m_rows;
    size_t m_cols;
};