//Corresponding header
#include "game_engine/Application.h"

//System headers

//Other libraries headers
#include "game_engine/communicator/NullCommunicator.h"
#include "utils/debug/SignalHandler.h"
#include "utils/ErrorCode.h"
#include "utils/Log.h"

//Own components headers

Application::~Application() noexcept {
  deinit();
  unloadDependencies();
  LOGG("Shutdown complete");
  printUptime();
}

ErrorCode Application::loadDependencies(
    const std::vector<DependencyDescription> &dependencies) {
  _dependencies = dependencies;
  for (const auto& [name, loadDependency, unloadDependency] : _dependencies) {
    LOG_ON_SAME_LINE("Loading dependency '%s' ... [", name.c_str());
    const ErrorCode errCode = loadDependency();
    if (ErrorCode::SUCCESS == errCode) {
      LOGG_ON_SAME_LINE("SUCCESS");
      LOG("]");
    } else {
      LOGR_ON_SAME_LINE("FAIL");
      LOG("]");
      LOGERR("loadDependency() failed for %s", name.c_str());
      return ErrorCode::FAILURE;
    }
  }

  return ErrorCode::SUCCESS;
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

ErrorCode Application::init(const ApplicationConfig &cfg) {
  _engine = std::make_unique<Engine>(*_communicator.get(), *_game.get());

  if (ErrorCode::SUCCESS != _engine->init(cfg.engineCfg)) {
    LOGERR("Error in _engine.init()");
    return ErrorCode::FAILURE;
  }

  if (ErrorCode::SUCCESS != _engine->recover()) {
    LOGERR("Error in _engine.recover()");
    return ErrorCode::FAILURE;
  }

  if (ErrorCode::SUCCESS != _communicator->init(cfg.communicatorCfg)) {
    LOGERR("Error in _communicator.init()");
    return ErrorCode::FAILURE;
  }

  if (ErrorCode::SUCCESS != _game->init(cfg.gameCfg)) {
    LOGERR("Error in _game.init()");
    return ErrorCode::FAILURE;
  }

  return ErrorCode::SUCCESS;
}

ErrorCode Application::run() {
  LOGG("Starting Application");
  return _engine->start();
}

void Application::deinit() {
  _game->deinit();
  _game.reset();

  //manually reset point after deinit to enforce destruction order
  _communicator->deinit();
  _communicator.reset();

  _engine->deinit();
  _engine.reset();
}

void Application::unloadDependencies() {
  //unload dependency in reverse order -> they might be dependent
  for (auto revIt = _dependencies.rbegin(); revIt != _dependencies.rend();
      ++revIt) {
    LOG_ON_SAME_LINE("Unloading dependency '%s' ... [", revIt->name.c_str());
    LOGG_ON_SAME_LINE("SUCCESS");
    LOG("]");

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

  LOG("Total Application uptime: Hours: "
      "%" PRId64", Minutes: %" PRId64", Seconds: %" PRId64"\n",
      elapsedHours, elapsedMinutes, elapsedSeconds);
}

