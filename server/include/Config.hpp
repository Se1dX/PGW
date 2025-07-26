#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <cstdint>

namespace pgw {

struct ServerConfig {
    std::string udp_ip;
    uint16_t udp_port;
    unsigned session_timeout_sec;
    std::string cdr_file;
    uint16_t http_port;
    unsigned graceful_shutdown_rate;
    std::string log_file;
    std::string log_level;
    std::vector<std::string> blacklist;
    unsigned max_sessions;
};

// объявление функции
ServerConfig load_server_config(const std::string& file_path);

} // namespace pgw