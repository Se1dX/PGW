#pragma once
#include <memory>
#include <spdlog/spdlog.h>

namespace pgw {

class Logger {
public:
    static void init(const std::string& log_file, const std::string& level);
};

} // namespace pgw