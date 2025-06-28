# GameManager Registrar Implementation Plan

Based on your project knowledge and the existing algorithm registration system, here's a comprehensive implementation plan for the GameManager registrar:

## Step 1: Create GameManagerRegistrar Header
**File: `simulator/GameManagerRegistrar.h`**

```cpp
#pragma once

#include <vector>
#include <string>
#include <memory>
#include <cassert>
#include "common/AbstractGameManager.h"

class GameManagerRegistrar {
    struct GameManagerEntry {
        std::string so_name;
        GameManagerFactory factory;
        
        GameManagerEntry(const std::string& name) : so_name(name) {}
        
        void setFactory(GameManagerFactory&& f) {
            assert(factory == nullptr);
            factory = std::move(f);
        }
        
        const std::string& name() const { return so_name; }
        
        std::unique_ptr<AbstractGameManager> create(bool verbose) const {
            return factory(verbose);
        }
        
        bool hasFactory() const {
            return factory != nullptr;
        }
    };
    
    std::vector<GameManagerEntry> gameManagers;
    static GameManagerRegistrar registrar;
    
public:
    static GameManagerRegistrar& getGameManagerRegistrar();
    
    void createGameManagerEntry(const std::string& name) {
        gameManagers.emplace_back(name);
    }
    
    void addGameManagerFactoryToLastEntry(GameManagerFactory&& factory) {
        gameManagers.back().setFactory(std::move(factory));
    }
    
    struct BadGameManagerRegistrationException {
        std::string name;
        bool hasName;
        bool hasFactory;
    };
    
    void validateLastRegistration() {
        const auto& last = gameManagers.back();
        bool hasName = !last.name().empty();
        if (!hasName || !last.hasFactory()) {
            throw BadGameManagerRegistrationException{
                .name = last.name(),
                .hasName = hasName,
                .hasFactory = last.hasFactory()
            };
        }
    }
    
    void removeLast() {
        gameManagers.pop_back();
    }
    
    auto begin() const { return gameManagers.begin(); }
    auto end() const { return gameManagers.end(); }
    std::size_t count() const { return gameManagers.size(); }
    void clear() { gameManagers.clear(); }
    
    // Get specific GameManager by name
    const GameManagerEntry* findByName(const std::string& name) const {
        for (const auto& entry : gameManagers) {
            if (entry.name() == name) {
                return &entry;
            }
        }
        return nullptr;
    }
};
```

## Step 2: Implement GameManagerRegistrar Source
**File: `simulator/GameManagerRegistrar.cpp`**

```cpp
#include "GameManagerRegistrar.h"

GameManagerRegistrar GameManagerRegistrar::registrar;

GameManagerRegistrar& GameManagerRegistrar::getGameManagerRegistrar() {
    return registrar;
}
```

## Step 3: Implement GameManagerRegistration Source
**File: `simulator/GameManagerRegistration.cpp`**

```cpp
#include "common/GameManagerRegistration.h"
#include "GameManagerRegistrar.h"

GameManagerRegistration::GameManagerRegistration(GameManagerFactory factory) {
    auto& registrar = GameManagerRegistrar::getGameManagerRegistrar();
    registrar.addGameManagerFactoryToLastEntry(std::move(factory));
}
```

## Step 4: Update Simulator to Use GameManager Registration
**Integration into main simulator logic:**

```cpp
// In your Simulator class or main function
#include "GameManagerRegistrar.h"

class Simulator {
    std::vector<void*> loadedGameManagerHandles; // For dlclose
    
public:
    void loadGameManagers(const std::vector<std::string>& gameManagerPaths) {
        auto& registrar = GameManagerRegistrar::getGameManagerRegistrar();
        
        for (const auto& path : gameManagerPaths) {
            // Extract name from path (e.g., "GameManager_123.so" -> "GameManager_123")
            std::string name = extractNameFromPath(path);
            
            // Create entry before loading
            registrar.createGameManagerEntry(name);
            
            // Load the .so file
            void* handle = dlopen(path.c_str(), RTLD_LAZY);
            if (!handle) {
                std::cerr << "Failed to load GameManager: " << dlerror() << std::endl;
                registrar.removeLast();
                continue;
            }
            
            loadedGameManagerHandles.push_back(handle);
            
            try {
                registrar.validateLastRegistration();
                std::cout << "Successfully loaded GameManager: " << name << std::endl;
            } catch (GameManagerRegistrar::BadGameManagerRegistrationException& e) {
                std::cerr << "Bad GameManager registration for: " << e.name << std::endl;
                std::cerr << "Has name: " << std::boolalpha << e.hasName << std::endl;
                std::cerr << "Has factory: " << std::boolalpha << e.hasFactory << std::endl;
                
                registrar.removeLast();
                dlclose(handle);
                loadedGameManagerHandles.pop_back();
            }
        }
    }
    
    void runWithGameManager(const std::string& gameManagerName, 
                           size_t map_width, size_t map_height,
                           const SatelliteView& map,
                           size_t max_steps, size_t num_shells,
                           Player& player1, Player& player2,
                           TankAlgorithmFactory player1_factory,
                           TankAlgorithmFactory player2_factory,
                           bool verbose) {
        
        auto& registrar = GameManagerRegistrar::getGameManagerRegistrar();
        const auto* entry = registrar.findByName(gameManagerName);
        
        if (!entry) {
            throw std::runtime_error("GameManager not found: " + gameManagerName);
        }
        
        auto gameManager = entry->create(verbose);
        auto result = gameManager->run(map_width, map_height, map, max_steps, num_shells,
                                      player1, player2, player1_factory, player2_factory);
        
        // Process result...
    }
    
    ~Simulator() {
        // Clean up before closing handles
        GameManagerRegistrar::getGameManagerRegistrar().clear();
        
        for (void* handle : loadedGameManagerHandles) {
            dlclose(handle);
        }
    }
    
private:
    std::string extractNameFromPath(const std::string& path) {
        size_t lastSlash = path.find_last_of('/');
        size_t start = (lastSlash == std::string::npos) ? 0 : lastSlash + 1;
        
        size_t lastDot = path.find_last_of('.');
        size_t end = (lastDot == std::string::npos) ? path.length() : lastDot;
        
        return path.substr(start, end - start);
    }
};
```

