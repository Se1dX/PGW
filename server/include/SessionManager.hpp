#pragma once

#include <unordered_map>
#include <set>
#include <mutex>
#include <chrono>
#include <string>
#include <memory>
#include <spdlog/spdlog.h>

namespace pgw {

class SessionManager {
public:
    SessionManager(unsigned timeout_sec, 
                   const std::set<std::string>& blacklist,
                   unsigned max_sessions);
    
    enum class CreateResult {
        CREATED,
        REJECTED_BLACKLIST,
        REJECTED_LIMIT,
        ALREADY_EXISTS
    };
    
    CreateResult try_create_session(const std::string& imsi);
    bool is_active(const std::string& imsi) const;
    void remove_session(const std::string& imsi);
    void remove_expired_sessions();
    unsigned active_sessions() const;
    void graceful_shutdown(unsigned rate);

private:
    struct Session {
        std::chrono::steady_clock::time_point created_at;
    };

    mutable std::mutex mutex_;
    std::unordered_map<std::string, Session> sessions_;
    const std::set<std::string>& blacklist_;
    const std::chrono::seconds session_timeout_;
    const unsigned max_sessions_;
};

} // namespace pgw