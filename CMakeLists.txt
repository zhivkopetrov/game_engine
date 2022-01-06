#author Zhivko Petrov

cmake_minimum_required(VERSION 3.10.2)

project(game_engine)
   
set(_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/game_engine)       
set(_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_library(
    ${PROJECT_NAME} 
    STATIC
        ${_INC_DIR}/engine/config/EngineConfig.h
        ${_INC_DIR}/engine/Engine.h
        ${_INC_DIR}/engine/DebugConsole.h
        ${_INC_DIR}/Application.h
        ${_INC_DIR}/Game.h
        
        ${_SRC_DIR}/engine/Engine.cpp
        ${_SRC_DIR}/engine/DebugConsole.cpp
        ${_SRC_DIR}/Application.cpp
)

set_target_cpp_standard(${PROJECT_NAME} 20)
enable_target_warnings(${PROJECT_NAME})
set_target_visibility(${PROJECT_NAME})

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
        VERSION 1.0.0
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC
        manager_utils
)

#invoke include-what-you-use (if it's enabled) for the current project
if(USE_IWYU)
    enable_target_include_what_you_use(${PROJECT_NAME})
endif(USE_IWYU)

if(BUILD_LIKE_POSITION_INDEPENDANT_CODE)
    enable_target_position_independent_code(${PROJECT_NAME})
endif()  




          



