#pragma once
#include <fstream>
#include <mutex>
#include <iomanip>
#include <chrono>

namespace pgw {

class CDRLogger {
public:
    // создаем логгер с указанием файла для записи CDR
    explicit CDRLogger(const std::string& filename);
    
    // записываем событие в лог: IMSI + действие (created, rejected, expired)
    void log(const std::string& imsi, const std::string& action);
    
private:
    // генерируем текущее время в читаемом формате
    std::string current_time() const;
    
    std::ofstream file_;     // файловый поток для записи
    mutable std::mutex mutex_; // защита для многопоточного доступа
};

} // namespace pgw