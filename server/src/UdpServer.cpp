#include "UdpServer.hpp"
#include <spdlog/spdlog.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <stdexcept>

namespace pgw {

UdpServer::UdpServer(const std::string& ip, uint16_t port, 
                     SessionManager& session_manager,
                     CDRLogger& cdr_logger)
    : session_manager_(session_manager),
      cdr_logger_(cdr_logger),
      running_(false) {
    
    sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd_ < 0) {
        throw std::runtime_error("ошибка создания сокета: " + std::string(strerror(errno)));
    }
    
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    
    if (inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr) <= 0) {
        close(sockfd_);
        throw std::runtime_error("неверный ip адрес: " + ip);
    }
    
    if (bind(sockfd_, (struct sockaddr*)&addr_, sizeof(addr_)) < 0) {
        close(sockfd_);
        throw std::runtime_error("ошибка привязки сокета: " + std::string(strerror(errno)));
    }
    
    // получаем реальный порт после привязки
    sockaddr_in actual_addr;
    socklen_t len = sizeof(actual_addr);
    getsockname(sockfd_, (struct sockaddr*)&actual_addr, &len);
    addr_.sin_port = actual_addr.sin_port;  // сохраняем реальный порт
    
    spdlog::info("UDP сервер создан на {}:{}", 
                 inet_ntoa(addr_.sin_addr), ntohs(addr_.sin_port));
}

uint16_t UdpServer::port() const {
    return ntohs(addr_.sin_port);
}

void UdpServer::handle_request(const std::string& imsi, const sockaddr_in& client_addr) {
    // обрабатываем запрос через менеджер сессий
    auto result = session_manager_.try_create_session(imsi);
    
    std::string response;
    std::string action;
    
    // формируем ответ клиенту и действие для лога
    switch(result) {
        case SessionManager::CreateResult::CREATED:
            response = "created";
            action = "created";
            break;
        case SessionManager::CreateResult::ALREADY_EXISTS:
            response = "created";
            action = "exists";
            break;
        default:
            response = "rejected";
            action = "rejected";
    }
    
    // логируем действие в CDR
    cdr_logger_.log(imsi, action);
    
    // отправляем ответ клиенту
    ssize_t sent = sendto(sockfd_, response.data(), response.size(), 0,
                         (struct sockaddr*)&client_addr, sizeof(client_addr));
    
    if (sent < 0) {
        spdlog::error("Ошибка отправки для IMSI {}: {}", imsi, strerror(errno));
    } else {
        spdlog::debug("Отправлено {} байт для IMSI {}: {}", sent, imsi, response);
    }
}

void UdpServer::run() {
    running_ = true;
    spdlog::info("Запуск UDP сервера...");
    
    char buffer[16]; // IMSI (15 цифр + null)
    sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    
    while (running_) {
        // ждем входящего запроса
        ssize_t n = recvfrom(sockfd_, buffer, sizeof(buffer), 0,
                            (struct sockaddr*)&client_addr, &len);
        
        if (n <= 0) {
            if (running_) {
                spdlog::warn("Ошибка при чтении из сокета: {}", strerror(errno));
            }
            continue;
        }
        
        // преобразуем данные в строку (IMSI)
        buffer[n] = '\0';
        std::string imsi(buffer);
        
        // преобразуем IP клиента в читаемый вид
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        
        spdlog::info("Получен запрос от {}: IMSI={}", client_ip, imsi);
        
        // обрабатываем запрос
        handle_request(imsi, client_addr);
    }
    
    close(sockfd_);
    spdlog::info("UDP сервер остановлен");
}

void UdpServer::stop() {
    running_ = false;
    // создаем фиктивный запрос для выхода из блокировки recvfrom
    sockaddr_in dummy_addr{};
    dummy_addr.sin_family = AF_INET;
    dummy_addr.sin_port = htons(port());
    inet_pton(AF_INET, "127.0.0.1", &dummy_addr.sin_addr);
    
    sendto(sockfd_, "", 1, 0, (struct sockaddr*)&dummy_addr, sizeof(dummy_addr));
}

} // namespace pgw