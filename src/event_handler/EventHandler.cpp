//Corresponding header
#include "game_engine/event_handler/EventHandler.h"

//C system headers

//C++ system headers

//Other libraries headers

//Own components headers
#include "utils/ErrorCode.h"
#include "utils/Log.h"

using namespace std::literals;

int32_t EventHandler::init(const HandleInputEventCb &handleInputEventCb) {
  if (SUCCESS != _inputEvent.init()) {
    LOGERR("Error in _inputEvent.init()");
    return FAILURE;
  }

  if (nullptr == handleInputEventCb) {
    LOGERR("Error, nullptr provided for HandleInputEventCb");
    return FAILURE;
  }
  _handleInputEventCb = handleInputEventCb;

  _pollInputEventsThread = std::thread(&EventHandler::pollInputEvents, this);

  return SUCCESS;
}

void EventHandler::deinit() {
  _inputEvent.deinit();
  _pollInputEventsThread.join();
}

void EventHandler::shutdown() {
  _isActive = true;
}

void EventHandler::invokeActionEvent(const ActionEventCb& cb,
                                     ActionEventType eventType) {
  if (ActionEventType::NON_BLOCKING == eventType) {
    _eventQueue.pushWithCopy(cb);
    return;
  }
}

void EventHandler::processStoredEvents(
    const std::chrono::microseconds& allowedTime) {
  ActionEventCb cb;
  const auto [isShutDowned, hasTimedOut] =
      _eventQueue.waitAndPop(cb, allowedTime);
  if (isShutDowned) {
    _isActive = false;
    return;
  }

  //TODO change with while and decrement the time
  if (hasTimedOut) {
    LOGR("eventQueue timedOut");
    return;
  }

  cb();
}

void EventHandler::pollInputEvents() {
  while (_isActive) {

    while (_inputEvent.pollEvent()) {
      const auto f = [this]() {
        _handleInputEventCb(_inputEvent);
      };

      invokeActionEvent(f, ActionEventType::NON_BLOCKING);
    }

    std::this_thread::sleep_for(1ms);
  }
}

