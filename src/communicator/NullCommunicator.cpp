//Corresponding header
#include "game_engine/communicator/NullCommunicator.h"

//C system headers

//C++ system headers

//Other libraries headers
#include "utils/ErrorCode.h"

//Own components headers

int32_t NullCommunicator::init([[maybe_unused]]const std::any &cfg) {
  return SUCCESS;
}

void NullCommunicator::deinit() {

}

void NullCommunicator::start() {

}

void NullCommunicator::shutdown() {

}
