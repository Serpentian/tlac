add_library(tlatrace
  tla_tracer.c
  trace_item.c
)

target_include_directories(tlatrace
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/..
)

# Link internal libs + cjson (via helper)
target_link_libraries(tlatrace
  PUBLIC
    tlatrace_clock
    tlatrace_helper
)
