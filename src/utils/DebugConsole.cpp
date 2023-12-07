//Corresponding header
#include "game_engine/utils/DebugConsole.h"

//System headers
#include <string>

//Other libraries headers
#include "utils/input/InputEvent.h"
#include "utils/LimitValues.h"
#include "utils/time/Time.h"
#include "utils/ErrorCode.h"
#include "utils/log/Log.h"

//Own components headers
#include "game_engine/utils/config/DebugConsoleConfig.h"

#define UPDATE_SKIPS 20

DebugConsole::DebugConsole()
    : _maxFrames(INIT_UINT32_VALUE), _updateCounter(UPDATE_SKIPS),
      _isActive(false) {

}

ErrorCode DebugConsole::init(const DebugConsoleConfig& cfg) {
  constexpr auto INITIAL_TEXT_X = 20;
  constexpr auto INITIAL_TEXT_Y = 10;
  constexpr auto TEXT_OFFSET_Y = 10;
  constexpr auto INITIAL_TEXT_CONTENT = "0";
  _debugTexts[FPS_COUNTER].create(cfg.fontRsrcId, INITIAL_TEXT_CONTENT,
      cfg.textColor, Point(INITIAL_TEXT_X, INITIAL_TEXT_Y));

  for (int32_t i = 1; i < DEBUG_TEXTS_COUNT; ++i) {
    _debugTexts[i].create(cfg.fontRsrcId, INITIAL_TEXT_CONTENT, cfg.textColor,
        Point(INITIAL_TEXT_X, (_debugTexts[i - 1].getY() +
            _debugTexts[i - 1].getCroppedFrameHeight()) + TEXT_OFFSET_Y));
  }

  _maxFrames = cfg.maxFrameRate;

  return ErrorCode::SUCCESS;
}

void DebugConsole::handleEvent(const InputEvent &e) {
  if (TouchEvent::KEYBOARD_RELEASE == e.type) {
    if (Keyboard::KEY_TILDA == e.key) {
      _isActive = !_isActive;
    }
  }
}


void DebugConsole::update(const DebugConsoleData &data) {
  --_updateCounter;
  if (0 < _updateCounter) {
    //update fps text once in a while to get a stable
    //(not constantly changing) image
    return;
  }
  _updateCounter = UPDATE_SKIPS;

  constexpr auto microsecondsInASecond = 1000000;
  auto currFrames = microsecondsInASecond / data.elapsedMicroSeconds;
  if (currFrames > _maxFrames) {
    currFrames = _maxFrames;
  }

  std::string textStr;
  textStr.reserve(50);

  textStr = "FPS: ";
  textStr.append(std::to_string(currFrames));
  _debugTexts[FPS_COUNTER].setText(textStr.c_str());

  textStr = "Active timers: ";
  textStr.append(std::to_string(data.activeTimers));
  _debugTexts[ACTIVE_TIMERS].setText(textStr.c_str());

  textStr = "Active widgets: ";
  textStr.append(std::to_string(data.activeWidgets));
  _debugTexts[ACTIVE_WIDGETS].setText(textStr.c_str());

  constexpr auto oneGigabyte = 1073741824; //1024 * 1024 * 1024
  constexpr auto precision = 3;

  const std::string usedVramStr = std::to_string(
       static_cast<double>(data.gpuMemoryUsage) / oneGigabyte);
  const size_t DOT_POS = usedVramStr.find('.');

  textStr = "GPU usage: ";
  textStr.append(usedVramStr.substr(0, DOT_POS + 1 + precision));
  textStr.append(" GB");
  _debugTexts[GPU_MEMORY_USAGE].setText(textStr.c_str());
}

void DebugConsole::draw() {
  for (auto & text : _debugTexts) {
    text.draw();
  }
}

