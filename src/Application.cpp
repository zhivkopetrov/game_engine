//Corresponding header
#include "game_engine/Application.h"

//C system headers

//C++ system headers

//Other libraries headers
#include "sdl_utils/SDLLoader.h"
#include "utils/debug/SignalHandler.h"
#include "utils/ErrorCode.h"
#include "utils/Log.h"

//Own components headers

Application::Application(std::unique_ptr<Game> game)
    : _game(std::move(game)) {

}

Application::~Application() noexcept {
  deinit();

  printUptime();
}

int32_t Application::init(const ApplicationConfig& cfg) {
  if (SUCCESS != loadDependencies(cfg.argc, cfg.args)) {
    LOGERR("Error in loadDependencies");
    return FAILURE;
  }

  _engine = std::make_unique<Engine>(*_game.get());

  if (SUCCESS != _engine->init(cfg.engineCfg)) {
    LOGERR("Error in _engine.init()");
    return FAILURE;
  }

  if (SUCCESS != _engine->recover()) {
    LOGERR("Error in _engine.recover()");
    return FAILURE;
  }

  if (SUCCESS != _game->init(cfg.gameCfg)) {
    LOGERR("Error in _game.init()");
    return FAILURE;
  }

  return SUCCESS;
}

int32_t Application::run() {
  return _engine->start();
}

int32_t Application::loadDependencies([[maybe_unused]]int32_t argc,
                                      [[maybe_unused]]char **args) {
  //install user defined signal handlers
  SignalHandler::installSignal(SIGSEGV);
  SignalHandler::installSignal(SIGQUIT);

  //open SDL libraries
  if (SUCCESS != SDLLoader::init()) {
    LOGERR("Error in SDLLoader::init() -> Terminating ...");
    return FAILURE;
  } else {
    LOGG("SDLLoader::init() took: %ld ms", _time.getElapsed().toMilliseconds());
  }

  return SUCCESS;
}

void Application::unloadDependencies() {
  //close SDL libraries
  SDLLoader::deinit();
}

void Application::deinit() {
  _game->deinit();
  _game.reset();

  _engine->deinit();
  _engine.reset();

  unloadDependencies();
}

void Application::printUptime() {
  const auto totalElapsedSeconds = _time.getElapsed().toSeconds();
  auto remainingSeconds = totalElapsedSeconds;

  constexpr auto secondsInAnHour = 3600;
  const auto elapsedHours = remainingSeconds / secondsInAnHour;
  remainingSeconds -= (elapsedHours * secondsInAnHour);

  constexpr auto secondsInAnMinute = 60;
  const auto elapsedMinutes = remainingSeconds / secondsInAnMinute;
  remainingSeconds -= (elapsedMinutes * secondsInAnMinute);

  const auto elapsedSeconds = remainingSeconds;

  LOGG("Total engine uptime: Hours: %ld, Minutes: %ld, Seconds: %ld",
      elapsedHours, elapsedMinutes, elapsedSeconds);
}

