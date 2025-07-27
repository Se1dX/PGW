#include "Logger.hpp"
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <iostream>
#include <mutex>

namespace pgw {

static void init_logger(const std::string& name, 
                       const std::string& log_file, 
                       const std::string& level) {
    static std::once_flag logger_init_flag;
    std::call_once(logger_init_flag, [&]() {
        try {
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                log_file, 1024*1024*5, 3
            );
            
            auto logger = std::make_shared<spdlog::logger>(name, 
                spdlog::sinks_init_list{console_sink, file_sink}
            );
            
            if (level == "debug") logger->set_level(spdlog::level::debug);
            else if (level == "info") logger->set_level(spdlog::level::info);
            else if (level == "warn") logger->set_level(spdlog::level::warn);
            else if (level == "error") logger->set_level(spdlog::level::err);
            else if (level == "critical") logger->set_level(spdlog::level::critical);
            else logger->set_level(spdlog::level::info);
            
            logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%^%l%$] %v");
            logger->flush_on(spdlog::level::trace);
            
            spdlog::register_logger(logger);
            spdlog::set_default_logger(logger);
            
            logger->info("Логгер '{}' инициализирован. Файл: {}", name, log_file);
            
        } catch (const std::exception& ex) {
            std::cerr << "Ошибка инициализации логгера: " << ex.what() << std::endl;
        }
    });
}

void Logger::init(const std::string& log_file, const std::string& level) {
    init_logger("pgw_server", log_file, level);
}

void Logger::init_client(const std::string& log_file, const std::string& level) {
    init_logger("pgw_client", log_file, level);
}

} // namespace pgw