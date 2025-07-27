#include <iostream>
#include "Config.hpp"
#include "Logger.hpp"
#include "UdpServer.hpp"
#include "SessionManager.hpp"
#include "CDRLogger.hpp"
#include <spdlog/spdlog.h>
#include <thread>
#include <csignal>

std::atomic<bool> shutdown_requested{false};

void signal_handler(int signal) {
    if (signal == SIGINT) {
        spdlog::info("Получен сигнал SIGINT, завершаем работу...");
        spdlog::default_logger()->flush(); 
        shutdown_requested = true;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Использование: " << argv[0] << " <путь_к_конфигу>\n";
        return 1;
    }

    try {
        // загрузка конфигурации
        auto config = pgw::load_server_config(argv[1]);
        
        // инициализация логгера
        pgw::Logger::init(config.log_file, config.log_level);
        spdlog::info("Конфигурация загружена");
        
        // создаем менеджер сессий
        std::set<std::string> blacklist(
            config.blacklist.begin(), 
            config.blacklist.end()
        );
        pgw::SessionManager session_manager(
            config.session_timeout_sec,
            blacklist,
            config.max_sessions
        );
        
        // инициализируем CDR логгер
        pgw::CDRLogger cdr_logger(config.cdr_file);
        spdlog::info("CDR логгер инициализирован, файл: {}", config.cdr_file);
        
        // создаем UDP сервер
        pgw::UdpServer udp_server(
            config.udp_ip,
            config.udp_port,
            session_manager,
            cdr_logger
        );
        spdlog::info("Сервер готов к работе на порту {}", config.udp_port);
        
        // обработка сигналов для graceful shutdown
        std::signal(SIGINT, signal_handler);
        
        // запускаем сервер в отдельном потоке
        std::thread server_thread([&udp_server] {
            udp_server.run();
        });
        
        spdlog::info("Сервер запущен. Для остановки нажмите Ctrl+C");
        
        // основной цикл обработки событий
        while (!shutdown_requested) {
            // периодическая очистка устаревших сессий
            session_manager.remove_expired_sessions();
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
        
        // остановка сервера
        spdlog::info("Останавливаем UDP сервер...");
        udp_server.stop();
        server_thread.join();
        
        spdlog::info("Сервер остановлен корректно");
    } catch (const std::exception& e) {
        // Гарантируем запись критической ошибки
        spdlog::critical("Критическая ошибка: {}", e.what());
        spdlog::shutdown();
        return 1;
    }
    
    spdlog::info("Сервер остановлен корректно");
    spdlog::shutdown(); 

    return 0;
}