#ifndef GAME_ENGINE_EVENTHANDLER_H_
#define GAME_ENGINE_EVENTHANDLER_H_

//System headers
#include <cstdint>
#include <thread>
#include <atomic>

//Other libraries headers
#include "sdl_utils/input/InputEventGenerator.h"
#include "utils/concurrency/ThreadSafeQueue.h"
#include "utils/ErrorCode.h"

//Own components headers
#include "game_engine/defines/ActionEventDefines.h"

//Forward declarations

class ActionEventHandler {
public:
  ErrorCode init(const HandleInputEventCb& handleInputEventCb);
  void deinit();
  void shutdown();

  /** @brief stores events from multiple threads in a thread-safe queue
   *         they will be later on executed from the update thread
   *         in a lock-free thread-safe manner
   *
   *  @param cb        - user defined callback
   *  @param eventType - BLOCKING: invoking thread will block on the result of
   *                               the provided cb
   *                     NON_BLOCKING: cb will be queued and control will return
   *
   *                     Read ActionEventType definition for further details
   * */
  void invokeActionEvent(const ActionEventCb& cb, ActionEventType eventType);

  //executes all stored events in a lock-free thread-safe manner

  /** @brief all stored events in a lock-free thread-safe manner
   *
   *  @param allowedTime - time for which the method will block trying to
   *                       execute as much stored events as possible (if any)
   *
   *                       NOTE: a non-busy wait is used internally (cond_var)
   * */
  void processStoredEvents(const std::chrono::microseconds& allowedTime);

private:
  //returns should stop or not
  bool processSingleEvent(const std::chrono::microseconds& allowedTime);

  //called from a dedicated thread
  void pollInputEvents();

  void invokeBlockingEvent(const ActionEventCb &cb);

  //stores callbacks from multiple threads and executes them sequentially
  //in the update thread in a lock-free thread-safe manner
  ThreadSafeQueue<ActionEventCb> _eventQueue;

  //captures OS input events such as
  //mouse, keyboard, joystick, touch screen, window events, etc...
  InputEventGenerator _inputEventGenerator;

  //redirect to Engine input handle event
  HandleInputEventCb _handleInputEventCb;

  std::thread _pollInputEventsThread;
  std::atomic<bool> _isActive = true;
};

#endif /* GAME_ENGINE_EVENTHANDLER_H_ */
