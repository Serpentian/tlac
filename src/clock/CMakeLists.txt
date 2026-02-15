add_library(tlatrace_clock
  client_clock.c
  clock_exception.c
  clock_factory.c
  file_clock.c
  local_clock.c
  memory_clock.c
)

target_include_directories(tlatrace_clock
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# pthreads
find_package(Threads REQUIRED)
target_link_libraries(tlatrace_clock PUBLIC Threads::Threads)

# server_clock executable
add_executable(server_clock server_clock.c)
target_include_directories(server_clock PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(server_clock PRIVATE Threads::Threads)
