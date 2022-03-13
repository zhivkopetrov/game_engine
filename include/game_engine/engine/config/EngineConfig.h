#ifndef GAME_ENGINE_ENGINECONFIG_H_
#define GAME_ENGINE_ENGINECONFIG_H_

//System headers
#include <string>

//Other libraries headers
#include "manager_utils/managers/config/ManagerHandlerConfig.h"

//Own components headers
#include "game_engine/defines/DependencyDefines.h"

//Forward declarations

struct EngineConfig {
  ManagerHandlerConfig managerHandlerCfg;
  uint64_t debugConsoleRsrcId = 0;
  uint32_t maxFrameRate = 0;
};

EngineConfig getDefaultEngineConfig(
    const std::string &projectInstallPrefix,
    const std::string &loadingScreenResourcesPath = { });

std::vector<DependencyDescription> getDefaultEngineDependencies(int32_t argc,
                                                                char **args);

#endif /* GAME_ENGINE_ENGINECONFIG_H_ */

