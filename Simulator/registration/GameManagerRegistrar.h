#pragma once

#include <vector>
#include <string>
#include <memory>
#include "common/AbstractGameManager.h"

class GameManagerRegistrar {
public:
    ~GameManagerRegistrar();
    
    struct GameManagerEntry {
        std::string so_name;
        GameManagerFactory factory;
        
        GameManagerEntry(const std::string& name);
        void setFactory(GameManagerFactory&& f);
        const std::string& name() const;
        std::unique_ptr<AbstractGameManager> create(bool verbose) const;
        bool hasFactory() const;
    };
    
    struct BadGameManagerRegistrationException {
        std::string name;
        bool hasName;
        bool hasFactory;
    };
    
private:
    std::vector<GameManagerEntry> gameManagers;
    static GameManagerRegistrar registrar;
    
public:
    static GameManagerRegistrar& getGameManagerRegistrar();
    
    void createGameManagerEntry(const std::string& name);
    void addGameManagerFactoryToLastEntry(GameManagerFactory&& factory);
    void validateLastRegistration();
    void removeLast();
    
    std::vector<GameManagerEntry>::const_iterator begin() const;
    std::vector<GameManagerEntry>::const_iterator end() const;
    std::size_t count() const;
    void clear();
    
    const GameManagerEntry* findByName(const std::string& name) const;
};