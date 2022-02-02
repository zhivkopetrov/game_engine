include(CMakeFindDependencyMacro)

#find_dependency will correctly forward REQUIRED or QUIET args to the consumer
#find_package is only for internal use
find_dependency(manager_utils REQUIRED)

if(NOT TARGET game_engine::game_engine)
  include(${CMAKE_CURRENT_LIST_DIR}/game_engineTargets.cmake)
endif()

# This is for catkin compatibility.
set(game_engine_LIBRARIES game_engine::game_engine)

get_target_property(
    game_engine_INCLUDE_DIRS
    game_engine::game_engine
    INTERFACE_INCLUDE_DIRECTORIES
)

