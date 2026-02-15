add_executable(two_phase_demo two_phase_demo.c)

target_include_directories(two_phase_demo PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

target_link_libraries(two_phase_demo PRIVATE tlatrace)
