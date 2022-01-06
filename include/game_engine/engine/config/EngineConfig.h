#ifndef GAME_ENGINE_ENGINECONFIG_H_
#define GAME_ENGINE_ENGINECONFIG_H_

//C system headers

//C++ system headers
#include <string>

//Other libraries headers
#include "manager_utils/managers/config/ManagerHandlerConfig.h"

//Own components headers

//Forward declarations

struct EngineConfig {
  ManagerHandlerConfig managerHandlerCfg;
  uint64_t debugConsoleRsrcId = 0;
  uint32_t maxFrameRate = 0;
};

EngineConfig getDefaultEngineConfig(
    const std::string &projectFolderName,
    const std::string &loadingScreenResourcesPath = { });

#endif /* GAME_ENGINE_ENGINECONFIG_H_ */

