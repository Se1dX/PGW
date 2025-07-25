#include "Config.hpp"
#include <fstream>
#include <stdexcept>

namespace pgw {

ServerConfig load_server_config(const std::string& file_path) {
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

    return ServerConfig {
        .udp_ip = config["udp_ip"].get<std::string>(),
        .udp_port = config["udp_port"].get<uint16_t>(),
        .session_timeout_sec = config["session_timeout_sec"].get<unsigned>(),
        .cdr_file = config["cdr_file"].get<std::string>(),
        .http_port = config["http_port"].get<uint16_t>(),
        .graceful_shutdown_rate = config["graceful_shutdown_rate"].get<unsigned>(),
        .log_file = config["log_file"].get<std::string>(),
        .log_level = config["log_level"].get<std::string>(),
        .blacklist = config["blacklist"].get<std::vector<std::string>>(),
        .max_sessions = config["max_sessions"].get<unsigned>()
    };
}

} // namespace pgw