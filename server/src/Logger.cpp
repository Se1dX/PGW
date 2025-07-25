#include "Logger.hpp"
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace pgw {

void Logger::init(const std::string& log_file, const std::string& level) {
    // Создаем два сенка: в файл и в консоль
    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(log_file, 1024*1024*5, 3));

    auto logger = std::make_shared<spdlog::logger>("pgw", begin(sinks), end(sinks));
    spdlog::register_logger(logger);
    spdlog::set_default_logger(logger);

    // Устанавливаем уровень логирования
    if (level == "debug") {
        spdlog::set_level(spdlog::level::debug);
    } else if (level == "info") {
        spdlog::set_level(spdlog::level::info);
    } else if (level == "warn") {
        spdlog::set_level(spdlog::level::warn);
    } else if (level == "error") {
        spdlog::set_level(spdlog::level::err);
    } else if (level == "critical") {
        spdlog::set_level(spdlog::level::critical);
    } else {
        spdlog::set_level(spdlog::level::info);
    }

    spdlog::flush_on(spdlog::level::warn);
    spdlog::info("Logger initialized");
}

} // namespace pgw