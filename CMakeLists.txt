cmake_minimum_required(VERSION 3.0)
project(scom)

file(GLOB SOURCES src/*.cpp)
file(GLOB HEADERS include/*.hpp)

include_directories(
  include
  )


add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME}
  pthread
  ncursesw
  ncurses++w
  formw
  menuw
  panelw
  )

