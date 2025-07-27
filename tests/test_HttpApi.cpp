#include "gtest/gtest.h"
#include "HttpApi.hpp"
#include "SessionManager.hpp"
#include "CDRLogger.hpp"
#include <httplib.h>
#include <atomic>
#include <thread>
#include <future>

using namespace std::chrono_literals;

class HttpApiTest : public ::testing::Test {
protected:
    void SetUp() override {
        // инициализация зависимостей
        session_manager = std::make_unique<pgw::SessionManager>(30, blacklist, 100);
        cdr_logger = std::make_unique<pgw::CDRLogger>("/tmp/test_cdr.log");
        
        // создаем HTTP API
        http_api = std::make_unique<pgw::HttpApi>(
            8080, 
            *session_manager, 
            *cdr_logger,
            shutdown_requested,
            5
        );
        
        // запускаем сервер в фоне
        server_future = std::async(std::launch::async, [this] {
            http_api->run();
        });
        
        // ждем запуска сервера
        std::this_thread::sleep_for(500ms);
    }
    
    void TearDown() override {
        http_api->stop();
        server_future.wait();
        shutdown_requested = false;
    }
    
    std::string send_http_request(const std::string& path) {
        httplib::Client client("localhost", 8080);
        client.set_connection_timeout(2); // таймаут 2 секунды
        auto res = client.Get(path.c_str());
        return res ? res->body : "";
    }
    
    std::set<std::string> blacklist;
    std::unique_ptr<pgw::SessionManager> session_manager;
    std::unique_ptr<pgw::CDRLogger> cdr_logger;
    std::unique_ptr<pgw::HttpApi> http_api;
    std::atomic<bool> shutdown_requested{false};
    std::future<void> server_future;
};

TEST_F(HttpApiTest, CheckSubscriberActive) {
    // создаем тестовую сессию
    session_manager->try_create_session("123456789012345");
    
    auto response = send_http_request("/check_subscriber?imsi=123456789012345");
    EXPECT_EQ(response, "active");
}

TEST_F(HttpApiTest, CheckSubscriberNotActive) {
    auto response = send_http_request("/check_subscriber?imsi=123456789012345");
    EXPECT_EQ(response, "not active");
}

TEST_F(HttpApiTest, CheckSubscriberMissingParam) {
    auto response = send_http_request("/check_subscriber");
    // проверяем часть сообщения об ошибке
    EXPECT_NE(response.find("IMSI parameter is required"), std::string::npos);
}

TEST_F(HttpApiTest, StopEndpoint) {
    // создаем несколько сессий
    session_manager->try_create_session("111111111111111");
    session_manager->try_create_session("222222222222222");
    
    auto response = send_http_request("/stop");
    EXPECT_EQ(response, "Initiating graceful shutdown...");
    
    // ждем завершения graceful shutdown
    std::this_thread::sleep_for(1500ms);
    
    // проверяем что сессии удалены
    EXPECT_EQ(session_manager->active_sessions(), 0);
}