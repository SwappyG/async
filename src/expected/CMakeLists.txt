cmake_minimum_required(VERSION 3.0)
project(expected)
enable_testing()

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED COMPONENTS unit_test_framework)
find_package(nil_cpp COMPONENTS meta)

include_directories (${Boost_INCLUDE_DIRS})

set(INC
  inc/${PROJECT_NAME}/expected.hpp
  inc/${PROJECT_NAME}/expected_traits.hpp
  inc/${PROJECT_NAME}/unexpected.hpp
  inc/${PROJECT_NAME}/unexpected_traits.hpp
)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE inc/)
target_link_libraries(${PROJECT_NAME} INTERFACE meta)

install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(FILES ${INC} DESTINATION inc)

function(add_boost_test test_name)
  message(STATUS argn: ${ARGN})
  message(STATUS ${test_name})
  add_executable(${test_name} tests/${test_name}.cpp)
  target_link_libraries(${test_name} ${Boost_LIBRARIES} ${PROJECT_NAME} ${ARGN})
  add_test(${test_name}_test COMMAND ${test_name})
endfunction()

add_boost_test(unexpected_test meta)
add_boost_test(expected_test meta)