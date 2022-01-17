#ifndef GAME_ENGINE_ENGINE_H_
#define GAME_ENGINE_ENGINE_H_

//C system headers

//C++ system headers
#include <cstdint>

//Other libraries headers
#include "manager_utils/managers/ManagerHandler.h"
#include "sdl_utils/input/InputEvent.h"

//Own components headers
#include "game_engine/engine/config/EngineConfig.h"
#include "game_engine/engine/DebugConsole.h"

//Forward declarations
class Game;

class Engine {
public:
  Engine(Game& game);
  ~Engine();

  int32_t init(const EngineConfig &engineCfg);

  void deinit();

  int32_t recover();

  int32_t start();

private:
  void mainLoop();

  bool processFrame();

  void drawFrame();

  void handleEvent();

  void populateDebugConsole(const int64_t elapsedMicroseconds);

  void limitFPS(const int64_t elapsedTime);

  ManagerHandler _managerHandler;
  InputEvent _inputEvent;
  DebugConsole _debugConsole;

  Game& _game;
};

#endif /* GAME_ENGINE_ENGINE_H_ */
