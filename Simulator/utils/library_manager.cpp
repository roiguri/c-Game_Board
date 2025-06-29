#include "library_manager.h"
#include <dlfcn.h>
#include <algorithm>
#include <filesystem>

namespace Simulator_098765432_123456789 {

LibraryManager& LibraryManager::getInstance() {
    static LibraryManager instance;
    return instance;
}

bool LibraryManager::loadLibrary(const std::string& path) {
    clearError();
    
    // Validate path
    if (path.empty()) {
        setError("Library path cannot be empty");
        return false;
    }
    
    // Check if library is already loaded
    if (std::find(m_names.begin(), m_names.end(), path) != m_names.end()) {
        setError("Library already loaded: " + path);
        return false;
    }
    
    // Check if file exists
    if (!std::filesystem::exists(path)) {
        setError("Library file does not exist: " + path);
        return false;
    }
    
    // Load library with RTLD_LAZY | RTLD_GLOBAL for proper symbol resolution
    void* handle = dlopen(path.c_str(), RTLD_LAZY | RTLD_GLOBAL);
    if (!handle) {
        const char* error = dlerror();
        setError(std::string("Failed to load library: ") + (error ? error : "Unknown error"));
        return false;
    }
    
    // Track loaded library
    m_handles.push_back(handle);
    m_names.push_back(path);
    
    return true;
}

void LibraryManager::unloadAllLibraries() {
    clearError();
    
    // Unload libraries in reverse order
    for (auto it = m_handles.rbegin(); it != m_handles.rend(); ++it) {
        if (*it) {
            dlclose(*it);
        }
    }
    
    m_handles.clear();
    m_names.clear();
}

std::vector<std::string> LibraryManager::getLoadedLibraryNames() const {
    return m_names;
}

std::string LibraryManager::getLastError() const {
    return m_lastError;
}

LibraryManager::~LibraryManager() {
    unloadAllLibraries();
}

void LibraryManager::clearError() {
    m_lastError.clear();
    dlerror(); // Clear any existing dlerror state
}

void LibraryManager::setError(const std::string& error) {
    m_lastError = error;
}

} // namespace Simulator_098765432_123456789