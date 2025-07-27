#pragma once
#include <nlohmann/json.hpp>
#include <string>

namespace pgw {

struct ClientConfig {
    std::string server_ip;
    uint16_t server_port;
    std::string log_file;
    std::string log_level;
};

ClientConfig load_client_config(const std::string& file_path);

} // namespace pgw