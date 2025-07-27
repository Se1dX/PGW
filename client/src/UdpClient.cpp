#include "UdpClient.hpp"
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <stdexcept>

namespace pgw {

UdpClient::UdpClient(const ClientConfig& config) 
    : config_(config) {
    
    sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd_ < 0) {
        throw std::runtime_error("Ошибка создания сокета: " + std::string(strerror(errno)));
    }
    
    server_addr_.sin_family = AF_INET;
    server_addr_.sin_port = htons(config.server_port);
    
    if (inet_pton(AF_INET, config.server_ip.c_str(), &server_addr_.sin_addr) <= 0) {
        close(sockfd_);
        throw std::runtime_error("Неверный IP сервера: " + config.server_ip);
    }
    
    spdlog::info("Клиент подключен к серверу {}:{}", 
                 config.server_ip, config.server_port);
}

std::string UdpClient::send_request(const std::string& imsi) {
    // отправка IMSI
    ssize_t sent = sendto(sockfd_, imsi.c_str(), imsi.size(), 0,
                         (struct sockaddr*)&server_addr_, sizeof(server_addr_));
    
    if (sent < 0) {
        spdlog::error("Ошибка отправки: {}", strerror(errno));
        return "";
    }
    
    spdlog::info("Отправлено {} байт: IMSI={}", sent, imsi);
    
    // получение ответа
    char buffer[128] = {0};
    sockaddr_in from_addr;
    socklen_t len = sizeof(from_addr);
    
    ssize_t received = recvfrom(sockfd_, buffer, sizeof(buffer), 0,
                              (struct sockaddr*)&from_addr, &len);
    
    if (received < 0) {
        spdlog::error("Ошибка получения: {}", strerror(errno));
        return "";
    }
    
    std::string response(buffer, received);
    spdlog::info("Получено {} байт: {}", received, response);
    return response;
}

} // namespace pgw