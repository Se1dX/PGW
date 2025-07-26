#include "Config.hpp"
#include "Logger.hpp"
#include <spdlog/spdlog.h>
#include <iostream>

using namespace pgw;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <config_path>" << std::endl;
        return 1;
    }

    try {
        // загрузка конфигурации
        auto config = load_server_config(argv[1]);
        
        // инициализация логгера
        Logger::init(config.log_file, config.log_level);
        
        // логирование успешной загрузки
        spdlog::info("Server configuration loaded successfully");
        spdlog::debug("UDP port: {}", config.udp_port);
        spdlog::debug("Max sessions: {}", config.max_sessions);
        
        spdlog::info("Blacklisted IMSIs:");
        for (const auto& imsi : config.blacklist) {
            spdlog::info("- {}", imsi);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}