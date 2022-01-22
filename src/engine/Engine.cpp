//Corresponding header
#include "game_engine/engine/Engine.h"

//C system headers

//C++ system headers
#include <string>
#include <thread>
#include <chrono>

//Other libraries headers
#include "manager_utils/managers/DrawMgr.h"
#include "manager_utils/managers/RsrcMgr.h"
#include "manager_utils/managers/TimerMgr.h"
#include "utils/rng/Rng.h"
#include "utils/ErrorCode.h"
#include "utils/Log.h"

//Own components headers
#include "game_engine/Game.h"

Engine::Engine(Game& game) : _game(game) {

}

Engine::~Engine() {
  deinit();
}

int32_t Engine::init(const EngineConfig &engineCfg) {
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

  if (SUCCESS != _inputEvent.init()) {
    LOGERR("Error in _inputEvent.init()");
    return FAILURE;
  }

  if (SUCCESS != _debugConsole.init(engineCfg.debugConsoleRsrcId,
          engineCfg.maxFrameRate)) {
    LOGERR("Error in _debugConsole.init()");
    return FAILURE;
  }

  return SUCCESS;
}

int32_t Engine::recover() {
  return SUCCESS;
}

void Engine::mainLoop() {
  //give some time to the main(rendering thread) to enter it's drawing loop
  using namespace std::literals;
  std::this_thread::sleep_for(1s);
  Time fpsTime;

  gTimerMgr->onInitEnd();

  while (true) {
    fpsTime.getElapsed(); //begin measure the new frame elapsed time

    if (processFrame()) {
      //user has requested exit -> break the main loop
      gDrawMgr->shutdownRenderer();
      return;
    }

    const auto elapsedMiscroSeconds = fpsTime.getElapsed().toMicroseconds();
    if (_debugConsole.isActive()) {
      populateDebugConsole(elapsedMiscroSeconds);
    }

#if !ENABLE_VSYNC
    limitFPS(elapsedMiscroSeconds);
#endif //!ENABLE_VSYNC
  }
}

void Engine::deinit() {
  _managerHandler.deinit();
  _inputEvent.deinit();
}

int32_t Engine::start() {
  std::thread engineThread = std::thread(&Engine::mainLoop, this);

  //enter rendering loop
  gDrawMgr->startRenderingLoop();

  //sanity check
  if (engineThread.joinable()) {
    engineThread.join();
  }

  return SUCCESS;
}

bool Engine::processFrame() {
  _managerHandler.process();

  while (_inputEvent.pollEvent()) {
    if (_inputEvent.checkForExitRequest()) {
      return true;
    }

    handleEvent();
  }

  drawFrame();

  return false;
}

void Engine::drawFrame() {
  gDrawMgr->clearScreen();

  _game.draw();

  if (_debugConsole.isActive()) {
    _debugConsole.draw();
  }

  gDrawMgr->finishFrame();
}

void Engine::handleEvent() {
  _game.handleEvent(_inputEvent);
  _debugConsole.handleEvent(_inputEvent);
}

void Engine::populateDebugConsole(const int64_t elapsedMiscroSeconds) {
  const DebugConsoleData debugData (elapsedMiscroSeconds,
      gTimerMgr->getActiveTimersCount(), gRsrcMgr->getGPUMemoryUsage(),
      gDrawMgr->getTotalWidgetCount());

  _debugConsole.update(debugData);
}

void Engine::limitFPS(const int64_t elapsedMicroseconds) {
  constexpr auto microsecondsInASeconds = 1000000;
  const auto maxMicrosecondsPerFrame = microsecondsInASeconds
      / gDrawMgr->getMaxFrameRate();

  const auto microSecondsFpsDelay =
      maxMicrosecondsPerFrame - elapsedMicroseconds;

  if (0 < microSecondsFpsDelay) {
    //Sleep the logic thread if max FPS is reached.
    //No need to struggle the CPU.
    std::this_thread::sleep_for(
        std::chrono::microseconds(microSecondsFpsDelay));
  }
}