## Step 5: Example Usage in Your GameManager Implementation
**File: `GameManager/game_manager.cpp`**

```cpp
#include "common/GameManagerRegistration.h"

namespace GameManager_098765432_123456789 {
    // Your GameManager implementation...
}

// At the end of the file, in global scope:
REGISTER_GAME_MANAGER(GameManager_098765432_123456789)
```

## Step 6: Unit Tests for GameManagerRegistrar
**File: `test/GameManagerRegistrarTest.cpp`**

```cpp
#include "gtest/gtest.h"
#include "simulator/GameManagerRegistrar.h"
#include "common/AbstractGameManager.h"

// Mock GameManager for testing
class MockGameManager : public AbstractGameManager {
public:
    MockGameManager(bool verbose) : verbose_(verbose) {}
    
    GameResult run(size_t, size_t, const SatelliteView&, size_t, size_t,
                  Player&, Player&, TankAlgorithmFactory, TankAlgorithmFactory) override {
        return GameResult{}; // Return dummy result
    }
    
private:
    bool verbose_;
};

class GameManagerRegistrarTest : public ::testing::Test {
protected:
    void SetUp() override {
        GameManagerRegistrar::getGameManagerRegistrar().clear();
    }
    
    void TearDown() override {
        GameManagerRegistrar::getGameManagerRegistrar().clear();
    }
};

TEST_F(GameManagerRegistrarTest, CreateAndAddFactory) {
    auto& registrar = GameManagerRegistrar::getGameManagerRegistrar();
    
    registrar.createGameManagerEntry("TestGameManager");
    
    GameManagerFactory factory = [](bool verbose) {
        return std::make_unique<MockGameManager>(verbose);
    };
    
    registrar.addGameManagerFactoryToLastEntry(std::move(factory));
    
    EXPECT_NO_THROW(registrar.validateLastRegistration());
    EXPECT_EQ(registrar.count(), 1);
}

TEST_F(GameManagerRegistrarTest, ValidationFailsWithoutFactory) {
    auto& registrar = GameManagerRegistrar::getGameManagerRegistrar();
    
    registrar.createGameManagerEntry("TestGameManager");
    
    EXPECT_THROW(registrar.validateLastRegistration(), 
                 GameManagerRegistrar::BadGameManagerRegistrationException);
}

TEST_F(GameManagerRegistrarTest, FindByName) {
    auto& registrar = GameManagerRegistrar::getGameManagerRegistrar();
    
    registrar.createGameManagerEntry("TestGameManager");
    GameManagerFactory factory = [](bool verbose) {
        return std::make_unique<MockGameManager>(verbose);
    };
    registrar.addGameManagerFactoryToLastEntry(std::move(factory));
    registrar.validateLastRegistration();
    
    const auto* entry = registrar.findByName("TestGameManager");
    ASSERT_NE(entry, nullptr);
    EXPECT_EQ(entry->name(), "TestGameManager");
    
    const auto* notFound = registrar.findByName("NonExistent");
    EXPECT_EQ(notFound, nullptr);
}

TEST_F(GameManagerRegistrarTest, CreateGameManagerInstance) {
    auto& registrar = GameManagerRegistrar::getGameManagerRegistrar();
    
    registrar.createGameManagerEntry("TestGameManager");
    GameManagerFactory factory = [](bool verbose) {
        return std::make_unique<MockGameManager>(verbose);
    };
    registrar.addGameManagerFactoryToLastEntry(std::move(factory));
    registrar.validateLastRegistration();
    
    const auto* entry = registrar.findByName("TestGameManager");
    auto gameManager = entry->create(true);
    
    EXPECT_NE(gameManager, nullptr);
    EXPECT_NE(dynamic_cast<MockGameManager*>(gameManager.get()), nullptr);
}
```

## Implementation Order

1. **First Commit**: Create `GameManagerRegistrar.h` and `GameManagerRegistrar.cpp`
2. **Second Commit**: Implement `GameManagerRegistration.cpp` 
3. **Third Commit**: Add unit tests for the registrar
4. **Fourth Commit**: Integrate GameManager loading into Simulator class
5. **Fifth Commit**: Add error handling and cleanup logic

## Key Design Decisions

- **Singleton Pattern**: Following the same pattern as `AlgorithmRegistrar`
- **Exception Handling**: Custom exception for registration validation failures
- **RAII**: Proper cleanup with dlclose in destructor
- **Name-based Lookup**: Ability to find and use specific GameManagers by name
- **Factory Pattern**: Clean separation between registration and instantiation

This design allows the simulator to dynamically load multiple GameManager implementations and select which one to use for running games, enabling competition between different GameManager strategies.