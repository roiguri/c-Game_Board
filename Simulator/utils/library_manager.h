#pragma once

#include <vector>
#include <string>
#include <memory>

namespace Simulator_098765432_123456789 {

class LibraryManager {
public:
    static LibraryManager& getInstance();
    
    bool loadLibrary(const std::string& path);
    void unloadAllLibraries();
    std::vector<std::string> getLoadedLibraryNames() const;
    std::string getLastError() const;
    
    ~LibraryManager();
    
    LibraryManager(const LibraryManager&) = delete;
    LibraryManager& operator=(const LibraryManager&) = delete;
    LibraryManager(LibraryManager&&) = delete;
    LibraryManager& operator=(LibraryManager&&) = delete;

private:
    LibraryManager() = default;
    
    std::vector<void*> m_handles;
    std::vector<std::string> m_names;
    mutable std::string m_lastError;
    
    void clearError();
    void setError(const std::string& error);
};

} // namespace Simulator_098765432_123456789