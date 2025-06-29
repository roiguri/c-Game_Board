#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <cassert>

//-------------------------------
// common/ActionRequest.h
//-------------------------------

enum class ActionRequest { 
    MoveForward, MoveBackward,
    RotateLeft90, RotateRight90, RotateLeft45, RotateRight45,
    Shoot, GetBattleInfo, DoNothing
};

//-------------------------------
// common/BattleInfo.h
//-------------------------------

class BattleInfo {
public:
	virtual ~BattleInfo() {}
};

//-------------------------------
// common/TankAlgorithm.h
//-------------------------------

class TankAlgorithm {
public:
	virtual ~TankAlgorithm() {}
    virtual ActionRequest getAction() = 0;
    virtual void updateBattleInfo(BattleInfo& info) = 0;
};

using TankAlgorithmFactory =
std::function<std::unique_ptr<TankAlgorithm>(int player_index, int tank_index)>;

//-------------------------------
// common/SatelliteView.h
//-------------------------------

class SatelliteView {
public:
	virtual ~SatelliteView() {}
    virtual char getObjectAt(size_t x, size_t y) const = 0;
};

//-------------------------------
// common/Player.h
//-------------------------------

class Player {
public:
	virtual ~Player() {}
    virtual void updateTankWithBattleInfo
        (TankAlgorithm& tank, SatelliteView& satellite_view) = 0;
};

using PlayerFactory = 
std::function<std::unique_ptr<Player>
(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells)>;


//-------------------------------
// common/GameResult.h
//-------------------------------

struct GameResult {
	int winner; // 0 = tie
	enum Reason { ALL_TANKS_DEAD, MAX_STEPS, ZERO_SHELLS };
    Reason reason;
    std::vector<size_t> remaining_tanks; // index 0 = player 1, etc.
};

//-------------------------------
// common/AbstractGameManager.h
//-------------------------------

class AbstractGameManager {
public:
	virtual ~AbstractGameManager() {}
    virtual GameResult run(
    size_t map_width, size_t map_height,
    	SatelliteView& map, // <= assume it is a snapshot, NOT updated
    size_t max_steps, size_t num_shells,
    Player& player1, Player& player2,
    TankAlgorithmFactory player1_tank_algo_factory,
    TankAlgorithmFactory player2_tank_algo_factory) = 0;
};

//-------------------------------
// common/PlayerRegistration.h
//-------------------------------

struct PlayerRegistration {
  PlayerRegistration(PlayerFactory);
};

#define REGISTER_PLAYER(class_name) \
PlayerRegistration register_me_##class_name \
	( [] (int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells) { \
        return std::make_unique<class_name>(player_index, x, y, max_steps, num_shells); \
    } );

//-------------------------------------
// common/TankAlgorithmRegistration.h
//-------------------------------------

struct TankAlgorithmRegistration {
  TankAlgorithmRegistration(TankAlgorithmFactory);
};

#define REGISTER_TANK_ALGORITHM(class_name) \
TankAlgorithmRegistration register_me_##class_name \
	( [](int player_index, int tank_index) { \
	    return std::make_unique<class_name>(player_index, tank_index); } \
	);

//-------------------------------------
// common/GameManagerRegistration.h
//-------------------------------------

struct GameManagerRegistration {
  GameManagerRegistration(std::function<std::unique_ptr<AbstractGameManager>()>);
};

#define REGISTER_GAME_MANAGER(class_name) \
GameManagerRegistration register_me_##class_name \
        ( [] (bool verbose) { return std::make_unique<class_name>(verbose); } );

//===========================================
// From here and below it is all yours!
// Below is Proposed code!! Not mandatory!!
//===========================================

//-------------------------------
// simulator/AlgorithmRegistrar.h
//-------------------------------

class AlgorithmRegistrar {
    class AlgorithmAndPlayerFactories {
        std::string so_name;
        TankAlgorithmFactory tankAlgorithmFactory;
        PlayerFactory playerFactory;
    public:
        AlgorithmAndPlayerFactories(const std::string& so_name) : so_name(so_name) {}
        void setTankAlgorithmFactory(TankAlgorithmFactory&& factory) {
            assert(tankAlgorithmFactory == nullptr);
            tankAlgorithmFactory = std::move(factory);
        }
        void setPlayerFactory(PlayerFactory&& factory) {
            assert(playerFactory == nullptr);
            playerFactory = std::move(factory);
        }
        const std::string& name() const { return so_name; }
        std::unique_ptr<Player> createPlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells) const {
            return playerFactory(player_index, x, y, max_steps, num_shells);
        }
        std::unique_ptr<TankAlgorithm> createTankAlgorithm(int player_index, int tank_index) const {
            return tankAlgorithmFactory(player_index, tank_index);
        }
        bool hasPlayerFactory() const {
            return playerFactory != nullptr;
        }
        bool hasTankAlgorithmFactory() const {
            return tankAlgorithmFactory != nullptr;
        }
    };
    std::vector<AlgorithmAndPlayerFactories> algorithms;
    static AlgorithmRegistrar registrar;
