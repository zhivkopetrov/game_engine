#ifndef GAME_ENGINE_DEPENDENCYDEFINES_H_
#define GAME_ENGINE_DEPENDENCYDEFINES_H_

//C system headers

//C++ system headers
#include <functional>
#include <string>

//Other libraries headers

//Own components headers

//Forward declarations

using LoadDependencyCb = std::function<int32_t()>;
using UnloadDependencyCb = std::function<void()>;

struct DependencyDescription {
  std::string name;
  LoadDependencyCb loadDependencyCb;
  UnloadDependencyCb unloadDependencyCb;
};


#endif /* GAME_ENGINE_DEPENDENCYDEFINES_H_ */
