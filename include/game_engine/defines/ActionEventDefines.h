#ifndef GAME_ENGINE_ACTIONEVENTDEFINES_H_
#define GAME_ENGINE_ACTIONEVENTDEFINES_H_

//System headers
#include <cstdint>
#include <functional>

//Other libraries headers

//Own components headers

//Forward declarations
class InputEvent;

enum class ActionEventType {
  //need to copy their data in the ActionEventCb lambda capture list,
  //because the invoking thread will return the call almost immediately
  BLOCKING,

  //may use reference in the ActionEventCb lambda capture list,
  //because the invoking thread will wait on the result
  NON_BLOCKING
};

enum class ActionEventHandlerPolicy {
  //will block calling thread (non-busy wait) for remaining frame time,
  //processing all events (currently stored and newly incoming)
  BLOCKING,

  //will return control after all pending events are consumed
  //or given allowed execution time is reached
  NON_BLOCKING
};

using ActionEventCb = std::function<void()>;
using InvokeActionEventCb =
    std::function<void(const ActionEventCb& cb, ActionEventType eventType)>;
using SystemShutdownCb = std::function<void()>;

#endif /* GAME_ENGINE_ACTIONEVENTDEFINES_H_ */
