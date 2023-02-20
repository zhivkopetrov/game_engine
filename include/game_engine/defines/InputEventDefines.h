#ifndef GAME_ENGINE_INPUTEVENTDEFINES_H_
#define GAME_ENGINE_INPUTEVENTDEFINES_H_

//System headers
#include <functional>

//Other libraries headers
#include "utils/input/InputEvent.h"

//Own components headers

//Forward declarations

enum class InputEventHandlerPolicy {
  //will block calling thread untill all pending events are consumed
  POLL_BLOCKING,

  //will spawn a new thread and perform event polling there
  RUN_IN_DEDICATED_THREAD
};

using HandleInputEventCb = std::function<void(const InputEvent&)>;

#endif /* GAME_ENGINE_INPUTEVENTDEFINES_H_ */
