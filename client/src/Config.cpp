#include "Config.hpp"
#include <fstream>
#include <stdexcept>

namespace pgw {

ClientConfig load_client_config(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Config file not found: " + file_path);
    }

    nlohmann::json config;
    try {
        file >> config;
    } catch (const nlohmann::json::parse_error& e) {
        throw std::runtime_error("Config parse error: " + std::string(e.what()));
    }

    return ClientConfig {
        .server_ip = config["server_ip"].get<std::string>(),
        .server_port = config["server_port"].get<uint16_t>(),
        .log_file = config["log_file"].get<std::string>(),
        .log_level = config["log_level"].get<std::string>()
    };
}

} // namespace pgw