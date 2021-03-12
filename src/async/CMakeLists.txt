cmake_minimum_required(VERSION 3.0)
project(async)

enable_testing()

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-pthread")

find_package(Boost REQUIRED COMPONENTS unit_test_framework)
find_package(Threads REQUIRED)

include_directories (${Boost_INCLUDE_DIRS})

set(INC
  inc/${PROJECT_NAME}/atomic.hpp
  inc/${PROJECT_NAME}/atomic_base.hpp
  inc/${PROJECT_NAME}/atomic_proxies.hpp
  inc/${PROJECT_NAME}/atomic_rw.hpp
  inc/${PROJECT_NAME}/atomic_rw_base.hpp
  inc/${PROJECT_NAME}/optional.hpp
  inc/${PROJECT_NAME}/optional_base.hpp
)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE inc/)
target_link_libraries(${PROJECT_NAME} INTERFACE meta Threads::Threads)

install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(FILES ${INC} DESTINATION inc)

function(add_boost_test test_name)
  add_executable(${test_name} tests/${test_name}.cpp)
  target_link_libraries(${test_name} ${Boost_LIBRARIES} ${PROJECT_NAME} ${ARGN})
  add_test(${test_name}_test COMMAND ${test_name})
endfunction()

add_boost_test(atomic_test)
add_boost_test(atomic_rw_test)
add_boost_test(container_test)
add_boost_test(container_traits_test)
add_boost_test(optional_test)
