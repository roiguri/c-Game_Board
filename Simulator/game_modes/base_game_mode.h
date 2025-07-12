#pragma once

#include <string>
#include <memory>
#include <vector>
#include "common/GameResult.h"
#include "utils/file_loader.h"
#include "utils/library_manager.h"
#include "utils/library_validator.h"

/**
 * @brief Abstract base class for all game mode implementations
 * 
 * This class implements the Template Method Pattern, defining the overall algorithm
 * for game mode execution while allowing subclasses to implement specific behaviors.
 * It consolidates common functionality that was previously duplicated across
 * BasicGameMode, ComparativeRunner, and CompetitiveRunner.
 * 
 * The template method execute() defines the execution flow:
 * 1. Common setup
 * 2. Load libraries (subclass-specific)
 * 3. Load map (subclass-specific) 
 * 4. Execute game logic (subclass-specific)
 * 5. Display results (subclass-specific)
 * 6. Common cleanup
 * 
 * Common utilities like file enumeration, output generation, and library validation
 * are provided through utility classes for consistent behavior across game modes.
 */
class BaseGameMode {
public:
    /**
     * @brief Base parameters structure that all game modes must support
     */
    struct BaseParameters {
        std::string mapFile;
        bool verbose = true;
        
        // Virtual destructor for proper cleanup in derived parameter types
        virtual ~BaseParameters() = default;
    };

    /**
     * @brief Construct a new BaseGameMode object
     * 
     * Initializes the library manager and sets up initial state.
     */
    BaseGameMode();

    /**
     * @brief Virtual destructor for proper cleanup
     * 
     * Ensures proper cleanup of resources when derived objects are destroyed.
     */
    virtual ~BaseGameMode();

    /**
     * @brief Template method that defines the game mode execution algorithm
     * 
     * This method implements the Template Method Pattern, defining the overall
     * execution flow while delegating specific steps to abstract methods that
     * subclasses must implement. Provides consistent error handling and cleanup
     * across all game modes.
     * 
     * @param params Base parameters (subclasses may use derived parameter types)
     * @return GameResult containing execution outcome and game state
     */
    GameResult execute(const BaseParameters& params);

protected:
    // Abstract methods that subclasses must implement

    /**
     * @brief Load libraries required for this game mode
     * 
     * Subclasses implement specific library loading logic based on their needs.
     * Should use the provided utility classes for consistent behavior.
     * 
     * @param params Game mode parameters
     * @return True if all required libraries were loaded successfully
     */
    virtual bool loadLibraries(const BaseParameters& params) = 0;

    /**
     * @brief Load and validate the game map
     * 
     * Subclasses implement map loading logic using the FileLoader utility.
     * 
     * @param mapFile Path to the map file
     * @return True if map was loaded and validated successfully
     */
    virtual bool loadMap(const std::string& mapFile) = 0;

    /**
     * @brief Execute the game mode-specific logic
     * 
     * Subclasses implement their specific game execution logic.
     * 
     * @param params Game mode parameters
     * @return GameResult containing execution outcome
     */
    virtual GameResult executeGameLogic(const BaseParameters& params) = 0;

    /**
     * @brief Display results in game mode-specific format
     * 
     * Subclasses implement their specific result display logic.
     * 
     * @param result Game result to display
     */
    virtual void displayResults(const GameResult& result) = 0;

    // Common utility methods available to subclasses

    /**
     * @brief Clean up resources and reset state
     * 
     * Performs common cleanup operations including library unloading
     * and resource deallocation. Can be overridden by subclasses for
     * additional cleanup, but should call the base implementation.
     */
    virtual void cleanup();

    /**
     * @brief Generate timestamp for output files
     * 
     * Delegates to OutputGenerator utility for consistent timestamp format.
     * 
     * @param includeMilliseconds Whether to include milliseconds in timestamp
     * @return Formatted timestamp string
     */
    std::string generateTimestamp(bool includeMilliseconds = false) const;

    /**
     * @brief Enumerate files by extension in directory
     * 
     * Delegates to FileEnumerator utility for consistent file enumeration.
     * 
     * @param directory Directory to scan
     * @param extension File extension to filter (e.g., ".so", ".txt")
     * @return Vector of file paths matching the extension
     */
    std::vector<std::string> enumerateFiles(const std::string& directory, const std::string& extension) const;

    /**
     * @brief Validate library file before loading
     * 
     * Delegates to LibraryValidator utility for consistent validation.
     * 
     * @param libraryPath Path to library file
     * @param type Library type ("GameManager" or "Algorithm")
     * @return LibraryValidator::LibraryInfo with validation results
     */
    LibraryValidator::LibraryInfo validateLibrary(const std::string& libraryPath, const std::string& type) const;

    /**
     * @brief Handle error with consistent logging and cleanup
     * 
     * Provides consistent error handling across all game modes.
     * 
     * @param error Error message to log
     */
    void handleError(const std::string& error);

    /**
     * @brief Create a standardized error result
     * 
     * Creates a GameResult indicating failure with consistent format.
     * 
     * @return GameResult representing an error condition
     */
    GameResult createErrorResult() const;

    /**
     * @brief Write content to file with fallback to console
     * 
     * Delegates to OutputGenerator utility for consistent file output.
     * 
     * @param filePath Path to output file
     * @param content Content to write
     * @param fallbackToConsole Whether to fallback to console on file error
     * @return True if content was written successfully
     */
    bool writeToFile(const std::string& filePath, const std::string& content, bool fallbackToConsole = true) const;

    /**
     * @brief Save validation warnings/errors to input_errors.txt file
     * 
     * Creates input_errors.txt file with one error per line. If file creation
     * fails, the error is logged but execution continues without crashing.
     * 
     * @param errors Vector of error/warning messages to save
     * @return True if file was created successfully, false otherwise
     */
    bool saveErrorsToFile(const std::vector<std::string>& errors);

    // Protected member variables accessible to subclasses

    /**
     * @brief Reference to the singleton LibraryManager for loading and unloading shared libraries
     */
    LibraryManager& m_libraryManager;

private:
    // No private methods needed - all functionality is in subclasses
};