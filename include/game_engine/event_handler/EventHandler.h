#ifndef GAME_ENGINE_EVENTHANDLER_H_
#define GAME_ENGINE_EVENTHANDLER_H_

//C system headers

//C++ system headers
#include <cstdint>
#include <thread>
#include <atomic>

//Other libraries headers
#include "sdl_utils/input/InputEvent.h"
#include "utils/concurrency/ThreadSafeQueue.h"

//Own components headers
#include "game_engine/defines/ActionEventDefines.h"

//Forward declarations

class EventHandler {
public:
  int32_t init(const HandleInputEventCb& handleInputEventCb);
  void deinit();
  void shutdown();

  void invokeActionEvent(const ActionEventCb& cb, ActionEventType eventType);

  void processStoredEvents(const std::chrono::microseconds& allowedTime);

private:
  //called from a dedicated thread
  void pollInputEvents();

  ThreadSafeQueue<ActionEventCb> _eventQueue;

  InputEvent _inputEvent;
  HandleInputEventCb _handleInputEventCb;

  std::thread _pollInputEventsThread;
  std::atomic<bool> _isActive = true;
};

#endif /* GAME_ENGINE_EVENTHANDLER_H_ */
