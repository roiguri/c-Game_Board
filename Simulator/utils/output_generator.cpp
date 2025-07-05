#include "output_generator.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>

// Thread-local error storage
thread_local std::string OutputGenerator::s_lastError;

std::string OutputGenerator::generateTimestamp(bool includeMilliseconds) {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S");
    
    if (includeMilliseconds) {
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        ss << "_" << std::setfill('0') << std::setw(3) << ms.count();
    }
    
    return ss.str();
}

bool OutputGenerator::writeToFile(const std::string& filePath, const std::string& content, bool fallbackToConsole) {
    try {
        // Ensure directory exists
        if (!ensureDirectoryExists(filePath)) {
            if (fallbackToConsole) {
                std::cerr << "Error: Cannot create directory for: " << filePath << std::endl;
                std::cerr << "Printing content to console instead:" << std::endl;
                std::cout << content << std::endl;
            }
            return false;
        }
        
        std::ofstream outFile(filePath);
        if (!outFile.is_open()) {
            setLastError("Cannot create output file: " + filePath);
            if (fallbackToConsole) {
                std::cerr << "Error: " << getLastError() << std::endl;
                std::cerr << "Printing content to console instead:" << std::endl;
                std::cout << content << std::endl;
            }
            return false;
        }
        
        outFile << content;
        outFile.close();
        
        if (outFile.fail()) {
            setLastError("Error writing to file: " + filePath);
            return false;
        }
        
        setLastError("");  // Clear any previous error
        return true;
        
    } catch (const std::exception& e) {
        setLastError("Exception writing to file " + filePath + ": " + e.what());
        if (fallbackToConsole) {
            std::cerr << "Error: " << getLastError() << std::endl;
            std::cerr << "Printing content to console instead:" << std::endl;
            std::cout << content << std::endl;
        }
        return false;
    }
}

std::string OutputGenerator::formatGameResult(const GameResult& result) {
    std::stringstream ss;
    
    if (result.winner == 0) {
        ss << "Tie";
    } else {
        ss << "Winner: Player " << result.winner;
    }
    
    // Convert enum reason to string
    ss << " (";
    switch (result.reason) {
        case GameResult::ALL_TANKS_DEAD:
            ss << "all tanks dead";
            break;
        case GameResult::MAX_STEPS:
            ss << "max steps reached";
            break;
        case GameResult::ZERO_SHELLS:
            ss << "zero shells remaining";
            break;
        default:
            ss << "unknown";
            break;
    }
    ss << ")";
    
    return ss.str();
}

std::string OutputGenerator::formatGameState(const SatelliteView& gameState, int rows, int cols) {
    std::string result;
    result.reserve(rows * cols + rows); // Pre-allocate for efficiency
    
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            result += gameState.getObjectAt(x, y);
        }
        if (y < rows - 1) {
            result += '\n';
        }
    }
    
    return result;
}

std::string OutputGenerator::createOutputPath(
    const std::string& baseName,
    const std::string& directory,
    const std::string& extension,
    bool includeMilliseconds) {
    
    std::string timestamp = generateTimestamp(includeMilliseconds);
    std::string filename = baseName + "_" + timestamp + extension;
    
    if (directory.empty()) {
        return filename;
    } else {
        std::filesystem::path dirPath(directory);
        return (dirPath / filename).string();
    }
}

std::ostream& OutputGenerator::writeToStream(
    const std::string& content,
    const std::string& filePath,
    bool fallbackToConsole) {
    
    static std::ofstream fileStream;
    
    // Close any previously opened file
    if (fileStream.is_open()) {
        fileStream.close();
    }
    
    try {
        // Ensure directory exists
        if (!ensureDirectoryExists(filePath)) {
            if (fallbackToConsole) {
                std::cerr << "Error: Cannot create directory for: " << filePath << std::endl;
                std::cerr << "Using console output instead" << std::endl;
                std::cout << content;
            }
            return std::cout;
        }
        
        fileStream.open(filePath);
        if (!fileStream.is_open()) {
            setLastError("Cannot create output file: " + filePath);
            if (fallbackToConsole) {
                std::cerr << "Error: " << getLastError() << std::endl;
                std::cerr << "Using console output instead" << std::endl;
                std::cout << content;
            }
            return std::cout;
        }
        
        // Write content to the file stream
        fileStream << content;
        
        setLastError("");  // Clear any previous error
        return fileStream;
        
    } catch (const std::exception& e) {
        setLastError("Exception opening file " + filePath + ": " + e.what());
        if (fallbackToConsole) {
            std::cerr << "Error: " << getLastError() << std::endl;
            std::cerr << "Using console output instead" << std::endl;
            std::cout << content;
        }
        return std::cout;
    }
}

const std::string& OutputGenerator::getLastError() {
    return s_lastError;
}

void OutputGenerator::setLastError(const std::string& error) {
    s_lastError = error;
}

bool OutputGenerator::ensureDirectoryExists(const std::string& filePath) {
    try {
        std::filesystem::path path(filePath);
        std::filesystem::path parentDir = path.parent_path();
        
        if (parentDir.empty()) {
            return true;  // No directory to create
        }
        
        if (std::filesystem::exists(parentDir)) {
            return std::filesystem::is_directory(parentDir);
        }
        
        return std::filesystem::create_directories(parentDir);
        
    } catch (const std::filesystem::filesystem_error& e) {
        setLastError("Filesystem error creating directory: " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        setLastError("Exception creating directory: " + std::string(e.what()));
        return false;
    }
}