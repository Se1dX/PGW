
add_library(pgw_common STATIC
  src/Config.cpp
  src/Logger.cpp
  src/SessionManager.cpp
)

target_include_directories(pgw_common PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(pgw_common PUBLIC
  nlohmann_json::nlohmann_json
  spdlog::spdlog
)


add_executable(pgw_server src/main.cpp)
target_link_libraries(pgw_server PRIVATE pgw_common)