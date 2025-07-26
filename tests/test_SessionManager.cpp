#include "gtest/gtest.h"
#include "SessionManager.hpp"
#include <set>
#include <thread>
#include <chrono>

using namespace std::chrono_literals;

TEST(SessionManagerTest, CreateSession) {
    std::set<std::string> blacklist = {"123456"};
    pgw::SessionManager manager(30, blacklist, 100);
    
    // успешное создание сессии
    auto result = manager.try_create_session("111111");
    EXPECT_EQ(result, pgw::SessionManager::CreateResult::CREATED);
    EXPECT_TRUE(manager.is_active("111111"));
    
    // попытка создать сессию для IMSI в черном списке
    result = manager.try_create_session("123456");
    EXPECT_EQ(result, pgw::SessionManager::CreateResult::REJECTED_BLACKLIST);
    EXPECT_FALSE(manager.is_active("123456"));
    
    // попытка создать дубликат сессии
    result = manager.try_create_session("111111");
    EXPECT_EQ(result, pgw::SessionManager::CreateResult::ALREADY_EXISTS);
}

TEST(SessionManagerTest, SessionLimit) {
    std::set<std::string> blacklist;
    pgw::SessionManager manager(30, blacklist, 2); // лимит 2 сессии
    
    EXPECT_EQ(manager.try_create_session("111111"), pgw::SessionManager::CreateResult::CREATED);
    EXPECT_EQ(manager.try_create_session("222222"), pgw::SessionManager::CreateResult::CREATED);
    
    // превышение лимита
    auto result = manager.try_create_session("333333");
    EXPECT_EQ(result, pgw::SessionManager::CreateResult::REJECTED_LIMIT);
    EXPECT_FALSE(manager.is_active("333333"));
    EXPECT_EQ(manager.active_sessions(), 2);
}

TEST(SessionManagerTest, SessionExpiration) {
    std::set<std::string> blacklist;
    pgw::SessionManager manager(1, blacklist, 100); // таймаут 1 секунда
    
    manager.try_create_session("111111");
    EXPECT_TRUE(manager.is_active("111111"));
    
    // ждем истечения таймаута
    std::this_thread::sleep_for(1100ms);
    
    // проверяем и удаляем устаревшие сессии
    manager.remove_expired_sessions();
    EXPECT_FALSE(manager.is_active("111111"));
    EXPECT_EQ(manager.active_sessions(), 0);
}

TEST(SessionManagerTest, RemoveSession) {
    std::set<std::string> blacklist;
    pgw::SessionManager manager(30, blacklist, 100);
    
    manager.try_create_session("111111");
    EXPECT_TRUE(manager.is_active("111111"));
    
    manager.remove_session("111111");
    EXPECT_FALSE(manager.is_active("111111"));
    
    // удаление несуществующей сессии
    manager.remove_session("999999");
    EXPECT_FALSE(manager.is_active("999999"));
}