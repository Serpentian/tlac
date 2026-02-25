add_library(tlatrace_clock
  clock_exception.c
  file_clock.c
  memory_clock.c
)

target_include_directories(tlatrace_clock
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# pthreads
find_package(Threads REQUIRED)
target_link_libraries(tlatrace_clock PUBLIC Threads::Threads)
