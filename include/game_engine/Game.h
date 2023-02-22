#ifndef GAME_ENGINE_GAME_H_
#define GAME_ENGINE_GAME_H_

//System headers
#include <cstdint>
#include <any>
#include <functional>

//Other libraries headers
#include "sdl_utils/drawing/defines/DrawConstants.h"
#include "utils/class/NonCopyable.h"
#include "utils/class/NonMoveable.h"
#include "utils/ErrorCode.h"

//Own components headers
#include "game_engine/defines/ActionEventDefines.h"
#include "sdl_utils/drawing/defines/DrawUtilityDefines.h"


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

  //NOTE: optionally override this method in your Game extended class
  //This method will be called after the all subsystems initization have passed
  //and right before the Engine enters its mainLoop.
  //this includes graphical asset loading calls made during initialization.
  //This could be useful where you don't want to rely on the initialization 
  //order of you entities during the ::init() phase. 
  //This method will be called when all entities are created and initialized.
  //One such example could be where a StateMachine is utilized and you want to 
  //'start' your game by entering the first state.
  virtual void start() {}

  //NOTE: optionally override this method in your Game extended class
  //This method will be called once every engine frame
  //It could be useful if your game utilises processing/polling behaviour
  virtual void process() {}

  void setInvokeActionEventCb(const InvokeActionEventCb& cb) {
    _invokeActionEventCb = cb;
  }

  void setSystemShutdownCb(const SystemShutdownCb& cb) {
    _systemShutdownCb = cb;
  }

  void setTakeScreenshotCb(const TakeScreenshotCb& cb) {
    _takeScreenshotCb = cb;
  }

protected:
  InvokeActionEventCb _invokeActionEventCb;
  SystemShutdownCb _systemShutdownCb;
  TakeScreenshotCb _takeScreenshotCb;
};

#endif /* GAME_ENGINE_GAME_H_ */
