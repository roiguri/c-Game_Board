#pragma once

#include <vector>
#include <string>

/**
 * @brief Centralized error collection for validation and processing errors
 * 
 * This class provides a centralized mechanism for collecting various types of
 * errors and warnings during game setup and validation. It supports different
 * error categories and provides methods for error file generation.
 * 
 * The class is designed to be extensible for future error types while maintaining
 * a clean separation of concerns between error collection and error handling.
 */
class ErrorCollector {
public:
    /**
     * @brief Construct a new ErrorCollector object
     * 
     * Initializes an empty error collection.
     */
    ErrorCollector();

    /**
     * @brief Destroy the ErrorCollector object
     * 
     * Cleans up any resources used by the error collector.
     */
    ~ErrorCollector();

    /**
     * @brief Add a fatal map error that excludes the map from execution
     * 
     * Adds an error that prevents a map from being used in the game/tournament.
     * These are typically validation failures like missing tanks or file corruption.
     * 
     * @param mapName Name of the map file that generated the error
     * @param error Description of the fatal error condition
     */
    void addMapError(const std::string& mapName, const std::string& error);

    /**
     * @brief Add a recoverable map warning that allows map inclusion
     * 
     * Adds a warning for issues that can be recovered from or corrected automatically.
     * The map can still be used despite these warnings.
     * 
     * @param mapName Name of the map file that generated the warning
     * @param warning Description of the warning condition
     */
    void addMapWarning(const std::string& mapName, const std::string& warning);

    /**
     * @brief Add multiple map validation warnings
     * 
     * Convenience method for adding multiple warnings from the same map.
     * Each warning will be formatted with the map name for clear identification.
     * 
     * @param mapName Name of the map file that generated the warnings
     * @param warnings Vector of warning descriptions
     */
    void addMapWarnings(const std::string& mapName, const std::vector<std::string>& warnings);

    /**
     * @brief Add a GameManager loading error
     * 
     * Adds an error for GameManager libraries that failed to load or register.
     * These errors are logged as input errors when enough GameManagers are still loaded.
     * 
     * @param gameManagerPath Path to the GameManager .so file that failed
     * @param error Description of the loading/registration failure
     */
    void addGameManagerError(const std::string& gameManagerPath, const std::string& error);

    /**
     * @brief Add an Algorithm loading error
     * 
     * Adds an error for Algorithm libraries that failed to load or register.
     * These errors are logged as input errors when enough algorithms are still loaded.
     * 
     * @param algorithmPath Path to the Algorithm .so file that failed
     * @param error Description of the loading/registration failure
     */
    void addAlgorithmError(const std::string& algorithmPath, const std::string& error);

    /**
     * @brief Check if any errors or warnings have been collected
     * 
     * @return True if there are any errors or warnings in the collection
     */
    bool hasErrors() const;

    /**
     * @brief Check if any fatal map errors have been collected
     * 
     * @return True if there are any fatal errors that exclude maps
     */
    bool hasMapErrors() const;

    /**
     * @brief Check if any recoverable map warnings have been collected
     * 
     * @return True if there are any warnings that don't exclude maps
     */
    bool hasMapWarnings() const;

    /**
     * @brief Get count of fatal map errors
     * 
     * @return Number of fatal errors that exclude maps from execution
     */
    size_t getErrorCount() const;

    /**
     * @brief Get count of GameManager loading errors
     * 
     * @return Number of GameManager libraries that failed to load
     */
    size_t getGameManagerErrorCount() const;

    /**
     * @brief Get count of Algorithm loading errors
     * 
     * @return Number of Algorithm libraries that failed to load
     */
    size_t getAlgorithmErrorCount() const;

    /**
     * @brief Get all collected errors
     * 
     * Returns a read-only reference to all collected errors and warnings.
     * Useful for inspection without modifying the collection.
     * 
     * @return Const reference to vector of all error messages
     */
    const std::vector<std::string>& getAllErrors() const;

    /**
     * @brief Save all collected errors to input_errors.txt file
     * 
     * Creates input_errors.txt file in the current working directory with
     * one error per line. If file creation fails, the error is logged but
     * execution continues without crashing.
     * 
     * @return True if file was created successfully, false otherwise
     */
    bool saveToFile() const;

    /**
     * @brief Clear all collected errors
     * 
     * Removes all collected errors from the collection. Useful for resetting
     * the collector between different processing sessions.
     */
    void clear();

private:
    /**
     * @brief Format a map error with standardized prefix
     * 
     * Creates a standardized format for map errors that includes the map name
     * and error description in a consistent format.
     * 
     * @param mapName Name of the map file
     * @param error Error description
     * @return Formatted error string
     */
    std::string formatMapError(const std::string& mapName, const std::string& error) const;

    /**
     * @brief Format a map warning with standardized prefix
     * 
     * Creates a standardized format for map warnings that includes the map name
     * and warning description in a consistent format.
     * 
     * @param mapName Name of the map file
     * @param warning Warning description
     * @return Formatted warning string
     */
    std::string formatMapWarning(const std::string& mapName, const std::string& warning) const;

    /**
     * @brief Format a GameManager error with standardized prefix
     * 
     * Creates a standardized format for GameManager loading errors.
     * 
     * @param gameManagerPath Path to the GameManager file
     * @param error Error description
     * @return Formatted error string
     */
    std::string formatGameManagerError(const std::string& gameManagerPath, const std::string& error) const;

    /**
     * @brief Format an Algorithm error with standardized prefix
     * 
     * Creates a standardized format for Algorithm loading errors.
     * 
     * @param algorithmPath Path to the Algorithm file
     * @param error Error description
     * @return Formatted error string
     */
    std::string formatAlgorithmError(const std::string& algorithmPath, const std::string& error) const;

    /**
     * @brief Extract filename from full path for error formatting
     * 
     * @param filePath Full path to file
     * @return Filename without path
     */
    std::string extractFilename(const std::string& filePath) const;

    /**
     * @brief Collection of all errors and warnings
     * 
     * Stores all collected errors and warnings in the order they were added.
     * Each string represents a formatted error message ready for output.
     * Uses a unified storage approach with error type distinction in formatting.
     */
    std::vector<std::string> m_errors;
    
    /**
     * @brief Count of fatal errors that exclude maps
     * 
     * Tracks the number of fatal errors separately from warnings to support
     * error/warning distinction without requiring separate storage vectors.
     */
    size_t m_errorCount;
    
    /**
     * @brief Count of GameManager loading errors
     * 
     * Tracks the number of GameManager libraries that failed to load or register.
     */
    size_t m_gameManagerErrorCount;
    
    /**
     * @brief Count of Algorithm loading errors
     * 
     * Tracks the number of Algorithm libraries that failed to load or register.
     */
    size_t m_algorithmErrorCount;
};