add_executable(pgw_client
  src/main.cpp
  src/Config.cpp
  src/UdpClient.cpp
  src/Logger.cpp
)

target_include_directories(pgw_client PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(pgw_client PRIVATE
  nlohmann_json::nlohmann_json
  spdlog::spdlog
)