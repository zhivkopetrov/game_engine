//Corresponding header
#include "game_engine/engine/Engine.h"

//System headers
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

ErrorCode Engine::init(const EngineConfig &engineCfg) {
  using namespace std::placeholders;

  if (ErrorCode::SUCCESS != Rng::getInstance().init()) {
    LOGERR("Error in Rng.init()");
    return ErrorCode::FAILURE;
  }

  if (ErrorCode::SUCCESS != _managerHandler.init(engineCfg.managerHandlerCfg)) {
    LOGERR("Error in _managerHandler.init()");
    return ErrorCode::FAILURE;
  }
  gDrawMgr->setMaxFrameRate(engineCfg.maxFrameRate);
  gDrawMgr->setSDLContainers(gRsrcMgr);

  if (ErrorCode::SUCCESS != _actionEventHandler.init(
          std::bind(&Engine::handleEvent, this, _1))) {
    LOGERR("Error in _actionEventHandler.init()");
    return ErrorCode::FAILURE;
  }

  if (ErrorCode::SUCCESS != _debugConsole.init(engineCfg.debugConsoleConfig)) {
    LOGERR("Error in _debugConsole.init()");
    return ErrorCode::FAILURE;
  }

  _game.setInvokeActionEventCb(
      std::bind(&ActionEventHandler::invokeActionEvent, &_actionEventHandler,
          _1, _2));

  _game.setSystemShutdownCb(std::bind(&Engine::shutdown, this));

  _game.setTakeScreenshotCb(
      std::bind(&DrawMgr::takeScreenshot, gDrawMgr, _1, _2, _3));

  return ErrorCode::SUCCESS;
}

ErrorCode Engine::recover() {
  return ErrorCode::SUCCESS;
}

void Engine::deinit() {
  _managerHandler.deinit();
  _actionEventHandler.deinit();
}

ErrorCode Engine::start() {
  std::thread updateThread = std::thread(&Engine::mainLoop, this);

  //communicator will probably spawn own thread and control will return
  _communicator.start();

  //blocking call
  gDrawMgr->startRenderingLoop();

  updateThread.join();
  return ErrorCode::SUCCESS;
}

void Engine::shutdown() {
  LOGG("Initiating shutdown...");
  _isActive = false;
  _communicator.shutdown();
  _actionEventHandler.shutdown();
  gDrawMgr->shutdownRenderer();

  const double delayedFramesPercent = (100.0 * _fpsCounter.delayedFrames)
      / _fpsCounter.totalFrames;
  LOG("Engine Statistics: [Delayed Frames\\All frames]: [%lu\\%lu] - %.2f%%",
      _fpsCounter.delayedFrames, _fpsCounter.totalFrames, delayedFramesPercent);
}

void Engine::mainLoop() {
  //give some time to the main(rendering thread) to enter it's drawing loop
  using namespace std::literals;
  std::this_thread::sleep_for(2ms);
  Time fpsTime;

  gTimerMgr->onInitEnd();

  while (_isActive) {
    fpsTime.getElapsed(); //begin measure the new frame elapsed time
    ++_fpsCounter.totalFrames;

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
    ++_fpsCounter.delayedFrames;

    //TODO figure out why alt-tab fullscreen is causing false-positive
    LOGY(
        "Warning, FPS drop. Frame [%lu] delayed with: (%ld us). No action " "events will be processed on this frame",
        _fpsCounter.totalFrames, (-1 * frameTimeLeftMicroseconds));
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

