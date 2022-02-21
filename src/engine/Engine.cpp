//Corresponding header
#include "game_engine/engine/Engine.h"

//C system headers

//C++ system headers
#include <string>
#include <chrono>

//Other libraries headers
#include "manager_utils/managers/DrawMgr.h"
#include "manager_utils/managers/RsrcMgr.h"
#include "manager_utils/managers/TimerMgr.h"
#include "sdl_utils/input/InputEvent.h"
#include "utils/rng/Rng.h"
#include "utils/ErrorCode.h"
#include "utils/Log.h"

//Own components headers
#include "game_engine/communicator/Communicator.h"
#include "game_engine/Game.h"

Engine::Engine(Communicator &communicator, Game &game)
    : _communicator(communicator), _game(game) {

}

int32_t Engine::init(const EngineConfig &engineCfg) {
  using namespace std::placeholders;

  if (SUCCESS != Rng::getInstance().init()) {
    LOGERR("Error in Rng.init()");
    return FAILURE;
  }

  if (SUCCESS != _managerHandler.init(engineCfg.managerHandlerCfg)) {
    LOGERR("Error in _managerHandler.init()");
    return FAILURE;
  }
  gDrawMgr->setMaxFrameRate(engineCfg.maxFrameRate);
  gDrawMgr->setSDLContainers(gRsrcMgr);

  if (SUCCESS != _actionEventHandler.init(
          std::bind(&Engine::handleEvent, this, _1))) {
    LOGERR("Error in _actionEventHandler.init()");
    return FAILURE;
  }

  if (SUCCESS != _debugConsole.init(engineCfg.debugConsoleRsrcId,
          engineCfg.maxFrameRate)) {
    LOGERR("Error in _debugConsole.init()");
    return FAILURE;
  }

  _game.setInvokeActionEventCb(
      std::bind(&ActionEventHandler::invokeActionEvent, &_actionEventHandler,
          _1, _2));

  _game.setSystemShutdownCb(std::bind(&Engine::shutdown, this));

  return SUCCESS;
}

int32_t Engine::recover() {
  return SUCCESS;
}

void Engine::deinit() {
  _managerHandler.deinit();
  _actionEventHandler.deinit();
}

int32_t Engine::start() {
  std::thread updateThread = std::thread(&Engine::mainLoop, this);

  //communicator will probably spawn own thread and control will return
  _communicator.start();

  //blocking call
  gDrawMgr->startRenderingLoop();

  updateThread.join();
  return SUCCESS;
}

void Engine::shutdown() {
  LOGG("Initiating shutdown...");
  _isActive = false;
  _communicator.shutdown();
  _actionEventHandler.shutdown();
  gDrawMgr->shutdownRenderer();
}

void Engine::mainLoop() {
  //give some time to the main(rendering thread) to enter it's drawing loop
  using namespace std::literals;
  std::this_thread::sleep_for(2ms);
  Time fpsTime;

  gTimerMgr->onInitEnd();

  while (_isActive) {
    fpsTime.getElapsed(); //begin measure the new frame elapsed time

    process();
    drawFrame();

    const auto elapsedMiscroSeconds = fpsTime.getElapsed().toMicroseconds();
    if (_debugConsole.isActive()) {
      populateDebugConsole(elapsedMiscroSeconds);
    }

    processEvents(elapsedMiscroSeconds);
  }
}

void Engine::process() {
  _managerHandler.process();
  _game.process();
}

void Engine::drawFrame() {
  gDrawMgr->clearScreen();

  _game.draw();

  if (_debugConsole.isActive()) {
    _debugConsole.draw();
  }

  gDrawMgr->finishFrame();
}

void Engine::handleEvent(const InputEvent &e) {
  if (e.checkForExitRequest()) {
    shutdown();
    return;
  }

  _game.handleEvent(e);
  _debugConsole.handleEvent(e);
}

void Engine::processEvents(int64_t frameElapsedMicroseconds) {
  constexpr int64_t microsecondsInASeconds = 1000000;
  const int64_t maxMicrosecondsPerFrame = microsecondsInASeconds
      / gDrawMgr->getMaxFrameRate();

  const int64_t frameTimeLeftMicroseconds = maxMicrosecondsPerFrame
      - frameElapsedMicroseconds;

  if (0 >= frameTimeLeftMicroseconds) {
    //TODO figure out why alt-tab fullscreen is causing false-positive
    LOGY("Warning, FPS drop. Frame delayed with: (%ld us). No events will be "
        "processed on this frame", (-1 * frameTimeLeftMicroseconds));
    return;
  }

  //process events for the rest of the frame duration without throttling the CPU
  _actionEventHandler.processStoredEvents(
      std::chrono::microseconds(frameTimeLeftMicroseconds));
}

void Engine::populateDebugConsole(int64_t frameElapsedMicroseconds) {
  const DebugConsoleData debugData(frameElapsedMicroseconds,
      gTimerMgr->getActiveTimersCount(), gRsrcMgr->getGPUMemoryUsage(),
      gDrawMgr->getTotalWidgetCount());

  _debugConsole.update(debugData);
}

