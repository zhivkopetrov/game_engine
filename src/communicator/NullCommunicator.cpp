//Corresponding header
#include "game_engine/communicator/NullCommunicator.h"

//System headers

//Other libraries headers
#include "utils/ErrorCode.h"

//Own components headers

ErrorCode NullCommunicator::init([[maybe_unused]]const std::any &cfg) {
  return ErrorCode::SUCCESS;
}

void NullCommunicator::deinit() {

}

void NullCommunicator::start() {

}

void NullCommunicator::shutdown() {

}
