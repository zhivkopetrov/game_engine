//Corresponding header
#include "game_engine/event_handler/ActionEventHandler.h"

//System headers
#include <future>
#include <memory>

//Other libraries headers
#include "utils/data_type/EnumClassUtils.h"
#include "utils/time/Time.h"
#include "utils/ErrorCode.h"
#include "utils/log/Log.h"

//Own components headers

using namespace std::literals;

ErrorCode ActionEventHandler::init(ActionEventHandlerPolicy executionPolicy) {
  _executionPolicy = executionPolicy;
  if (ActionEventHandlerPolicy::BLOCKING == _executionPolicy) {
    LOG("ActionEventHandlerPolicy: BLOCKING");
  } else {
    LOG("ActionEventHandlerPolicy: NON_BLOCKING");
  }

  return ErrorCode::SUCCESS;
}

void ActionEventHandler::deinit() {

}

void ActionEventHandler::shutdown() {
  _isActive = false;
  _eventQueue.shutdown();
}

void ActionEventHandler::invokeActionEvent(const ActionEventCb &cb,
                                           ActionEventType eventType) {
  switch (eventType) {
  case ActionEventType::NON_BLOCKING:
    _eventQueue.pushWithCopy(cb);
    break;
  case ActionEventType::BLOCKING:
    invokeBlockingEvent(cb);
    break;
  default:
    LOGERR("Error, received unsupported ActionEventType: %d",
        getEnumValue(eventType));
    break;
  }
}

void ActionEventHandler::processStoredEvents(
    const std::chrono::microseconds &allowedTime) {
  auto remainingTime = allowedTime;
  Time elapsedTime;
  while (0us < remainingTime) {
    const bool shouldStop = 
      (ActionEventHandlerPolicy::BLOCKING == _executionPolicy) ? 
        processSingleEventBlockingPolicy(remainingTime) :
        processSingleEventNonBlockingPolicy();
    if (shouldStop) {
      break;
    }

    const auto elapsedMicroseconds = elapsedTime.getElapsed().toMicroseconds();
    remainingTime -= std::chrono::microseconds(elapsedMicroseconds);
  }
}

bool ActionEventHandler::processSingleEventBlockingPolicy(
    const std::chrono::microseconds &allowedTime) {
  bool shouldStop = false;
  ActionEventCb cb;

  const auto [isShutdowned, hasTimedOut] = _eventQueue.waitAndPop(cb,
      allowedTime);
  if (isShutdowned) {
    _isActive = false;
    shouldStop = true;
    return shouldStop;
  }

  if (hasTimedOut) {
    return shouldStop;
  }

  cb();
  return shouldStop;
}

bool ActionEventHandler::processSingleEventNonBlockingPolicy() {
  bool shouldStop = false;
  ActionEventCb cb;
  
  const bool success = _eventQueue.tryPop(cb);
  if (!success) {
    shouldStop = true;
    return true;
  }

  cb();
  return shouldStop;
}

void ActionEventHandler::invokeBlockingEvent(const ActionEventCb &cb) {
  //NOTE: keep task alive, because otherwise it could lead the
  //      invoking thread with dangling reference to local object
  auto task = std::make_shared<std::packaged_task<void()>>(cb);

  auto future = task->get_future();
  auto f = [task](){
    (*task)();
  };
  _eventQueue.push(std::move(f));

  while (true) {
    if (!_isActive) {
      break;
    }

    const auto status = future.wait_for(10ms);
    if (std::future_status::ready == status) {
      break;
    }
  }
}

