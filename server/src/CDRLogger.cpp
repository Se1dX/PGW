#include "CDRLogger.hpp"
#include <ctime>
#include <iomanip>
#include <sstream>

namespace pgw {

CDRLogger::CDRLogger(const std::string& filename) {
    // открываем файл в режиме добавления (append)
    file_.open(filename, std::ios::app);
    if (!file_.is_open()) {
        throw std::runtime_error("Не удалось открыть CDR файл: " + filename);
    }
    // записываем заголовок при первом открытии
    if (file_.tellp() = 0) file_ << "timestamp,imsi,action\n";
}

std::string CDRLogger::current_time() const {
    // получаем текущее системное время
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    // форматируем время в читаемый вид
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void CDRLogger::log(const std::string& imsi, const std::string& action) {
    std::lock_guard lock(mutex_);  // защищаем запись от конкурентного доступа
    
    // форматируем строку лога: время, IMSI, действие
    file_ << current_time() << "," << imsi << "," << action << "\n";
    file_.flush();  // сразу пишем на диск

}

} // namespace pgw