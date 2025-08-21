#pragma once

#include <string>
#include <vector>
#include <filesystem>

/**
 * @brief Utility class for file enumeration operations across game modes
 */
class FileEnumerator {
public:
    /**
     * @brief Enumerate all .so (shared object) files in the specified directory
     * 
     * Scans the given directory for files with .so extension and returns them
     * in sorted order for consistent results across platforms.
     * 
     * @param directory Directory path to scan for .so files
     * @return Vector of .so file paths sorted alphabetically
     */
    static std::vector<std::string> enumerateSoFiles(const std::string& directory);

    /**
     * @brief Enumerate all map files (.txt) in the specified directory
     * 
     * Scans the given directory for files with .txt extension which are
     * used as game map files in the system.
     * 
     * @param directory Directory path to scan for map files
     * @return Vector of .txt map file paths sorted alphabetically
     */
    static std::vector<std::string> enumerateMapFiles(const std::string& directory);

    /**
     * @brief Check if a directory path is valid and accessible
     * 
     * Validates that the given path exists, is a directory, and is accessible.
     * Provides detailed error information for troubleshooting.
     * 
     * @param directory Directory path to validate
     * @return True if directory is valid and accessible, false otherwise
     */
    static bool isValidDirectory(const std::string& directory);

    /**
     * @brief Get the last error message from directory operations
     * 
     * Returns human-readable error message from the most recent operation
     * that failed. Useful for debugging and user feedback.
     * 
     * @return String containing the last error message, empty if no error
     */
    static const std::string& getLastError();

private:
    /**
     * @brief Enumerate files with specified extension in directory
     * 
     * Internal helper method that handles the common file enumeration logic
     * with optional extension filtering.
     * 
     * @param directory Directory to scan
     * @param extension File extension to filter (empty string for all files)
     * @param sorted Whether to sort results alphabetically
     * @return Vector of file paths matching criteria
     */
    static std::vector<std::string> enumerateFiles(
        const std::string& directory, 
        const std::string& extension = "",
        bool sorted = true
    );

    /**
     * @brief Set the last error message for error reporting
     * 
     * @param error Error message to store
     */
    static void setLastError(const std::string& error);

    static thread_local std::string s_lastError;
};