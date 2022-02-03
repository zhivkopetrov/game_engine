#ifndef GAME_ENGINE_APPLICATION_H_
#define GAME_ENGINE_APPLICATION_H_

//C system headers

//C++ system headers
#include <cstdint>
#include <memory>

//Other libraries headers
#include "utils/class/NonCopyable.h"
#include "utils/class/NonMoveable.h"
#include "utils/time/Time.h"

//Own components headers
#include "game_engine/engine/Engine.h"
#include "game_engine/Game.h"

//Forward declarations

struct ApplicationConfig {
  EngineConfig engineCfg;
  std::any gameCfg;
  int32_t argc;
  char **args;
};

class Application : public NonCopyable, public NonMoveable {
public:
  Application(std::unique_ptr<Game> game);
  virtual ~Application() noexcept;

  int32_t init(const ApplicationConfig& cfg);

  int32_t run();

protected:
  virtual int32_t loadDependencies(int32_t argc, char **args);
  virtual void unloadDependencies();

private:
  void deinit();
  void printUptime();

  std::unique_ptr<Game> _game = nullptr;
  std::unique_ptr<Engine> _engine = nullptr;

  //used to measure engine init and total uptime
  Time _time;
};

#endif /* GAME_ENGINE_APPLICATION_H_ */
