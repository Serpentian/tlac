add_executable(test_client_clock test_client_clock.c)

target_include_directories(test_client_clock
  PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/clock
)

target_link_libraries(test_client_clock
  PRIVATE
    tlatrace_clock
)
