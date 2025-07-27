#pragma once
#include <netinet/in.h>
#include <atomic>
#include <string>
#include "SessionManager.hpp"
#include "CDRLogger.hpp"

namespace pgw {

class UdpServer {
public:
    UdpServer(const std::string& ip, uint16_t port, 
              SessionManager& session_manager,
              CDRLogger& cdr_logger);
    
    void run();
    void stop();
    uint16_t port() const;  // метод для получения порта
    
private:
    void handle_request(const std::string& imsi, const sockaddr_in& client_addr);
    
    int sockfd_;
    sockaddr_in addr_;
    std::atomic<bool> running_{false};
    SessionManager& session_manager_;
    CDRLogger& cdr_logger_;
};

} // namespace pgw