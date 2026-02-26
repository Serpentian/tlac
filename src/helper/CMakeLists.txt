add_library(tlatrace_helper
  ndjson_serializer.c
)

target_include_directories(tlatrace_helper
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/..
)

# cJSON dependency
target_link_libraries(tlatrace_helper PUBLIC PkgConfig::CJSON)
