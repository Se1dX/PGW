#include "HttpApi.hpp"
#include <spdlog/spdlog.h>

namespace pgw {

HttpApi::HttpApi(uint16_t port, 
                 SessionManager& session_manager,
                 CDRLogger& cdr_logger,
                 std::atomic<bool>& shutdown_requested,
                 unsigned graceful_shutdown_rate)
    : port_(port),
      session_manager_(session_manager),
      cdr_logger_(cdr_logger),
      shutdown_requested_(shutdown_requested),
      graceful_shutdown_rate_(graceful_shutdown_rate) {
    
    spdlog::debug("HTTP API инициализирован на порту {}", port);
}

HttpApi::~HttpApi() {
    stop();
}

void HttpApi::run() {
    if (running_) return;
    
    running_ = true;
    server_ = std::make_unique<httplib::Server>();
    
    setup_routes();
    
    server_thread_ = std::thread([this] {
        spdlog::info("HTTP сервер запущен на порту {}", port_);
        server_->listen("0.0.0.0", port_);
    });
}

void HttpApi::stop() {
    if (!running_) return;
    
    running_ = false;
    if (server_) {
        server_->stop();
    }
    
    if (server_thread_.joinable()) {
        server_thread_.join();
    }
    
    spdlog::info("HTTP сервер остановлен");
}

void HttpApi::setup_routes() {
    // проверка статуса абонента
    server_->Get("/check_subscriber", [this](const httplib::Request& req, httplib::Response& res) {
        auto imsi = req.get_param_value("imsi");
        if (imsi.empty()) {
            res.status = 400;
            res.set_content("Error: IMSI parameter is required", "text/plain"); // уточненное сообщение
            spdlog::warn("HTTP /check_subscriber: missing IMSI parameter");
            return;
        }
        
        bool active = session_manager_.is_active(imsi);
        res.set_content(active ? "active" : "not active", "text/plain");
        spdlog::debug("HTTP /check_subscriber: IMSI={} -> {}", imsi, active ? "active" : "not active");
    });
    
    // запрос на остановку сервера
    server_->Get("/stop", [this](const httplib::Request&, httplib::Response& res) {
        res.set_content("Initiating graceful shutdown...", "text/plain");
        spdlog::info("Получен HTTP запрос /stop, инициирую graceful shutdown");
        
        // запускаем graceful shutdown в отдельном потоке
        std::thread shutdown_thread(&HttpApi::graceful_shutdown_handler, this);
        shutdown_thread.detach();
    });
}

void HttpApi::graceful_shutdown_handler() {
    shutdown_requested_ = true;
    spdlog::info("Graceful shutdown начат со скоростью {}/сек", graceful_shutdown_rate_);
    session_manager_.graceful_shutdown(graceful_shutdown_rate_, cdr_logger_);
}

} // namespace pgw