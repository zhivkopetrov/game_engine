#ifndef GAME_ENGINE_APPLICATION_H_
#define GAME_ENGINE_APPLICATION_H_

//System headers
#include <cstdint>
#include <memory>

//Other libraries headers
#include "utils/class/NonCopyable.h"
#include "utils/class/NonMoveable.h"
#include "utils/time/Time.h"
#include "utils/ErrorCode.h"

//Own components headers
#include "game_engine/config/ApplicationConfig.h"
#include "game_engine/engine/Engine.h"
#include "game_engine/communicator/Communicator.h"
#include "game_engine/Game.h"

//Forward declarations

class Application: public NonCopyable, public NonMoveable {
public:
  virtual ~Application() noexcept;

  ErrorCode loadDependencies(
      const std::vector<DependencyDescription> &dependencies);

  void obtain(std::unique_ptr<Game> game,
              std::unique_ptr<Communicator> communicator = nullptr);

  ErrorCode init(const ApplicationConfig &cfg);

  ErrorCode run();

private:
  void deinit();
  void unloadDependencies();
  void printUptime();

  std::vector<DependencyDescription> _dependencies;

  std::unique_ptr<Engine> _engine;
  std::unique_ptr<Communicator> _communicator;
  std::unique_ptr<Game> _game;

  //used to measure engine init and total uptime
  Time _time;
};

#endif /* GAME_ENGINE_APPLICATION_H_ */
