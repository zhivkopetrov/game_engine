#ifndef GAME_ENGINE_GAME_H_
#define GAME_ENGINE_GAME_H_

//C system headers

//C++ system headers
#include <cstdint>
#include <any>
#include <functional>

//Other libraries headers

//Own components headers
#include "game_engine/defines/ActionEventDefines.h"

//Forward declarations
class InputEvent;

class Game {
public:
  virtual ~Game() = default;

  virtual int32_t init(const std::any& cfg) = 0;
  virtual void deinit() = 0;

  virtual void draw() const = 0;
  virtual void handleEvent(const InputEvent &e) = 0;

  //called once every frame
  virtual void process() = 0;

  void setInvokeActionEventCb(const InvokeActionEventCb& cb) {
    _invokeActionEventCb = cb;
  }

protected:
  InvokeActionEventCb _invokeActionEventCb;
};

#endif /* GAME_ENGINE_GAME_H_ */
