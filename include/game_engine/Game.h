#ifndef GAME_ENGINE_GAME_H_
#define GAME_ENGINE_GAME_H_

//System headers
#include <cstdint>
#include <any>
#include <functional>

//Other libraries headers
#include "utils/class/NonCopyable.h"
#include "utils/class/NonMoveable.h"
#include "utils/ErrorCode.h"

//Own components headers
#include "game_engine/defines/ActionEventDefines.h"

//Forward declarations
class InputEvent;

class Game : public NonCopyable, public NonMoveable {
public:
  Game() = default;
  virtual ~Game() noexcept = default;

  virtual ErrorCode init(const std::any& cfg) = 0;
  virtual void deinit() = 0;

  virtual void draw() const = 0;
  virtual void handleEvent(const InputEvent &e) = 0;

  //called once every frame
  virtual void process() = 0;

  void setInvokeActionEventCb(const InvokeActionEventCb& cb) {
    _invokeActionEventCb = cb;
  }

  void setSystemShutdownCb(const SystemShutdownCb& cb) {
    _systemShutdownCb = cb;
  }

protected:
  InvokeActionEventCb _invokeActionEventCb;
  SystemShutdownCb _systemShutdownCb;
};

#endif /* GAME_ENGINE_GAME_H_ */
