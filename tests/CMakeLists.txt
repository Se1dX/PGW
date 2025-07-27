
add_executable(tests
    test_Config.cpp
    test_SessionManager.cpp 
    test_UdpServer.cpp
    test_HttpApi.cpp
)

target_include_directories(tests PRIVATE
    ${CMAKE_SOURCE_DIR}/server/include
)

target_link_libraries(tests PRIVATE
    pgw_common
    gtest_main
)

enable_testing()

include(GoogleTest)
gtest_discover_tests(tests)
