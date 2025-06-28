#include "GameManagerRegistrar.h"
#include <cassert>

// Static member definition
GameManagerRegistrar GameManagerRegistrar::registrar;

// GameManagerEntry implementations
GameManagerRegistrar::GameManagerEntry::GameManagerEntry(const std::string& name) : so_name(name) {}

void GameManagerRegistrar::GameManagerEntry::setFactory(GameManagerFactory&& f) {
    assert(factory == nullptr);
    factory = std::move(f);
}

const std::string& GameManagerRegistrar::GameManagerEntry::name() const {
    return so_name;
}

std::unique_ptr<AbstractGameManager> GameManagerRegistrar::GameManagerEntry::create(bool verbose) const {
    return factory(verbose);
}

bool GameManagerRegistrar::GameManagerEntry::hasFactory() const {
    return factory != nullptr;
}

// GameManagerRegistrar implementations
GameManagerRegistrar& GameManagerRegistrar::getGameManagerRegistrar() {
    return registrar;
}

void GameManagerRegistrar::createGameManagerEntry(const std::string& name) {
    gameManagers.emplace_back(name);
}

void GameManagerRegistrar::addGameManagerFactoryToLastEntry(GameManagerFactory&& factory) {
    gameManagers.back().setFactory(std::move(factory));
}

void GameManagerRegistrar::validateLastRegistration() {
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

void GameManagerRegistrar::removeLast() {
    gameManagers.pop_back();
}

std::vector<GameManagerRegistrar::GameManagerEntry>::const_iterator GameManagerRegistrar::begin() const {
    return gameManagers.begin();
}

std::vector<GameManagerRegistrar::GameManagerEntry>::const_iterator GameManagerRegistrar::end() const {
    return gameManagers.end();
}

std::size_t GameManagerRegistrar::count() const {
    return gameManagers.size();
}

void GameManagerRegistrar::clear() {
    gameManagers.clear();
}

const GameManagerRegistrar::GameManagerEntry* GameManagerRegistrar::findByName(const std::string& name) const {
    for (const auto& entry : gameManagers) {
        if (entry.name() == name) {
            return &entry;
        }
    }
    return nullptr;
}