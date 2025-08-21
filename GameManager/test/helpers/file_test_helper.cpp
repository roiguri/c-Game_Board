#include "file_test_helper.h"
#include <filesystem>
#include <fstream>

namespace GameManager_318835816_211314471 {

FileTestHelper::FileTestHelper() {
    recordInitialFiles();
}

FileTestHelper::~FileTestHelper() {
    cleanupTestFiles();
}

void FileTestHelper::recordInitialFiles() {
    m_initialFiles.clear();
    try {
        for (const auto& entry : std::filesystem::directory_iterator(".")) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (startsWith(filename, "game_")) {
                    m_initialFiles.insert(filename);
                }
            }
        }
    } catch (const std::filesystem::filesystem_error&) {
        // If we can't read the directory, just continue with empty initial files
    }
}

std::vector<std::string> FileTestHelper::getNewGameFiles() const {
    std::vector<std::string> newFiles;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(".")) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (startsWith(filename, "game_") && m_initialFiles.find(filename) == m_initialFiles.end()) {
                    newFiles.push_back(filename);
                }
            }
        }
    } catch (const std::filesystem::filesystem_error&) {
        // If we can't read the directory, return empty vector
    }
    return newFiles;
}

void FileTestHelper::cleanupTestFiles() {
    try {
        for (const auto& entry : std::filesystem::directory_iterator(".")) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (startsWith(filename, "game_") && m_initialFiles.find(filename) == m_initialFiles.end()) {
                    std::filesystem::remove(entry.path());
                }
            }
        }
    } catch (const std::filesystem::filesystem_error&) {
        // If we can't clean up files, just continue
    }
}

bool FileTestHelper::startsWith(const std::string& str, const std::string& prefix) {
    if (prefix.length() > str.length()) {
        return false;
    }
    return str.substr(0, prefix.length()) == prefix;
}

bool FileTestHelper::endsWith(const std::string& str, const std::string& suffix) {
    if (suffix.length() > str.length()) {
        return false;
    }
    return str.substr(str.length() - suffix.length()) == suffix;
}

std::string FileTestHelper::cleanFilename(const std::string& name) {
    std::string cleaned = name;
    const std::string invalidChars = "\\/:*?\"<>|";
    for (char& c : cleaned) {
        if (invalidChars.find(c) != std::string::npos) {
            c = '_';
        }
    }
    return cleaned;
}

} // namespace GameManager_318835816_211314471