public:
    static AlgorithmRegistrar& getAlgorithmRegistrar();
    void createAlgorithmFactoryEntry(const std::string& name) {
        algorithms.emplace_back(name);
    }
    void addPlayerFactoryToLastEntry(PlayerFactory&& factory) {
        algorithms.back().setPlayerFactory(std::move(factory));
    }
    void addTankAlgorithmFactoryToLastEntry(TankAlgorithmFactory&& factory) {
        algorithms.back().setTankAlgorithmFactory(std::move(factory));
    }
    struct BadRegistrationException {
        std::string name;
        bool hasName, hasPlayerFactory, hasTankAlgorithmFactory;
    };
    void validateLastRegistration() {
        const auto& last = algorithms.back();
        bool hasName = (last.name() != "");
        if(!hasName || !last.hasPlayerFactory() || !last.hasTankAlgorithmFactory() ) {
            throw BadRegistrationException{
                .name = last.name(),
                .hasName = hasName,
                .hasPlayerFactory = last.hasPlayerFactory(),
                .hasTankAlgorithmFactory = last.hasTankAlgorithmFactory()
            };
        }
    }
    void removeLast() {
        algorithms.pop_back();
    }
    auto begin() const {
        return algorithms.begin();
    }
    auto end() const {
        return algorithms.end();
    }
    std::size_t count() const { return algorithms.size(); }
    void clear() { algorithms.clear(); }
};

//-----------------------------------
// simulator/AlgorithmRegistrar.cpp
//-----------------------------------
AlgorithmRegistrar AlgorithmRegistrar::registrar;

AlgorithmRegistrar& AlgorithmRegistrar::getAlgorithmRegistrar() {
    return registrar;
}

//----------------------------------
// Simulator/PlayerRegistration.cpp
//----------------------------------

PlayerRegistration::PlayerRegistration(PlayerFactory factory) {
    auto& regsitrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    regsitrar.addPlayerFactoryToLastEntry(std::move(factory));
}

//----------------------------------------
// Simulator/TankAlgorithmRegistration.cpp
//----------------------------------------

TankAlgorithmRegistration::TankAlgorithmRegistration(TankAlgorithmFactory factory) {
    auto& regsitrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    regsitrar.addTankAlgorithmFactoryToLastEntry(std::move(factory));
}

//--------------------------------------
// algorithm/TankAlgorithm_123456789.h
//--------------------------------------

namespace Algorithm_123456789 {

class TankAlgorithm_123456789 : public TankAlgorithm {
public:
    TankAlgorithm_123456789(int, int) {}
    ActionRequest getAction() override {
        return ActionRequest::Shoot;
    }
    void updateBattleInfo(BattleInfo&) override {}
};

}

//-------------------------------
// algorithm/TankAlgorithm_123456789.cpp
//-------------------------------
// in your code, this should not be commented!
// using namespace Algorithm_123456789;
// REGISTER_TANK_ALGORITHM(TankAlgorithm_123456789);

//-------------------------------
// algorithm/Player_123456789.h
//-------------------------------

namespace Algorithm_123456789 {

class Player_123456789: public Player {
public:
    Player_123456789(int, size_t, size_t, size_t, size_t) {}
    void updateTankWithBattleInfo
        (TankAlgorithm&, SatelliteView&) override {
    }
};

}

//-------------------------------
// algorithm/Player_123456789.cpp
//-------------------------------
// in your code, this should not be commented!
// using namespace Algorithm_123456789;
// REGISTER_PLAYER(Player_123456789);

//-------------------------------
// main.cpp
//-------------------------------

// to fake a call to dlopen that actually creates the registration
void fake_dlopen_algo_so(const std::string& name) {
    using namespace Algorithm_123456789;
    REGISTER_TANK_ALGORITHM(TankAlgorithm_123456789);
    if(name != "Bad") {
        REGISTER_PLAYER(Player_123456789);
    }
}

int main() {
    //-----
    // NOTE:
    // This should NOT be in your main!!!
    // It should be somewhere in your Simulator class
    //-----
    // dlopen - we have the name of the so
    // let's assume that the so name is "Algorithm_123456789.so"
    // strip the so suffix, so name is "Algorithm_123456789"
    std::vector<std::string> names = {"Algorithm_123456789", "Bad", "Algorithm_123456781"};
    auto& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    for(const auto& name: names) {
        registrar.createAlgorithmFactoryEntry(name);
        // TODO - actual dlopen, also: check dlopen for failure
        fake_dlopen_algo_so(name);
        try {
            registrar.validateLastRegistration();
        }
        catch(AlgorithmRegistrar::BadRegistrationException& e) {
            // TODO: report according to requirements
            std::cout << "---------------------------------" << std::endl;
            std::cout << "BadRegistrationException for: " << name << std::endl;
            std::cout << "Name as registered: " << e.name << std::endl;
            std::cout << "Has tank algorithm factory? " << std::boolalpha << e.hasTankAlgorithmFactory << std::endl;
            std::cout << "Has Player factory? " << std::boolalpha << e.hasPlayerFactory << std::endl;
            std::cout << "---------------------------------" << std::endl;
            registrar.removeLast();
        }
    }
    // loop over all factories to do something
    for(const auto& algo: AlgorithmRegistrar::getAlgorithmRegistrar()) {
        auto algorithm = algo.createTankAlgorithm(1, 0);
        std::cout << algo.name() << ": " << static_cast<int>(algorithm->getAction()) << std::endl;
    }
    AlgorithmRegistrar::getAlgorithmRegistrar().clear();
    // dlclose
    // TODO handle dlclose
}
