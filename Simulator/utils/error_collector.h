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
     * @brief Add map validation warnings
     * 
     * Adds multiple warnings related to map processing/validation. Each warning
     * will be formatted with the map name for clear identification in error files.
     * 
     * @param mapName Name of the map file that generated the warnings
     * @param warnings Vector of warning descriptions
     */
    void addMapWarnings(const std::string& mapName, const std::vector<std::string>& warnings);

    // Future extension points for other error types
    // void addLibraryError(const std::string& error);
    // void addAlgorithmError(const std::string& error);
    // void addConfigurationError(const std::string& error);

    /**
     * @brief Check if any errors have been collected
     * 
     * @return True if there are any errors/warnings in the collection
     */
    bool hasErrors() const;

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
     * @brief Collection of all errors and warnings
     * 
     * Stores all collected errors in the order they were added. Each string
     * represents a formatted error message ready for output.
     */
    std::vector<std::string> m_errors;
};