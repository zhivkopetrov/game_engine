#ifndef GAME_ENGINE_ENGINE_H_
#define GAME_ENGINE_ENGINE_H_

//C system headers

//C++ system headers
#include <cstdint>

//Other libraries headers
#include "manager_utils/managers/ManagerHandler.h"

//Own components headers
#include "game_engine/engine/config/EngineConfig.h"
#include "game_engine/event_handler/ActionEventHandler.h"
#include "game_engine/utils/DebugConsole.h"

//Forward declarations
class Game;
class InputEvent;

class Engine {
public:
  Engine(Game& game);

  int32_t init(const EngineConfig &engineCfg);
  void deinit();
  int32_t recover();
  int32_t start();
  void handleEvent(const InputEvent& e);

private:
  void mainLoop();
  void shutdown();
  void process(); //called once per frame
  void drawFrame();
  void processEvents(int64_t frameElapsedMicroseconds);
  void populateDebugConsole(int64_t frameElapsedMicroseconds);

  ManagerHandler _managerHandler;
  ActionEventHandler _actionEventHandler;
  DebugConsole _debugConsole;
  Game& _game;

  bool _isActive = true;
};

#endif /* GAME_ENGINE_ENGINE_H_ */
