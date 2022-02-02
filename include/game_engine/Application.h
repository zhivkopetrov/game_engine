#ifndef GAME_ENGINE_APPLICATION_H_
#define GAME_ENGINE_APPLICATION_H_

//C system headers

//C++ system headers
#include <cstdint>
#include <memory>

//Other libraries headers
#include "utils/time/Time.h"

//Own components headers
#include "game_engine/engine/Engine.h"
#include "game_engine/Game.h"

//Forward declarations

class Application {
public:
  Application(const std::shared_ptr<Game>& game);
  ~Application();

  int32_t init(const EngineConfig &engineCfg, const std::any& gameCfg);

  int32_t run();

private:
  void deinit();

  int32_t loadDependencies();
  void unloadDependencies();

  std::shared_ptr<Game> _game = nullptr;
  std::unique_ptr<Engine> _engine = nullptr;

  //used to measure engine init and total uptime
  Time _time;
};

#endif /* GAME_ENGINE_APPLICATION_H_ */
