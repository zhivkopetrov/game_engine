#ifndef GAME_ENGINE_ENGINECONFIG_H_
#define GAME_ENGINE_ENGINECONFIG_H_

//C system headers

//C++ system headers

//Other libraries headers
#include "manager_utils/managers/config/ManagerHandlerConfig.h"

//Own components headers

//Forward declarations

struct EngineConfig {
  ManagerHandlerConfig managerHandlerCfg;
  uint64_t debugConsoleRsrcId = 0;
  uint32_t maxFrameRate = 0;
};

#endif /* GAME_ENGINE_ENGINECONFIG_H_ */

