#pragma once

#include <filesystem>
#include <set>
#include <string>
#include <vector>

namespace GameManager_098765432_123456789 {

/**
 * @brief Helper class for managing file system operations in GameManager tests
 * 
 * Provides functionality to track, verify, and clean up test files,
 * particularly for testing verbose output functionality.
 */
class FileTestHelper {
public:
    /**
     * @brief Constructor - records initial file state
     */
    FileTestHelper();
    
    /**
     * @brief Destructor - cleans up test files
     */
    ~FileTestHelper();
    
    /**
     * @brief Get list of new game_*.txt files created since construction
     * @return Vector of filenames that were created during the test
     */
    std::vector<std::string> getNewGameFiles() const;
    
    /**
     * @brief Clean up any test files created during the test
     */
    void cleanupTestFiles();
    
    /**
     * @brief Check if a string starts with a given prefix
     * @param str The string to check
     * @param prefix The prefix to look for
     * @return true if str starts with prefix
     */
    static bool startsWith(const std::string& str, const std::string& prefix);
    
    /**
     * @brief Check if a string ends with a given suffix
     * @param str The string to check
     * @param suffix The suffix to look for
     * @return true if str ends with suffix
     */
    static bool endsWith(const std::string& str, const std::string& suffix);
    
    /**
     * @brief Clean filename by replacing invalid filesystem characters with underscores
     * @param name The filename to clean
     * @return Cleaned filename safe for filesystem use
     */
    static std::string cleanFilename(const std::string& name);

private:
    /**
     * @brief Record the initial state of game_*.txt files in current directory
     */
    void recordInitialFiles();
    
    std::set<std::string> m_initialFiles;
};

} // namespace GameManager_098765432_123456789