add_library(tlatrace
  tla_tracer.c
  trace_item.c
  virtual_field.c
)

target_include_directories(tlatrace
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/clock
    ${CMAKE_SOURCE_DIR}/src/helper
)

# Link internal libs + cjson (via helper)
target_link_libraries(tlatrace
  PUBLIC
    tlatrace_clock
    tlatrace_helper
)
