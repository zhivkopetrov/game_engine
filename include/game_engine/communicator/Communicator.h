#ifndef GAME_ENGINE_COMMUNICATOR_H_
#define GAME_ENGINE_COMMUNICATOR_H_

//System headers
#include <cstdint>
#include <any>

//Other libraries headers
#include "utils/class/NonCopyable.h"
#include "utils/class/NonMoveable.h"
#include "utils/ErrorCode.h"

//Own components headers

//Forward declarations

class Communicator : public NonCopyable, public NonMoveable {
public:
  Communicator() = default;
  virtual ~Communicator() noexcept = default;

  virtual ErrorCode init(const std::any& cfg) = 0;
  virtual void deinit() = 0;

  virtual void start() = 0;
  virtual void shutdown() = 0;
};

#endif /* GAME_ENGINE_COMMUNICATOR_H_ */
