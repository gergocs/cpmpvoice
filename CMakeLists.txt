cmake_minimum_required(VERSION 3.21)
project(cpmpvoice)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(${PROJECT_SOURCE_DIR}/portaudio)

add_executable(cpmpvoice main.cpp AudioHandler.cpp AudioHandler.h)

target_link_libraries(cpmpvoice PortAudio)

install(TARGETS ${PROJECT_NAME} DESTINATION .)
