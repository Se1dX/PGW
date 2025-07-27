#pragma once
#include <string>
#include <netinet/in.h>
#include <spdlog/spdlog.h>
#include "Config.hpp"

namespace pgw {

class UdpClient {
public:
    UdpClient(const ClientConfig& config);
    
    std::string send_request(const std::string& imsi);

private:
    int sockfd_;
    sockaddr_in server_addr_;
    const ClientConfig& config_;
};

} // namespace pgw