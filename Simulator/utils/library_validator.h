#pragma once

#include <string>
#include <memory>
#include <vector>

/**
 * @brief Utility class for library loading and validation operations across game modes
 * 
 * This class consolidates library validation logic that was previously duplicated
 * across multiple game mode implementations (ComparativeRunner, CompetitiveRunner).
 * It provides methods for validating GameManager and Algorithm libraries with proper
 * error handling and resource management using RAII principles.
 */
class LibraryValidator {
public:
    /**
     * @brief Structure containing library validation information
     */
    struct LibraryInfo {
        std::string path;           // Full path to the library file
        std::string name;           // Library name (extracted from filename)
        bool loaded;                // Whether library was successfully loaded
        std::string error;          // Error message if loading failed
        bool validated;             // Whether library passed validation checks
        
        LibraryInfo() : loaded(false), validated(false) {}
        
        explicit LibraryInfo(const std::string& libraryPath) 
            : path(libraryPath), loaded(false), validated(false) {
            if (!libraryPath.empty()) {
                // Extract name from path (stem without extension)
                size_t lastSlash = libraryPath.find_last_of("/\\");
                size_t lastDot = libraryPath.find_last_of(".");
                
                // Get filename (everything after last slash, or whole path if no slash)
                std::string filename;
                if (lastSlash != std::string::npos) {
                    filename = libraryPath.substr(lastSlash + 1);
                } else {
                    filename = libraryPath;
                }
                
                // Remove extension if present
                if (lastDot != std::string::npos && (lastSlash == std::string::npos || lastDot > lastSlash)) {
                    size_t dotInFilename = filename.find_last_of(".");
                    if (dotInFilename != std::string::npos) {
                        name = filename.substr(0, dotInFilename);
                    } else {
                        name = filename;
                    }
                } else {
                    name = filename;
                }
            }
        }
        
        // Check if library is ready for use
        bool isReady() const {
            return loaded && validated && error.empty();
        }
    };

    /**
     * @brief Validate a GameManager library using the registration system
     * 
     * Loads the library, creates a GameManager entry, validates registration,
     * and performs cleanup on errors. Uses RAII principles for resource management.
     * 
     * @param soPath Path to the GameManager .so file
     * @return LibraryInfo with validation results and any error information
     */
    static LibraryInfo validateGameManager(const std::string& soPath);

    /**
     * @brief Validate an Algorithm library using the registration system
     * 
     * Loads the library, creates an Algorithm entry, validates registration,
     * and performs cleanup on errors. Uses RAII principles for resource management.
     * 
     * @param soPath Path to the Algorithm .so file
     * @return LibraryInfo with validation results and any error information
     */
    static LibraryInfo validateAlgorithm(const std::string& soPath);


    /**
     * @brief Check if a library file is valid and accessible
     * 
     * Performs basic file system checks before attempting to load the library.
     * Checks existence, file permissions, and basic format validation.
     * 
     * @param soPath Path to the library file
     * @return True if file appears to be a valid library file
     */
    static bool isValidLibraryFile(const std::string& soPath);

    /**
     * @brief Get the last error message from validation operations
     * 
     * Returns human-readable error message from the most recent operation
     * that failed. Useful for debugging and user feedback.
     * 
     * @return String containing the last error message, empty if no error
     */
    static const std::string& getLastError();

    /**
     * @brief Clean up resources from failed validation attempts
     * 
     * Cleans up any partially loaded resources, unregistered entries,
     * and resets the validator state. Called automatically on validation failures.
     */
    static void cleanup();

private:
    /**
     * @brief Validate GameManager registration after library loading
     * 
     * Helper method to check if GameManager registration completed successfully
     * and the registered GameManager is functional.
     * 
     * @param info LibraryInfo to update with validation results
     * @return True if validation passed
     */
    static bool validateGameManagerRegistration(LibraryInfo& info);

    /**
     * @brief Validate Algorithm registration after library loading
     * 
     * Helper method to check if Algorithm registration completed successfully
     * and the registered Algorithm is functional.
     * 
     * @param info LibraryInfo to update with validation results
     * @return True if validation passed
     */
    static bool validateAlgorithmRegistration(LibraryInfo& info);

    /**
     * @brief Set the last error message for error reporting
     * 
     * @param error Error message to store
     */
    static void setLastError(const std::string& error);

    static thread_local std::string s_lastError;
};