#author Zhivko Petrov

cmake_minimum_required(VERSION 3.10.2)

project(
    game_engine
    LANGUAGES CXX
    VERSION 1.0.0
)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_helpers/find_modules)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake_helpers/helpers.cmake)

find_package(manager_utils REQUIRED)

set(_INC_FOLDER_NAME include)
set(_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${_INC_FOLDER_NAME}/${PROJECT_NAME})       
set(_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_library(
    ${PROJECT_NAME} 
    STATIC
        ${_INC_DIR}/engine/config/EngineConfig.h
        ${_INC_DIR}/engine/Engine.h
        ${_INC_DIR}/engine/DebugConsole.h
        ${_INC_DIR}/Application.h
        ${_INC_DIR}/Game.h
        
        ${_SRC_DIR}/engine/config/EngineConfig.cpp
        ${_SRC_DIR}/engine/Engine.cpp
        ${_SRC_DIR}/engine/DebugConsole.cpp
        ${_SRC_DIR}/Application.cpp
)

add_library(game_engine::game_engine ALIAS game_engine)

target_include_directories(
    ${PROJECT_NAME} 
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${_INC_FOLDER_NAME}>
        $<INSTALL_INTERFACE:${_INC_FOLDER_NAME}>
)

target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC
        manager_utils::manager_utils
)

set_target_cpp_standard(${PROJECT_NAME} 20)
enable_target_warnings(${PROJECT_NAME})
set_target_visibility(${PROJECT_NAME})
install_and_export_target(${PROJECT_NAME} ${_INC_FOLDER_NAME})

if(NOT DISABLE_ROS_TOOLING)
    enable_ros_tooling_for_target(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/package.xml)
endif()

#invoke include-what-you-use (if it's enabled) for the current project
if(USE_IWYU)
    enable_target_include_what_you_use(${PROJECT_NAME})
endif(USE_IWYU)

if(BUILD_LIKE_POSITION_INDEPENDANT_CODE)
    enable_target_position_independent_code(${PROJECT_NAME})
endif()  




          



