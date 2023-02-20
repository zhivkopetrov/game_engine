//Corresponding header
#include "game_engine/engine/Engine.h"

//System headers
#include <string>
#include <chrono>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

//Other libraries headers
#include "manager_utils/managers/DrawMgr.h"
#include "manager_utils/managers/RsrcMgr.h"
#include "manager_utils/managers/TimerMgr.h"
#include "utils/input/InputEvent.h"
#include "utils/rng/Rng.h"
#include "utils/ErrorCode.h"
#include "utils/Log.h"

//Own components headers
#include "game_engine/communicator/Communicator.h"
#include "game_engine/Game.h"

using namespace std::literals;

Engine::Engine(Communicator &communicator, Game &game)
    : _communicator(communicator), _game(game) {

}

ErrorCode Engine::init(const EngineConfig &cfg) {
  using namespace std::placeholders;

  if (ErrorCode::SUCCESS != Rng::getInstance().init()) {
    LOGERR("Error in Rng.init()");
    return ErrorCode::FAILURE;
  }

  if (ErrorCode::SUCCESS != _managerHandler.init(cfg.managerHandlerCfg)) {
    LOGERR("Error in _managerHandler.init()");
    return ErrorCode::FAILURE;
  }
  gDrawMgr->setMaxFrameRate(cfg.maxFrameRate);
  gDrawMgr->setSDLContainers(gRsrcMgr);

  if (ErrorCode::SUCCESS != _debugConsole.init(cfg.debugConsoleConfig)) {
    LOGERR("Error in _debugConsole.init()");
    return ErrorCode::FAILURE;
  }

  if (ErrorCode::SUCCESS != _actionEventHandler.init(
      cfg.actionEventHandlerPolicy)) {
    LOGERR("Error in _actionEventHandler.init()");
    return ErrorCode::FAILURE;
  }

  const InvokeActionEventCb invokeActionEventCb = 
    std::bind(&ActionEventHandler::invokeActionEvent, &_actionEventHandler,
          _1, _2);
  const HandleInputEventCb handleInputEventCb = 
    std::bind(&Engine::handleEvent, this, _1);

  if (ErrorCode::SUCCESS != _inputEventHandler.init(
      cfg.inputEventHandlerPolicy, invokeActionEventCb, handleInputEventCb)) {
    LOGERR("Error in _inputEventHandler.init()");
    return ErrorCode::FAILURE;
  }

  _game.setInvokeActionEventCb(invokeActionEventCb);
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
  _inputEventHandler.deinit();
  _actionEventHandler.deinit();
}

ErrorCode Engine::start() {
  //communicator will spawn own thread and control will return
  _communicator.start();

  const RendererPolicy rendererPolicy = gDrawMgr->getRendererPolicy();
  if (RendererPolicy::SINGLE_THREADED == rendererPolicy) {
    LOG("Using SINGLE_THREADED Renderer execution policy");
    //blocking call
    mainLoop();
  } else {
    LOG("Using MULTI_THREADED Renderer execution policy");
    std::thread updateThread = std::thread(&Engine::mainLoop, this);

    /** Blocking call.
      * Rendering operations MUST be performed on the main thread
      * to guarantee cross-platform support */
    gDrawMgr->startRenderingLoop();

    updateThread.join();
  }

  return ErrorCode::SUCCESS;
}

void Engine::shutdown() {
  LOGG("Initiating shutdown...");
  _isActive = false;
  _inputEventHandler.shutdown();
  _actionEventHandler.shutdown();
  gDrawMgr->shutdownRenderer();

  const double delayedFramesPercent = (100.0 * _fpsCounter.delayedFrames)
      / _fpsCounter.totalFrames;
  LOG("Engine Statistics: [Delayed Frames\\All frames]: [%" PRIu64"\\%" PRIu64"] - %.2f%%",
      _fpsCounter.delayedFrames, _fpsCounter.totalFrames, delayedFramesPercent);

#ifdef __EMSCRIPTEN__
  emscripten_cancel_main_loop();
#endif
}

void Engine::mainLoop() {
  mainLoopPrepare();

  #ifdef __EMSCRIPTEN__
    const int32_t requestedFPS = 
      static_cast<int32_t>(gDrawMgr->getMaxFrameRate());
    constexpr bool simulateInfiniteLoop = true;
    void* userData = this;

    emscripten_set_main_loop_arg([](void* userData){
      Engine& engine = *static_cast<Engine*>(userData);
      engine.mainLoopInternal();
    }, userData, requestedFPS, simulateInfiniteLoop);
#else
  mainLoopInternal();
#endif
}

void Engine::mainLoopInternal() {
  Time fpsTime;
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

void Engine::mainLoopPrepare() {
  Time time;
  //give some time to the main(rendering thread) to enter it's drawing loop
  std::this_thread::sleep_for(2ms);

  //manually swap the rendering back buffer in order to process all stored
  //renderer commands during initialization before the main loop is started
  gDrawMgr->finishFrame(); //load the stored renderer commands in the renderer
  gDrawMgr->finishFrame(); //wait for the commands to execute

  //base point for all timers started during initialization
  gTimerMgr->onInitEnd();

  LOG("Engine mainLoopPrepare() took: [%" PRId64" ms]",
      time.getElapsed().toMilliseconds());
}

void Engine::process() {
  _managerHandler.process();
  _inputEventHandler.process();
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
    LOGY("Warning, FPS drop. Frame [%" PRIu64"] delayed with: (%" PRId64" us). "
         "Will process actions events only for 1000us this frame",
        _fpsCounter.totalFrames, (-1 * frameTimeLeftMicroseconds));

    //process events for 1000 us, even though frame is lagging behind
    _actionEventHandler.processStoredEvents(1000us);
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
