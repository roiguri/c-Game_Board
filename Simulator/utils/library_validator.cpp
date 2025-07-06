#include "library_validator.h"
#include "library_manager.h"
#include "registration/GameManagerRegistrar.h"
#include "registration/AlgorithmRegistrar.h"
#include <filesystem>
#include <iostream>

// Thread-local error storage
thread_local std::string LibraryValidator::s_lastError;

LibraryValidator::LibraryInfo LibraryValidator::validateGameManager(const std::string& soPath) {
    LibraryInfo info(soPath);
    
    try {
        // First validate the file exists and is accessible
        if (!isValidLibraryFile(soPath)) {
            info.error = getLastError();
            return info;
        }
        
        LibraryManager& libManager = LibraryManager::getInstance();
        GameManagerRegistrar& gmRegistrar = GameManagerRegistrar::getGameManagerRegistrar();
        
        // Create GameManager entry
        gmRegistrar.createGameManagerEntry(soPath);
        
        // Load library
        if (!libManager.loadLibrary(soPath)) {
            info.error = libManager.getLastError();
            gmRegistrar.removeLast();
            setLastError(info.error);
            return info;
        }
        
        info.loaded = true;
        
        // Validate registration
        if (!validateGameManagerRegistration(info)) {
            cleanup();
            return info;
        }
        
        info.validated = true;
        setLastError("");  // Clear any previous error
        
    } catch (const std::exception& e) {
        info.error = "Exception during GameManager validation: " + std::string(e.what());
        info.loaded = false;
        info.validated = false;
        setLastError(info.error);
        cleanup();
    }
    
    return info;
}

LibraryValidator::LibraryInfo LibraryValidator::validateAlgorithm(const std::string& soPath) {
    LibraryInfo info(soPath);
    
    try {
        // First validate the file exists and is accessible
        if (!isValidLibraryFile(soPath)) {
            info.error = getLastError();
            return info;
        }
        
        LibraryManager& libManager = LibraryManager::getInstance();
        AlgorithmRegistrar& algoRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
        
        // Create Algorithm entry
        algoRegistrar.createAlgorithmFactoryEntry(soPath);
        
        // Load library
        if (!libManager.loadLibrary(soPath)) {
            info.error = libManager.getLastError();
            algoRegistrar.removeLast();
            setLastError(info.error);
            return info;
        }
        
        info.loaded = true;
        
        // Validate registration
        if (!validateAlgorithmRegistration(info)) {
            cleanup();
            return info;
        }
        
        info.validated = true;
        setLastError("");  // Clear any previous error
        
    } catch (const std::exception& e) {
        info.error = "Exception during Algorithm validation: " + std::string(e.what());
        info.loaded = false;
        info.validated = false;
        setLastError(info.error);
        cleanup();
    }
    
    return info;
}


bool LibraryValidator::isValidLibraryFile(const std::string& soPath) {
    try {
        if (soPath.empty()) {
            setLastError("Library path cannot be empty");
            return false;
        }
        
        std::filesystem::path path(soPath);
        
        if (!std::filesystem::exists(path)) {
            setLastError("Library file does not exist: " + soPath);
            return false;
        }
        
        if (!std::filesystem::is_regular_file(path)) {
            setLastError("Path is not a regular file: " + soPath);
            return false;
        }
        
        // Check file extension
        if (path.extension() != ".so") {
            setLastError("File does not have .so extension: " + soPath);
            return false;
        }
        
        // Check if file is readable by attempting to get file size
        std::error_code ec;
        [[maybe_unused]] auto fileSize = std::filesystem::file_size(path, ec);
        if (ec) {
            setLastError("Cannot access library file (permission denied?): " + soPath);
            return false;
        }
        
        setLastError("");  // Clear any previous error
        return true;
        
    } catch (const std::filesystem::filesystem_error& e) {
        setLastError("Filesystem error validating library " + soPath + ": " + e.what());
        return false;
    } catch (const std::exception& e) {
        setLastError("Exception validating library " + soPath + ": " + e.what());
        return false;
    }
}

