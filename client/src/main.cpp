#include <iostream>
#include "Config.hpp"
#include "UdpClient.hpp"
#include "Logger.hpp"
#include <spdlog/spdlog.h>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Использование: " << argv[0] << " <конфиг> <IMSI>\n";
        return 1;
    }
    
    try {
        // загрузка конфигурации
        auto config = pgw::load_client_config(argv[1]);
        
        // инициализация логгера КЛИЕНТА
        pgw::Logger::init_client(config.log_file, config.log_level);
        spdlog::info("Запуск клиента с IMSI={}", argv[2]);
        
        // создание и отправка запроса
        pgw::UdpClient client(config);
        std::string response = client.send_request(argv[2]);
        
        // вывод результата
        if (!response.empty()) {
            std::cout << "Ответ сервера: " << response << "\n";
        } else {
            std::cerr << "Ошибка при получении ответа\n";
        }
        
        spdlog::shutdown();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << "\n";
        return 1;
    }
}