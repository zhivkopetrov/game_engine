#ifndef GAME_ENGINE_APPLICATIONCONFIG_H_
#define GAME_ENGINE_APPLICATIONCONFIG_H_

//System headers
#include <cstdint>
#include <any>

//Other libraries headers

//Own components headers
#include "game_engine/engine/config/EngineConfig.h"

//Forward declarations

struct ApplicationConfig {
  EngineConfig engineCfg;
  std::any gameCfg;
  std::any communicatorCfg;
};

#endif /* GAME_ENGINE_APPLICATIONCONFIG_H_ */
