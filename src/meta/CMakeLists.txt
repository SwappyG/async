cmake_minimum_required(VERSION 3.0)
project(meta)

enable_testing()

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED COMPONENTS unit_test_framework)

include_directories (${Boost_INCLUDE_DIRS})

include_directories (${CMAKE_SOURCE_DIR})

set(INC
  inc/${PROJECT_NAME}/detector.hpp
  inc/${PROJECT_NAME}/devoid.hpp
  inc/${PROJECT_NAME}/enable_if.hpp
  inc/${PROJECT_NAME}/identity.hpp
  inc/${PROJECT_NAME}/none_such.hpp
  inc/${PROJECT_NAME}/void_safe.hpp
)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE inc/)
target_link_libraries(${PROJECT_NAME} INTERFACE ${Boost_LIBRARIES})

install(
  TARGETS ${PROJECT_NAME} DESTINATION lib
)

install(FILES ${INC} DESTINATION inc)