#pragma once
#include "SessionManager.hpp"
#include "CDRLogger.hpp"
#include <httplib.h>
#include <atomic>
#include <memory>
#include <thread>

namespace pgw {

class HttpApi {
public:
    HttpApi(uint16_t port, 
            SessionManager& session_manager,
            CDRLogger& cdr_logger,
            std::atomic<bool>& shutdown_requested,
            unsigned graceful_shutdown_rate);
    
    ~HttpApi();
    
    void run();
    void stop();

private:
    void setup_routes();
    void graceful_shutdown_handler();
    
    uint16_t port_;
    SessionManager& session_manager_;
    CDRLogger& cdr_logger_;
    std::atomic<bool>& shutdown_requested_;
    unsigned graceful_shutdown_rate_;
    
    std::unique_ptr<httplib::Server> server_;
    std::thread server_thread_;
    std::atomic<bool> running_{false};
};

} // namespace pgw