const std::string& LibraryValidator::getLastError() {
    return s_lastError;
}

void LibraryValidator::cleanup() {
    try {
        GameManagerRegistrar& gmRegistrar = GameManagerRegistrar::getGameManagerRegistrar();
        AlgorithmRegistrar& algoRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
        
        // Clear registrars to clean up any failed registrations
        gmRegistrar.clear();
        algoRegistrar.clear();
        
        setLastError("");  // Clear error state
        
    } catch (const std::exception& e) {
        setLastError("Exception during cleanup: " + std::string(e.what()));
        std::cerr << "Warning: " << getLastError() << std::endl;
    }
}

bool LibraryValidator::validateGameManagerRegistration(LibraryInfo& info) {
    try {
        GameManagerRegistrar& gmRegistrar = GameManagerRegistrar::getGameManagerRegistrar();
        
        // Validate registration
        try {
            gmRegistrar.validateLastRegistration();
        } catch (const GameManagerRegistrar::BadGameManagerRegistrationException& e) {
            info.error = "GameManager registration failed for " + e.name;
            if (!e.hasFactory) {
                info.error += " (missing factory)";
            }
            if (!e.hasName) {
                info.error += " (missing name)";
            }
            setLastError(info.error);
            return false;
        }
        
        // Check if we can get the GameManager name
        if (gmRegistrar.count() > 0) {
            auto gmEntry = gmRegistrar.begin();
            info.name = gmEntry->name();
            
            // Try to create an instance to verify functionality
            try {
                auto gameManager = gmEntry->create(false);  // verbose = false for testing
                if (!gameManager) {
                    info.error = "GameManager factory returned null instance";
                    setLastError(info.error);
                    return false;
                }
            } catch (const std::exception& e) {
                info.error = "Failed to create GameManager instance: " + std::string(e.what());
                setLastError(info.error);
                return false;
            }
        } else {
            info.error = "No GameManager registered after loading";
            setLastError(info.error);
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        info.error = "Exception during GameManager registration validation: " + std::string(e.what());
        setLastError(info.error);
        return false;
    }
}

bool LibraryValidator::validateAlgorithmRegistration(LibraryInfo& info) {
    try {
        AlgorithmRegistrar& algoRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
        
        // Validate registration
        try {
            algoRegistrar.validateLastRegistration();
        } catch (const BadRegistrationException& e) {
            info.error = "Algorithm registration failed: " + std::string(e.what());
            setLastError(info.error);
            return false;
        }
        
        // Check if we can get the Algorithm name
        if (algoRegistrar.size() > 0) {
            auto algoEntry = algoRegistrar.begin();
            info.name = algoEntry->getName();
            
            // Try to create instances to verify functionality
            try {
                // Use dummy parameters for validation - TankAlgorithmFactory(player_index, tank_index)
                auto tankAlgorithm = algoEntry->getTankAlgorithmFactory()(0, 0);
                // Use dummy parameters for validation - PlayerFactory(player_index, x, y, max_steps, num_shells)
                auto player = algoEntry->getPlayerFactory()(0, 0, 0, 100, 10);
                
                if (!tankAlgorithm || !player) {
                    info.error = "Algorithm factories returned null instances";
                    setLastError(info.error);
                    return false;
                }
            } catch (const std::exception& e) {
                info.error = "Failed to create Algorithm instances: " + std::string(e.what());
                setLastError(info.error);
                return false;
            }
        } else {
            info.error = "No Algorithm registered after loading";
            setLastError(info.error);
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        info.error = "Exception during Algorithm registration validation: " + std::string(e.what());
        setLastError(info.error);
        return false;
    }
}

void LibraryValidator::setLastError(const std::string& error) {
    s_lastError = error;
}