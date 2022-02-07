#ifndef GAME_ENGINE_APPLICATIONCONFIG_H_
#define GAME_ENGINE_APPLICATIONCONFIG_H_

//C system headers

//C++ system headers
#include <cstdint>
#include <vector>
#include <any>

//Other libraries headers

//Own components headers
#include "game_engine/engine/config/EngineConfig.h"

//Forward declarations

struct ApplicationConfig {
  EngineConfig engineCfg;
  std::any gameCfg;
};

#endif /* GAME_ENGINE_APPLICATIONCONFIG_H_ */
