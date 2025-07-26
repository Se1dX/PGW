#include "gtest/gtest.h"
#include "Config.hpp"
#include <filesystem>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

TEST(ConfigTest, LoadValidConfig) {
    // создаем временный конфиг
    const auto temp_path = fs::temp_directory_path() / "test_config.json";
    {
        // явно преобразуем путь в строку
        std::ofstream file(temp_path.string());
        file << R"({
            "udp_ip": "0.0.0.0",
            "udp_port": 9000,
            "session_timeout_sec": 30,
            "cdr_file": "cdr.log",
            "http_port": 8080,
            "graceful_shutdown_rate": 10,
            "log_file": "pgw.log",
            "log_level": "INFO",
            "blacklist": ["001010123456789", "001010000000001"],
            "max_sessions": 10000
        })";
    }
    
    // преобразуем путь в строку
    const auto config = pgw::load_server_config(temp_path.string());
    
    EXPECT_EQ(config.udp_ip, "0.0.0.0");
    EXPECT_EQ(config.udp_port, 9000);
    EXPECT_EQ(config.session_timeout_sec, 30);
    EXPECT_EQ(config.cdr_file, "cdr.log");
    EXPECT_EQ(config.http_port, 8080);
    EXPECT_EQ(config.graceful_shutdown_rate, 10);
    EXPECT_EQ(config.log_file, "pgw.log");
    EXPECT_EQ(config.log_level, "INFO");
    EXPECT_EQ(config.blacklist.size(), 2);
    EXPECT_EQ(config.blacklist[0], "001010123456789");
    EXPECT_EQ(config.max_sessions, 10000);
    
    // удаляем временный файл
    fs::remove(temp_path);
}

TEST(ConfigTest, MissingFile) {
    EXPECT_THROW(
        pgw::load_server_config("missing.json"), 
        std::runtime_error
    );
}

TEST(ConfigTest, InvalidJson) {
    // создаем временный конфиг с ошибкой
    const auto temp_path = fs::temp_directory_path() / "invalid_config.json";
    {
        std::ofstream file(temp_path.string());
        file << R"({ "udp_port": "invalid_value" })"; // неправильный тип
    }
    
    // ожидаем исключение от nlohmann::json
    EXPECT_THROW({
        pgw::load_server_config(temp_path.string());
    }, nlohmann::json::exception); 
    
    fs::remove(temp_path);
}