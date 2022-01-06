#ifndef GAME_ENGINE_GAME_H_
#define GAME_ENGINE_GAME_H_

//C system headers

//C++ system headers
#include <cstdint>
#include <any>

//Other libraries headers

//Own components headers

//Forward declarations
class InputEvent;

class Game {
public:
  virtual ~Game() = default;

  virtual int32_t init(const std::any& cfg) = 0;
  virtual void deinit() = 0;

  virtual void draw() = 0;
  virtual void handleEvent(const InputEvent &e) = 0;
};

#endif /* GAME_ENGINE_GAME_H_ */
