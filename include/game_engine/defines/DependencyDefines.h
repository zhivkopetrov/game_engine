#ifndef GAME_ENGINE_DEPENDENCYDEFINES_H_
#define GAME_ENGINE_DEPENDENCYDEFINES_H_

//System headers
#include <functional>
#include <string>

//Other libraries headers
#include "utils/ErrorCode.h"

//Own components headers

//Forward declarations

using LoadDependencyCb = std::function<ErrorCode()>;
using UnloadDependencyCb = std::function<void()>;

struct DependencyDescription {
  std::string name;
  LoadDependencyCb loadDependencyCb;
  UnloadDependencyCb unloadDependencyCb;
};


#endif /* GAME_ENGINE_DEPENDENCYDEFINES_H_ */
