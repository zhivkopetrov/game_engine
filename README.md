# game_engine

**A C++ 2D game engine static library**

It supports:
- 2D hardware accelerate widget rendering with double-buffering
- Multithreaded backend utilities (rendering, audio, inputs, asset load/unload)
- Convinient loading & unloading of application dependencies
- GUI tuning - frame rate, window specifics
- Convinient mechanism of thread-safe communication channel with the rendering thread (UI thread)
- External communicator protocol integrated with the Engine (ROS2 executors, gRPC, RabbitMQ, etc...)
- Convinient Base class Game skeleton, which could be easily extented into any specific Game object
- Simple visual debugging tools

Examples:
```
int32_t main(int32_t argc, char **args) {
  Application app;

  const auto dependencies =
      GuiConfigGenerator::generateDependencies(argc, args);
  if (ErrorCode::SUCCESS != app.loadDependencies(dependencies)) {
    LOGERR("app.loadDependencies() failed");
    return EXIT_FAILURE;
  }

  auto game = std::make_unique<DevBattleGui>();
  app.obtain(std::move(game));

  const auto cfg = GuiConfigGenerator::generateConfig();
  if (ErrorCode::SUCCESS != app.init(cfg)) {
    LOGERR("app.init() failed");
    return EXIT_FAILURE;
  }

  if (ErrorCode::SUCCESS != app.run()) {
    LOGERR("app.run() failed");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
```

**Usage from plain CMake**
- Consume directly with find_package(game_engine) in a CMakeLists.txt
- Link against your target with suitable access modifier
```
target_link_libraries(
    ${PROJECT_NAME} 
    PUBLIC
        game_engine::game_engine
)
```
- Example usage project: https://github.com/zhivkopetrov/dev_battle.git


**Usage as part of ROS(catkin) / ROS2(colcon) meta-build systems**
- Consume directly with find_package(game_engine) in the package CMakeLists.txt
- Link agains your target
- The library automatically exposes and install it's artifacts following ROS/ROS2 structure
- Example usage project: https://github.com/zhivkopetrov/robotics_v1


**Dependencies**
- cmake_helpers - https://github.com/zhivkopetrov/cmake_helpers.git
- The provided library CMakeLists.txt assumes the helpers.cmake from cmake_helpers repo have already been included
- manager_utils - https://github.com/zhivkopetrov/manager_utils


**Platform support**
- Tested on Unix, Windows
- Never tested on Mac