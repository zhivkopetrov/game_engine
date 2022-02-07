//Corresponding header
#include "game_engine/event_handler/ActionEventHandler.h"

//C system headers

//C++ system headers

//Other libraries headers
#include "utils/time/Time.h"
#include "utils/ErrorCode.h"
#include "utils/Log.h"

//Own components headers

using namespace std::literals;

int32_t ActionEventHandler::init(const HandleInputEventCb &handleInputEventCb) {
  if (SUCCESS != _inputEventGenerator.init()) {
    LOGERR("Error in _inputEventGenerator.init()");
    return FAILURE;
  }

  if (nullptr == handleInputEventCb) {
    LOGERR("Error, nullptr provided for HandleInputEventCb");
    return FAILURE;
  }
  _handleInputEventCb = handleInputEventCb;

  _pollInputEventsThread =
      std::thread(&ActionEventHandler::pollInputEvents, this);

  return SUCCESS;
}

void ActionEventHandler::deinit() {
  //wait for the thread to join before deinit of the InputEventGenerator
  //or the generator may access the deinitilized object
  _pollInputEventsThread.join();

  _inputEventGenerator.deinit();
}

void ActionEventHandler::shutdown() {
  _isActive = false;
  _eventQueue.shutdown();
}

void ActionEventHandler::invokeActionEvent(const ActionEventCb& cb,
                                           ActionEventType eventType) {
  if (ActionEventType::NON_BLOCKING == eventType) {
    _eventQueue.pushWithCopy(cb);
    return;
  }
}

void ActionEventHandler::processStoredEvents(
    const std::chrono::microseconds& allowedTime) {
  auto remainingTime = allowedTime;
  Time elapsedTime;
  while (0us < remainingTime) {
    const bool shouldStop = processSingleEvent(remainingTime);
    if (shouldStop) {
      break;
    }

    const auto elapsedMicroseconds = elapsedTime.getElapsed().toMicroseconds();
    remainingTime -= std::chrono::microseconds(elapsedMicroseconds);
  }
}

bool ActionEventHandler::processSingleEvent(
    const std::chrono::microseconds& allowedTime) {
  bool shouldStop = true;

  ActionEventCb cb;
  const auto [isShutdowned, hasTimedOut] =
      _eventQueue.waitAndPop(cb, allowedTime);
  if (isShutdowned) {
    _isActive = false;
    return shouldStop;
  }

  if (hasTimedOut) {
    return shouldStop;
  }

  cb();
  shouldStop = false;
  return shouldStop;
}

void ActionEventHandler::pollInputEvents() {
  while (true) { //main loop
    while (true) { //poll events loop
      if (!_isActive) {
        return;
      }

      const auto outcome = _inputEventGenerator.pollEvent();
      if (!outcome.hasEvent) {
        break;
      }

      const auto f = [this, outcome]() {
        _handleInputEventCb(outcome.event);
      };

      invokeActionEvent(f, ActionEventType::NON_BLOCKING);
    } //end pool events loop

    std::this_thread::sleep_for(1ms);
  } //end main loop
}

