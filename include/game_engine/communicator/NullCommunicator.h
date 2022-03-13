#ifndef GAME_ENGINE_NULLCOMMUNICATOR_H_
#define GAME_ENGINE_NULLCOMMUNICATOR_H_

//System headers

//Other libraries headers

//Own components headers
#include "game_engine/communicator/Communicator.h"

//Forward declarations

class NullCommunicator final : public Communicator {
public:
  ErrorCode init(const std::any &cfg) override;
  void deinit() override;
  void start() override;
  void shutdown() override;
};

#endif /* GAME_ENGINE_NULLCOMMUNICATOR_H_ */
