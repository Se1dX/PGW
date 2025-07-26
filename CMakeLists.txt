cmake_minimum_required(VERSION 3.16)
project(pgw VERSION 0.1 LANGUAGES CXX)
enable_testing()

# ускорение повторных конфигураций - используем кэшированные зависимости
set(FETCHCONTENT_UPDATES_DISCONNECTED ON CACHE BOOL "Skip update checks for dependencies")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# настройка базового каталога для зависимостей
set(FETCHCONTENT_BASE_DIR "${CMAKE_BINARY_DIR}/_deps")

# nlohmann::json
FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json
  GIT_TAG v3.10.0
)
FetchContent_MakeAvailable(nlohmann_json)

# cpp-httplib
FetchContent_Declare(
  httplib
  GIT_REPOSITORY https://github.com/yhirose/cpp-httplib
  GIT_TAG v0.23.0
)
FetchContent_MakeAvailable(httplib)

# spdlog
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog
  GIT_TAG v1.15.0
)
# отключаем ненужные компоненты spdlog
set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_BENCH OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(spdlog)

# googletest
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest
  GIT_TAG release-1.12.1
)
# отключаем ненужные компоненты googletest
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_subdirectory(server)
add_subdirectory(client)
add_subdirectory(tests)

# цели для удобного запуска
add_custom_target(run_server
  COMMAND pgw_server ${CMAKE_SOURCE_DIR}/config/server_config.json
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  DEPENDS pgw_server
  COMMENT "Running PGW server"
)

add_custom_target(run_client
  COMMAND pgw_client ${CMAKE_SOURCE_DIR}/config/client_config.json 001010123456789
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  DEPENDS pgw_client
  COMMENT "Running PGW client"
)

# оптимизация: предварительная загрузка зависимостей
if(NOT FETCHCONTENT_SOURCE_DIR_NLOHMANN_JSON)
  FetchContent_GetProperties(nlohmann_json)
  if(NOT nlohmann_json_POPULATED)
    FetchContent_Populate(nlohmann_json)
  endif()
endif()

if(NOT FETCHCONTENT_SOURCE_DIR_HTTPLIB)
  FetchContent_GetProperties(httplib)
  if(NOT httplib_POPULATED)
    FetchContent_Populate(httplib)
  endif()
endif()