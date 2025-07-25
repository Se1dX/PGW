# tests/CMakeLists.txt
add_executable(tests
    test_Config.cpp
)

target_include_directories(tests PRIVATE
    ${CMAKE_SOURCE_DIR}/server/include
)

target_link_libraries(tests PRIVATE
    pgw_common
    gtest_main
)

# Включаем поддержку тестирования
enable_testing()

# Явно добавляем тест
add_test(NAME UnitTests COMMAND tests)

# Опционально: автоматическое обнаружение тестов GoogleTest
include(GoogleTest)
gtest_discover_tests(tests)