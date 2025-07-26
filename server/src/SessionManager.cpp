#include "SessionManager.hpp"

namespace pgw {

using namespace std::chrono;

SessionManager::SessionManager(unsigned timeout_sec, 
                               const std::set<std::string>& blacklist,
                               unsigned max_sessions)
    : session_timeout_(timeout_sec),
      blacklist_(blacklist),
      max_sessions_(max_sessions) {
    
    spdlog::debug("SessionManager initialized with timeout: {}s, max sessions: {}", 
                  timeout_sec, max_sessions);
}

SessionManager::CreateResult SessionManager::try_create_session(const std::string& imsi) {
    std::lock_guard lock(mutex_);
    
    // проверка черного списка
    if (blacklist_.find(imsi) != blacklist_.end()) {
        spdlog::info("Session rejected (blacklist): {}", imsi);
        return CreateResult::REJECTED_BLACKLIST;
    }
    
    // проверка существующей сессии
    if (sessions_.find(imsi) != sessions_.end()) {
        spdlog::debug("Session already exists: {}", imsi);
        return CreateResult::ALREADY_EXISTS;
    }
    
    // проверка лимита сессий
    if (sessions_.size() >= max_sessions_) {
        spdlog::warn("Session limit reached ({}), rejecting: {}", max_sessions_, imsi);
        return CreateResult::REJECTED_LIMIT;
    }
    
    // создание новой сессии
    sessions_[imsi] = Session{steady_clock::now()};
    spdlog::info("Session created: {}", imsi);
    return CreateResult::CREATED;
}

bool SessionManager::is_active(const std::string& imsi) const {
    std::lock_guard lock(mutex_);
    return sessions_.find(imsi) != sessions_.end();
}

void SessionManager::remove_session(const std::string& imsi) {
    std::lock_guard lock(mutex_);
    if (sessions_.erase(imsi) > 0) {
        spdlog::info("Session removed: {}", imsi);
    }
}

void SessionManager::remove_expired_sessions() {
    const auto now = steady_clock::now();
    std::lock_guard lock(mutex_);
    
    unsigned removed_count = 0;
    for (auto it = sessions_.begin(); it != sessions_.end(); ) {
        if (now - it->second.created_at > session_timeout_) {
            spdlog::info("Session expired: {}", it->first);
            it = sessions_.erase(it);
            removed_count++;
        } else {
            ++it;
        }
    }
    
    if (removed_count > 0) {
        spdlog::info("Removed {} expired sessions", removed_count);
    }
}

unsigned SessionManager::active_sessions() const {
    std::lock_guard lock(mutex_);
    return sessions_.size();
}

void SessionManager::graceful_shutdown(unsigned rate) {
    // реализация graceful shutdown будет добавлена позже
    spdlog::info("Graceful shutdown initiated, rate: {}", rate);
}

} // namespace pgw