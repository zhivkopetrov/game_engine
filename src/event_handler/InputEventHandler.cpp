//Corresponding header
#include "game_engine/event_handler/InputEventHandler.h"

//System headers

//Other libraries headers
#include "utils/data_type/EnumClassUtils.h"
#include "utils/log/Log.h"

//Own components headers

using namespace std::literals;

ErrorCode InputEventHandler::init(
    InputEventHandlerPolicy executionPolicy,
    const InvokeActionEventCb &invokeActionEventCb,
    const HandleInputEventCb &handleInputEventCb) {
  if (nullptr == invokeActionEventCb) {
    LOGERR("Error, nullptr provided for InvokeActionEventCb");
    return ErrorCode::FAILURE;
  }
  _invokeActionEventCb = invokeActionEventCb;

  if (nullptr == handleInputEventCb) {
    LOGERR("Error, nullptr provided for HandleInputEventCb");
    return ErrorCode::FAILURE;
  }
  _handleInputEventCb = handleInputEventCb;

  if (ErrorCode::SUCCESS != _inputEventGenerator.init()) {
    LOGERR("Error in _inputEventGenerator.init()");
    return ErrorCode::FAILURE;
  }

  _executionPolicy = executionPolicy;
  if (InputEventHandlerPolicy::RUN_IN_DEDICATED_THREAD == _executionPolicy) {
    LOG("InputEventHandlerPolicy: RUN_IN_DEDICATED_THREAD");
    _pollInputEventsThread = std::thread(
      &InputEventHandler::pollEventsInThread, this);
  } else {
    LOG("InputEventHandlerPolicy: POLL_BLOCKING");
  }

  return ErrorCode::SUCCESS;
}

void InputEventHandler::deinit() {
  if (InputEventHandlerPolicy::RUN_IN_DEDICATED_THREAD == _executionPolicy) {
    //wait for the thread to join before deinit of the InputEventGenerator
    //or the generator may access the deinitilized object
    _pollInputEventsThread.join();
  }

  _inputEventGenerator.deinit();
}

void InputEventHandler::shutdown() {
  _isActive = false;
}

void InputEventHandler::process() {
  if (InputEventHandlerPolicy::POLL_BLOCKING != _executionPolicy) {
    //event are handled in a dedicated thread
    return;
  }

  pollAllPendingEvents();
}

void InputEventHandler::pollAllPendingEvents() {
  while (true) {
    if (!_isActive) {
      break;
    }

    const auto outcome = _inputEventGenerator.pollEvent();
    if (!outcome.hasEvent) {
      break;
    }

    const auto f = [this, outcome]() {
      _handleInputEventCb(outcome.event);
    };

    _invokeActionEventCb(f, ActionEventType::NON_BLOCKING);
  }
}

void InputEventHandler::pollEventsInThread() {
  while (true) {
    if (!_isActive) {
      break;
    }

    pollAllPendingEvents();
    std::this_thread::sleep_for(1ms);
  }
}
