#ifndef GAME_ENGINE_ENGINE_H_
#define GAME_ENGINE_ENGINE_H_

//System headers
#include <cstdint>

//Other libraries headers
#include "manager_utils/managers/ManagerHandler.h"
#include "utils/ErrorCode.h"

//Own components headers
#include "game_engine/engine/config/EngineConfig.h"
#include "game_engine/event_handler/ActionEventHandler.h"
#include "game_engine/event_handler/InputEventHandler.h"
#include "game_engine/utils/DebugConsole.h"

//Forward declarations
class Game;
class Communicator;
class InputEvent;

class Engine {
public:
  Engine(Communicator& communicator, Game& game);

  ErrorCode init(const EngineConfig &cfg);
  void deinit();
  ErrorCode recover();
  ErrorCode start();
  void handleEvent(const InputEvent& e);
  void shutdown();

  void mainLoopInternal();

private:
  void mainLoop();
  void mainLoopPrepare();
  void process(); //called once per frame
  void drawFrame();
  void processEvents(int64_t frameElapsedMicroseconds);
  void populateDebugConsole(int64_t frameElapsedMicroseconds);

  struct FpsCounter {
    uint64_t totalFrames { };
    uint64_t delayedFrames { };
  };

  ManagerHandler _managerHandler;
  ActionEventHandler _actionEventHandler;
  InputEventHandler _inputEventHandler;
  DebugConsole _debugConsole;
  FpsCounter _fpsCounter;

  Communicator& _communicator;
  Game& _game;

  bool _isActive = true;
};

#endif /* GAME_ENGINE_ENGINE_H_ */
