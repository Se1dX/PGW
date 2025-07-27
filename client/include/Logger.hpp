#pragma once
#include <memory>
#include <spdlog/spdlog.h>

namespace pgw {

class Logger {
public:
    // для сервера
    static void init(const std::string& log_file, const std::string& level);
    
    // для клиента
    static void init_client(const std::string& log_file, const std::string& level);
};

} // namespace pgw