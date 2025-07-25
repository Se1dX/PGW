cmake_minimum_required(VERSION 3.16)
project(pgw VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

#nlohmann::json
FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json
  GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

#cpp-httplib
FetchContent_Declare(
  httplib
  GIT_REPOSITORY https://github.com/yhirose/cpp-httplib
  GIT_TAG v0.23.0
)
FetchContent_MakeAvailable(httplib)

#spdlog
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog
  GIT_TAG v1.15.0
)
FetchContent_MakeAvailable(spdlog)

#gtest
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest
  GIT_TAG v1.16.0
)
FetchContent_MakeAvailable(googletest)

add_subdirectory(server)
add_subdirectory(client)
add_subdirectory(tests)