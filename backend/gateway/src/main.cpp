#include "gateway.hpp"
#include <boost/asio/io_context.hpp>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <vector>
int main() {
    try {
        boost::asio::io_context io;
        const char* p=std::getenv("GATEWAY_PORT");
        monetix::HttpGateway gateway(io, static_cast<unsigned short>(std::atoi(p?p:"8080")));
        gateway.run();
        const unsigned n = std::max(2u, std::thread::hardware_concurrency());
        std::vector<std::thread> threads;
        for (unsigned i=0;i<n;i++) threads.emplace_back([&]{io.run();});
        for (auto& t: threads) t.join();
    } catch (const std::exception& e) { std::cerr << e.what() << '\n'; return 1; }
}
