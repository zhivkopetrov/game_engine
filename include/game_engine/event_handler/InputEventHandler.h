#ifndef GAME_ENGINE_INPUTEVENTHANDLER_H_
#define GAME_ENGINE_INPUTEVENTHANDLER_H_

//System headers
#include <cstdint>
#include <functional>
#include <thread>
#include <atomic>

//Other libraries headers
#include "sdl_utils/input/InputEventGenerator.h"
#include "utils/ErrorCode.h"

//Own components headers
#include "game_engine/defines/InputEventDefines.h"
#include "game_engine/defines/ActionEventDefines.h"

//Forward declarations

class InputEventHandler {
public:
  ErrorCode init(InputEventHandlerPolicy executionPolicy,
                 const InvokeActionEventCb& invokeActionEventCb,
                 const HandleInputEventCb& handleInputEventCb);
  void deinit();
  void shutdown();
  void process();

private:
  void pollAllPendingEvents();

  void pollEventsInThread();

  //captures OS input events such as
  //mouse, keyboard, joystick, touch screen, window events, etc...
  InputEventGenerator _inputEventGenerator;

  //redirect to Engine input handle event
  HandleInputEventCb _handleInputEventCb;

  //redirect to ActionEventHandler
  InvokeActionEventCb _invokeActionEventCb;

  InputEventHandlerPolicy _executionPolicy = 
    InputEventHandlerPolicy::POLL_BLOCKING;

  std::thread _pollInputEventsThread;
  std::atomic<bool> _isActive = true;
};

#endif //GAME_ENGINE_INPUTEVENTHANDLER_H_