#include "Simulator/file_satellite_view.h"

FileSatelliteView::FileSatelliteView(const std::vector<std::string>& boardData, size_t rows, size_t cols)
    : m_boardData(boardData), m_rows(rows), m_cols(cols) {
}

char FileSatelliteView::getObjectAt(size_t x, size_t y) const {
    if (x >= m_cols || y >= m_rows) {
        return '&';  // Out of bounds character
    }
    
    // Within board dimensions but no data available
    if (y >= m_boardData.size()) {
        return ' ';
    }
    const std::string& row = m_boardData[y];
    if (x >= row.length()) {
        return ' ';
    }

    // Validate character against allowed SatelliteView characters
    char ch = row[x];
    if (ch == '#' ||    // Wall
        ch == '@' ||    // Mine
        ch == ' ' ||    // Empty
        ch == '*' ||    // Shell
        ch == '%' ||    // Current tank // TODO: % should be empty space
        (ch >= '1' && ch <= '9')) {  // Tank players 1-9
        return ch;
    }
    return ' ';
}