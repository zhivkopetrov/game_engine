#ifndef GAME_ENGINE_GAMEUTILITYDEFINES_H_
#define GAME_ENGINE_GAMEUTILITYDEFINES_H_

//System headers
#include <cstdint>
#include <functional>

//Other libraries headers
#include "sdl_utils/drawing/defines/DrawConstants.h"

//Own components headers

using TakeScreenshotCb = std::function<void(const char *file,
    const ScreenshotContainer container, const int32_t quality)>;

#endif /* GAME_ENGINE_GAMEUTILITYDEFINES_H_ */
