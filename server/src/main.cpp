#include <iostream>
#include "Config.hpp"
#include "Logger.hpp"
#include "UdpServer.hpp"
#include "SessionManager.hpp"
#include "CDRLogger.hpp"
#include "HttpApi.hpp"
#include <spdlog/spdlog.h>
#include <thread>
#include <csignal>
#include <memory>

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

    // объявляем умные указатели для основных компонентов
    std::unique_ptr<pgw::SessionManager> session_manager;
    std::unique_ptr<pgw::CDRLogger> cdr_logger;
    std::unique_ptr<pgw::UdpServer> udp_server;
    std::unique_ptr<pgw::HttpApi> http_api;

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
        session_manager = std::make_unique<pgw::SessionManager>(
            config.session_timeout_sec,
            blacklist,
            config.max_sessions
        );
        
        // инициализируем CDR логгер
        cdr_logger = std::make_unique<pgw::CDRLogger>(config.cdr_file);
        spdlog::info("CDR логгер инициализирован, файл: {}", config.cdr_file);
        
        // создаем UDP сервер
        udp_server = std::make_unique<pgw::UdpServer>(
            config.udp_ip,
            config.udp_port,
            *session_manager,
            *cdr_logger
        );
        spdlog::info("Сервер готов к работе на порту {}", config.udp_port);
        
        // создаем и запускаем HTTP API
        http_api = std::make_unique<pgw::HttpApi>(
            config.http_port,
            *session_manager,
            *cdr_logger,
            shutdown_requested,
            config.graceful_shutdown_rate
        );
        http_api->run();
        spdlog::info("HTTP API доступен на порту {}", config.http_port);
        
        // обработка сигналов для graceful shutdown
        std::signal(SIGINT, signal_handler);
        
        // запускаем UDP сервер в отдельном потоке
        std::thread server_thread([&udp_server] {
            udp_server->run();
        });
        
        spdlog::info("Сервер запущен. Для остановки нажмите Ctrl+C");
        
        // основной цикл обработки событий
        while (!shutdown_requested) {
            // периодическая очистка устаревших сессий
            session_manager->remove_expired_sessions();
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
        
        // остановка серверов
        spdlog::info("Останавливаем UDP сервер...");
        udp_server->stop();
        server_thread.join();

        spdlog::info("Останавливаем HTTP сервер...");
        http_api->stop();
        
        spdlog::info("Сервер остановлен корректно");
    } catch (const std::exception& e) {
        spdlog::critical("Критическая ошибка: {}", e.what());
        return 1;
    }
    
    spdlog::shutdown(); 
    return 0;
}