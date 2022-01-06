#ifndef GAME_ENGINE_DEBUGCONSOLE_H_
#define GAME_ENGINE_DEBUGCONSOLE_H_

//C system headers

//C++ system headers
#include <cstdint>

//Other libraries headers
#include "manager_utils/drawing/Text.h"

//Own components headers

//Forward declarations
class InputEvent;

struct DebugConsoleData {
  explicit DebugConsoleData(int64_t inputElapsedMicroSeconds,
                            uint64_t inputActiveTimers,
                            uint64_t inputGpuMemoryUsage,
                            uint32_t intpuActiveWidgets)
      : elapsedMicroSeconds(inputElapsedMicroSeconds),
        activeTimers(inputActiveTimers), gpuMemoryUsage(inputGpuMemoryUsage),
        activeWidgets(intpuActiveWidgets) {}

  int64_t elapsedMicroSeconds;
  uint64_t activeTimers;
  uint64_t gpuMemoryUsage;
  uint32_t activeWidgets;
};

class DebugConsole {
public:
  DebugConsole();

  int32_t init(const uint64_t fontRsrcId, const int64_t maxFrameRate);

  /** @ brief used to toggle active/inactive status of the debug console
   * */
  void handleEvent(const InputEvent &e);

  void update(const DebugConsoleData &data);

  void draw();

  bool isActive() const {
    return _isActive;
  }

private:
  enum DebugTexts {
    FPS_COUNTER, ACTIVE_TIMERS, ACTIVE_WIDGETS, GPU_MEMORY_USAGE,

    DEBUG_TEXTS_COUNT
  };

  Text _debugTexts[DEBUG_TEXTS_COUNT];

  int64_t _maxFrames;

  int32_t _updateCounter;

  bool _isActive;
};

#endif /* GAME_ENGINE_DEBUGCONSOLE_H_ */

