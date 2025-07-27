#include "gtest/gtest.h"
#include "UdpServer.hpp"
#include "SessionManager.hpp"
#include "CDRLogger.hpp"
#include <thread>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <chrono>
#include <stdexcept>
#include <condition_variable>
#include <cstring>

using namespace std::chrono_literals;

class UdpServerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // создаем временный файл безопасно
        char tmp_file[] = "/tmp/pgw_test_XXXXXX";
        int fd = mkstemp(tmp_file);
        if (fd == -1) {
            throw std::runtime_error("не удалось создать временный файл");
        }
        close(fd);
        cdr_file = tmp_file;
        
        // инициализируем зависимости
        session_manager = std::make_unique<pgw::SessionManager>(30, blacklist, 100);
        cdr_logger = std::make_unique<pgw::CDRLogger>(cdr_file);
        
        // запускаем сервер и получаем реальный порт
        server = std::make_unique<pgw::UdpServer>("127.0.0.1", 0, *session_manager, *cdr_logger);
        actual_port = server->port();
        
        // запускаем сервер в потоке
        server_thread = std::thread([this] {
            server->run();
        });
        
        // ждем запуска сервера
        std::this_thread::sleep_for(100ms);
    }
    
    void TearDown() override {
        server->stop();
        if (server_thread.joinable()) {
            server_thread.join();
        }
        std::remove(cdr_file.c_str());
    }
    
    int create_client_socket() {
        int sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock < 0) {
            throw std::runtime_error("ошибка создания сокета: " + std::string(strerror(errno)));
        }
        return sock;
    }
    
    std::string cdr_file;
    std::set<std::string> blacklist = {"123456"};
    std::unique_ptr<pgw::SessionManager> session_manager;
    std::unique_ptr<pgw::CDRLogger> cdr_logger;
    std::unique_ptr<pgw::UdpServer> server;
    std::thread server_thread;
    uint16_t actual_port; // реальный порт сервера
};

TEST_F(UdpServerTest, BasicRequest) {
    // создаем клиентский сокет
    int client_sock = create_client_socket();
    
    // настраиваем адрес сервера
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(actual_port);
    
    // преобразуем ip адрес
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        close(client_sock);
        FAIL() << "неверный адрес сервера";
    }
    
    // отправляем imsi
    std::string imsi = "111222333444555";
    ssize_t sent = sendto(client_sock, imsi.c_str(), imsi.size(), 0, 
                         (sockaddr*)&server_addr, sizeof(server_addr));
    
    // проверяем успешность отправки
    if (sent < 0) {
        close(client_sock);
        FAIL() << "ошибка sendto: " << strerror(errno);
    }
    
    // настраиваем таймаут приема
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    setsockopt(client_sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    
    // принимаем ответ
    char buffer[128] = {0};
    sockaddr_in from_addr;
    socklen_t from_len = sizeof(from_addr);
    ssize_t received = recvfrom(client_sock, buffer, sizeof(buffer), 0,
                              (sockaddr*)&from_addr, &from_len);
    
    // закрываем сокет
    close(client_sock);
    
    // проверяем результаты
    ASSERT_GT(received, 0) << "ответ не получен: " << strerror(errno);
    std::string response(buffer, received);
    EXPECT_EQ(response, "created");
    EXPECT_TRUE(session_manager->is_active(imsi));
}

TEST_F(UdpServerTest, BlacklistedRequest) {
    int client_sock = create_client_socket();
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(actual_port);
    
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        close(client_sock);
        FAIL() << "неверный адрес сервера";
    }
    
    // отправляем imsi из черного списка
    std::string imsi = "123456";
    ssize_t sent = sendto(client_sock, imsi.c_str(), imsi.size(), 0, 
                         (sockaddr*)&server_addr, sizeof(server_addr));
    
    if (sent < 0) {
        close(client_sock);
        FAIL() << "ошибка sendto: " << strerror(errno);
    }
    
    // настраиваем таймаут
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    setsockopt(client_sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    
    // прием ответа
    char buffer[128] = {0};
    sockaddr_in from_addr;
    socklen_t from_len = sizeof(from_addr);
    ssize_t received = recvfrom(client_sock, buffer, sizeof(buffer), 0,
                              (sockaddr*)&from_addr, &from_len);
    
    close(client_sock);
    
    ASSERT_GT(received, 0) << "ответ не получен: " << strerror(errno);
    std::string response(buffer, received);
    EXPECT_EQ(response, "rejected");
    EXPECT_FALSE(session_manager->is_active(imsi));
}