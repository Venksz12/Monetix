#pragma once
#include <boost/asio/io_context.hpp>
#include <string>

namespace monetix {
class HttpGateway {
public:
    HttpGateway(boost::asio::io_context& io, unsigned short port);
    void run();
private:
    boost::asio::io_context& io_;
    unsigned short port_;
};
std::string authenticate_api_key(const std::string& api_key);
bool rate_limit_allow(const std::string& key, int capacity, double refill_per_sec, int& remaining);
bool publish_event(const std::string& subject, const std::string& payload);
}
