add_library(tlatrace_helper
  configuration_manager.c
  ndjson_serializer.c
)

target_include_directories(tlatrace_helper
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# cJSON dependency
target_link_libraries(tlatrace_helper PUBLIC PkgConfig::CJSON)
