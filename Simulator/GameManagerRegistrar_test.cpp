#include "gtest/gtest.h"
#include "GameManagerRegistrar.h"
#include "test/mocks/MockGameManager.h"

class GameManagerRegistrarTest : public ::testing::Test {
protected:
    void SetUp() override {
        GameManagerRegistrar::getGameManagerRegistrar().clear();
    }
    
    void TearDown() override {
        GameManagerRegistrar::getGameManagerRegistrar().clear();
    }
};

TEST_F(GameManagerRegistrarTest, SingletonBehavior) {
    auto& registrar1 = GameManagerRegistrar::getGameManagerRegistrar();
    auto& registrar2 = GameManagerRegistrar::getGameManagerRegistrar();
    
    EXPECT_EQ(&registrar1, &registrar2);
}

TEST_F(GameManagerRegistrarTest, CreateGameManagerEntry) {
    auto& registrar = GameManagerRegistrar::getGameManagerRegistrar();
    
    EXPECT_EQ(registrar.count(), 0);
    
    registrar.createGameManagerEntry("TestGameManager");
    
    EXPECT_EQ(registrar.count(), 1);
}

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

TEST_F(GameManagerRegistrarTest, ValidationFailsWithEmptyName) {
    auto& registrar = GameManagerRegistrar::getGameManagerRegistrar();
    
    registrar.createGameManagerEntry("");
    
    GameManagerFactory factory = [](bool verbose) {
        return std::make_unique<MockGameManager>(verbose);
    };
    
    registrar.addGameManagerFactoryToLastEntry(std::move(factory));
    
    EXPECT_THROW(registrar.validateLastRegistration(), 
                 GameManagerRegistrar::BadGameManagerRegistrationException);
}

TEST_F(GameManagerRegistrarTest, ExceptionContainsCorrectInfo) {
    auto& registrar = GameManagerRegistrar::getGameManagerRegistrar();
    
    registrar.createGameManagerEntry("TestGameManager");
    
    try {
        registrar.validateLastRegistration();
        FAIL() << "Expected BadGameManagerRegistrationException";
    } catch (const GameManagerRegistrar::BadGameManagerRegistrationException& e) {
        EXPECT_EQ(e.name, "TestGameManager");
        EXPECT_TRUE(e.hasName);
        EXPECT_FALSE(e.hasFactory);
    }
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
    
    // Verify it's actually our mock and has correct verbose setting
    auto* mockGameManager = dynamic_cast<MockGameManager*>(gameManager.get());
    EXPECT_NE(mockGameManager, nullptr);
    EXPECT_TRUE(mockGameManager->isVerbose());
}

TEST_F(GameManagerRegistrarTest, CreateGameManagerInstanceWithoutVerbose) {
    auto& registrar = GameManagerRegistrar::getGameManagerRegistrar();
    
    registrar.createGameManagerEntry("TestGameManager");
    GameManagerFactory factory = [](bool verbose) {
        return std::make_unique<MockGameManager>(verbose);
    };
    registrar.addGameManagerFactoryToLastEntry(std::move(factory));
    registrar.validateLastRegistration();
    
    const auto* entry = registrar.findByName("TestGameManager");
    auto gameManager = entry->create(false);
    
    auto* mockGameManager = dynamic_cast<MockGameManager*>(gameManager.get());
    EXPECT_NE(mockGameManager, nullptr);
    EXPECT_FALSE(mockGameManager->isVerbose());
}

TEST_F(GameManagerRegistrarTest, RemoveLastEntry) {
    auto& registrar = GameManagerRegistrar::getGameManagerRegistrar();
    
    registrar.createGameManagerEntry("TestGameManager");
    EXPECT_EQ(registrar.count(), 1);
    
    registrar.removeLast();
    EXPECT_EQ(registrar.count(), 0);
}

TEST_F(GameManagerRegistrarTest, IteratorSupport) {
    auto& registrar = GameManagerRegistrar::getGameManagerRegistrar();
    
    // Add multiple entries
    registrar.createGameManagerEntry("GameManager1");
    GameManagerFactory factory1 = [](bool verbose) {
        return std::make_unique<MockGameManager>(verbose);
    };
    registrar.addGameManagerFactoryToLastEntry(std::move(factory1));
    registrar.validateLastRegistration();
    
    registrar.createGameManagerEntry("GameManager2");
    GameManagerFactory factory2 = [](bool verbose) {
        return std::make_unique<MockGameManager>(verbose);
    };
    registrar.addGameManagerFactoryToLastEntry(std::move(factory2));
    registrar.validateLastRegistration();
    
    // Test iterator
    std::vector<std::string> names;
    for (const auto& entry : registrar) {
        names.push_back(entry.name());
    }
    
    EXPECT_EQ(names.size(), 2);
    EXPECT_EQ(names[0], "GameManager1");
    EXPECT_EQ(names[1], "GameManager2");
}

TEST_F(GameManagerRegistrarTest, ClearRegistrar) {
    auto& registrar = GameManagerRegistrar::getGameManagerRegistrar();
    
    registrar.createGameManagerEntry("TestGameManager");
    EXPECT_EQ(registrar.count(), 1);
    
    registrar.clear();
    EXPECT_EQ(registrar.count(), 0);
}

TEST_F(GameManagerRegistrarTest, GameManagerEntryHasFactory) {
    auto& registrar = GameManagerRegistrar::getGameManagerRegistrar();
    
    registrar.createGameManagerEntry("TestGameManager");
    
    // We'll test hasFactory indirectly through validation
    // Before adding factory, validation should fail
    EXPECT_THROW(registrar.validateLastRegistration(), 
                 GameManagerRegistrar::BadGameManagerRegistrationException);
    
    GameManagerFactory factory = [](bool verbose) {
        return std::make_unique<MockGameManager>(verbose);
    };
    registrar.addGameManagerFactoryToLastEntry(std::move(factory));
    
    // After adding factory, validation should pass
    EXPECT_NO_THROW(registrar.validateLastRegistration());
}