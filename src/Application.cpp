//Corresponding header
#include "game_engine/Application.h"

//C system headers

//C++ system headers

//Other libraries headers
#include "game_engine/communicator/NullCommunicator.h"
#include "utils/debug/SignalHandler.h"
#include "utils/ErrorCode.h"
#include "utils/Log.h"

//Own components headers

Application::~Application() noexcept {
  deinit();
  unloadDependencies();
  LOG("Shutdown complete");
  printUptime();
}

int32_t Application::loadDependencies(
    const std::vector<DependencyDescription> &dependencies) {
  _dependencies = dependencies;
  for (const auto& [name, loadDependency, unloadDependency] : _dependencies) {
    if (SUCCESS != loadDependency()) {
      LOGERR("loadDependency() failed for %s", name.c_str());
      return FAILURE;
    }
  }

  return SUCCESS;
}

void Application::obtain(std::unique_ptr<Game> game,
                         std::unique_ptr<Communicator> communicator) {
  _game = std::move(game);

  if (communicator) {
    _communicator = std::move(communicator);
  } else {
    _communicator = std::make_unique<NullCommunicator>();
  }
}

int32_t Application::init(const ApplicationConfig &cfg) {
  _engine = std::make_unique<Engine>(*_communicator.get(), *_game.get());

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

  if (SUCCESS != _communicator->init(cfg.communicatorCfg)) {
    LOGERR("Error in _communicator.init()");
    return FAILURE;
  }

  return SUCCESS;
}

int32_t Application::run() {
  LOGG("Starting Application");
  return _engine->start();
}

void Application::deinit() {
  //manually reset point after deinit to enforce destruction order
  _communicator->deinit();
  _communicator.reset();

  _game->deinit();
  _game.reset();

  _engine->deinit();
  _engine.reset();
}

void Application::unloadDependencies() {
  //unload dependency in reverse order -> they might be dependant
  for (auto revIt = _dependencies.rbegin(); revIt != _dependencies.rend();
      ++revIt) {
    revIt->unloadDependencyCb();
  }
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

  LOG("Total Application uptime: Hours: %ld, Minutes: %ld, Seconds: %ld\n",
      elapsedHours, elapsedMinutes, elapsedSeconds);
}

