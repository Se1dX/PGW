#include "Logger.hpp"
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <iostream>
#include <mutex>

namespace pgw {

static std::shared_ptr<spdlog::logger> global_logger;

void Logger::init(const std::string& log_file, const std::string& level) {
    static std::once_flag logger_init_flag;
    std::call_once(logger_init_flag, [&]() {
        try {
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                log_file, 1024*1024*5, 3
            );
            
            console_sink->set_level(spdlog::level::trace);
            file_sink->set_level(spdlog::level::trace);
            
            global_logger = std::make_shared<spdlog::logger>("pgw", 
                spdlog::sinks_init_list{console_sink, file_sink}
            );
            
            if (level == "debug") global_logger->set_level(spdlog::level::debug);
            else if (level == "info") global_logger->set_level(spdlog::level::info);
            else if (level == "warn") global_logger->set_level(spdlog::level::warn);
            else if (level == "error") global_logger->set_level(spdlog::level::err);
            else if (level == "critical") global_logger->set_level(spdlog::level::critical);
            else global_logger->set_level(spdlog::level::info);
            
            global_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%^%l%$] %v");
            
            global_logger->flush_on(spdlog::level::trace);
            
            spdlog::register_logger(global_logger);
            spdlog::set_default_logger(global_logger);
            
            global_logger->flush();
            
            global_logger->info("Логгер инициализирован. Файл: {}", log_file);
            global_logger->debug("Уровень логирования: {}", level);
            
        } catch (const spdlog::spdlog_ex& ex) {
            std::cerr << "SPDLOG error: " << ex.what() << std::endl;
        } catch (const std::exception& ex) {
            std::cerr << "Logger init error: " << ex.what() << std::endl;
        }
    });
}

} // namespace pgw