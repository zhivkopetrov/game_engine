#ifndef GAME_ENGINE_DEBUGCONSOLE_H_
#define GAME_ENGINE_DEBUGCONSOLE_H_

//System headers
#include <cstdint>

//Other libraries headers
#include "manager_utils/drawing/Text.h"
#include "utils/ErrorCode.h"

//Own components headers

//Forward declarations
class InputEvent;
struct DebugConsoleConfig;

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

  ErrorCode init(const DebugConsoleConfig& cfg);

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

