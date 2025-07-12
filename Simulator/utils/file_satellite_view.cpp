#include "file_satellite_view.h"

FileSatelliteView::FileSatelliteView(const std::vector<std::string>& boardData, size_t rows, size_t cols)
    : m_rows(rows), m_cols(cols), m_isValid(true) {
    initializeAndRecoverBoard(boardData);
}

char FileSatelliteView::getObjectAt(size_t x, size_t y) const {
    if (x >= m_cols || y >= m_rows) {
        return '&';  // Out of bounds character
    }
    
    // Within board dimensions but no data available
    if (y >= m_processedBoard.size()) {
        return ' ';
    }
    const std::string& row = m_processedBoard[y];
    if (x >= row.length()) {
        return ' ';
    }

    return row[x];
}

// Validation interface implementation
bool FileSatelliteView::isValid() const {
    return m_isValid;
}

const std::string& FileSatelliteView::getErrorReason() const {
    return m_errorReason;
}

const std::vector<std::string>& FileSatelliteView::getWarnings() const {
    return m_warnings;
}

// Helper methods for validation state management
void FileSatelliteView::setError(const std::string& errorMessage) {
    m_isValid = false;
    m_errorReason = errorMessage;
}

void FileSatelliteView::addWarning(const std::string& warningMessage) {
    m_warnings.push_back(warningMessage);
}

void FileSatelliteView::initializeAndRecoverBoard(const std::vector<std::string>& rawBoardData) {
    // Critical error checks - early exit
    if (m_rows == 0 || m_cols == 0) {
        setError("Board dimensions cannot be zero");
        return;
    }

    // Check for tanks in board data
    bool hasTanks = false;
    for (const auto& row : rawBoardData) {
        for (char ch : row) {
            if (ch >= '1' && ch <= '9') {
                hasTanks = true;
                break;
            }
        }
        if (hasTanks) break;
    }

    if (!hasTanks) {
        setError("Board must contain at least one tank (characters 1-9)");
        return;
    }

    // Initialize processed board
    m_processedBoard.clear();
    m_processedBoard.reserve(m_rows);

    // Process and recover board data
    for (size_t y = 0; y < m_rows; ++y) {
        std::string processedRow;
        
        if (y < rawBoardData.size()) {
            const std::string& rawRow = rawBoardData[y];
            
            // Process existing row
            for (size_t x = 0; x < m_cols; ++x) {
                char ch = ' ';
                
                if (x < rawRow.length()) {
                    char rawChar = rawRow[x];
                    
                    // Validate and clean character
                    if (rawChar == '#' ||    // Wall
                        rawChar == '@' ||    // Mine
                        rawChar == ' ' ||    // Empty
                        rawChar == '*' ||    // Shell
                        (rawChar >= '1' && rawChar <= '9')) {  // Tank players 1-9
                        ch = rawChar;
                    } else {
                        // Invalid character - replace with empty and warn
                        ch = ' ';
                        addWarning("Invalid character '" + std::string(1, rawChar) + 
                                 "' at position (" + std::to_string(x) + "," + std::to_string(y) + 
                                 ") replaced with empty space");
                    }
                } else {
                    // Row too short - pad with empty space and warn
                    if (x == rawRow.length() && !rawRow.empty()) {
                        addWarning("Row " + std::to_string(y) + " is shorter than expected width " + 
                                 std::to_string(m_cols) + ", padded with empty spaces");
                    }
                }
                
                processedRow += ch;
            }
            
            // Check for extra characters in row
            if (rawRow.length() > m_cols) {
                addWarning("Row " + std::to_string(y) + " is longer than expected width " + 
                         std::to_string(m_cols) + ", extra characters ignored");
            }
        } else {
            // Missing row - create empty row and warn
            processedRow = std::string(m_cols, ' ');
            if (y == rawBoardData.size()) {
                addWarning("Board has fewer rows than expected (" + std::to_string(rawBoardData.size()) + 
                         " vs " + std::to_string(m_rows) + "), missing rows filled with empty spaces");
            }
        }
        
        m_processedBoard.push_back(processedRow);
    }

    // Check for extra rows
    if (rawBoardData.size() > m_rows) {
        addWarning("Board has more rows than expected (" + std::to_string(rawBoardData.size()) + 
                 " vs " + std::to_string(m_rows) + "), extra rows ignored");
    }
}