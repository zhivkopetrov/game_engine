#ifndef GAME_ENGINE_DEBUGCONSOLECONFIG_H_
#define GAME_ENGINE_DEBUGCONSOLECONFIG_H_

//System headers
#include <cstdint>

//Other libraries headers
#include "utils/drawing/Color.h"

//Own components headers

//Forward declarations

struct DebugConsoleConfig {
  uint64_t fontRsrcId {};
  int64_t maxFrameRate {};
  Color textColor = Colors::CYAN;
};

#endif /* GAME_ENGINE_DEBUGCONSOLECONFIG_H_ */
