#include "file_enumerator.h"
#include <algorithm>
#include <iostream>

// Thread-local error storage
thread_local std::string FileEnumerator::s_lastError;

std::vector<std::string> FileEnumerator::enumerateSoFiles(const std::string& directory) {
    return enumerateFiles(directory, ".so", true);
}

std::vector<std::string> FileEnumerator::enumerateMapFiles(const std::string& directory) {
    // Map files are always .txt format
    return enumerateFiles(directory, ".txt", true);
}

FileEnumerator::FileInfo FileEnumerator::validateFile(const std::string& filePath) {
    FileInfo info(filePath);
    
    try {
        if (filePath.empty()) {
            info.error = "File path is empty";
            return info;
        }
        
        std::filesystem::path fsPath(filePath);
        
        if (!std::filesystem::exists(fsPath)) {
            info.error = "File does not exist: " + filePath;
            return info;
        }
        
        if (!std::filesystem::is_regular_file(fsPath)) {
            info.error = "Path is not a regular file: " + filePath;
            return info;
        }
        
        // TODO: consider how to handle file validation
        // Check if file is readable by attempting to get file size
        std::error_code ec;
        [[maybe_unused]] auto fileSize = std::filesystem::file_size(fsPath, ec);
        if (ec) {
            info.error = "Cannot access file (permission denied?): " + filePath;
            return info;
        }
        
        info.isValid = true;
        info.error.clear();
        
    } catch (const std::filesystem::filesystem_error& e) {
        info.error = "Filesystem error validating file " + filePath + ": " + e.what();
    } catch (const std::exception& e) {
        info.error = "Exception validating file " + filePath + ": " + e.what();
    }
    
    return info;
}

bool FileEnumerator::isValidDirectory(const std::string& directory) {
    try {
        if (directory.empty()) {
            setLastError("Directory path is empty");
            return false;
        }
        
        std::filesystem::path dirPath(directory);
        
        if (!std::filesystem::exists(dirPath)) {
            setLastError("Directory does not exist: " + directory);
            return false;
        }
        
        if (!std::filesystem::is_directory(dirPath)) {
            setLastError("Path is not a directory: " + directory);
            return false;
        }
        
        // Test directory accessibility by attempting to iterate
        std::error_code ec;
        auto iter = std::filesystem::directory_iterator(dirPath, ec);
        if (ec) {
            setLastError("Cannot access directory (permission denied?): " + directory);
            return false;
        }
        
        setLastError("");  // Clear any previous error
        return true;
        
    } catch (const std::filesystem::filesystem_error& e) {
        setLastError("Filesystem error validating directory " + directory + ": " + e.what());
        return false;
    } catch (const std::exception& e) {
        setLastError("Exception validating directory " + directory + ": " + e.what());
        return false;
    }
}

const std::string& FileEnumerator::getLastError() {
    return s_lastError;
}

std::vector<std::string> FileEnumerator::enumerateFiles(
    const std::string& directory, 
    const std::string& extension,
    bool sorted) {
    
    std::vector<std::string> files;
    
    try {
        // Validate directory first
        if (!isValidDirectory(directory)) {
            // Error message already set by isValidDirectory
            return files;
        }
        
        // Iterate through directory entries
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string filePath = entry.path().string();
                std::string fileExtension = entry.path().extension().string();
                
                // Filter by extension if specified
                if (extension.empty() || fileExtension == extension) {
                    files.push_back(filePath);
                }
            }
        }
        
        // Sort files for consistent ordering if requested
        if (sorted) {
            std::sort(files.begin(), files.end());
        }
        
        setLastError("");  // Clear any previous error
        
    } catch (const std::filesystem::filesystem_error& e) {
        std::string errorMsg = "Filesystem error enumerating files in " + directory + ": " + e.what();
        setLastError(errorMsg);
        std::cerr << errorMsg << std::endl;
    } catch (const std::exception& e) {
        std::string errorMsg = "Exception enumerating files in " + directory + ": " + e.what();
        setLastError(errorMsg);
        std::cerr << errorMsg << std::endl;
    }
    
    return files;
}

void FileEnumerator::setLastError(const std::string& error) {
    s_lastError = error;
}