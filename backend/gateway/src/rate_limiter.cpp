#include "gateway.hpp"
#include <chrono>
#include <mutex>
#include <unordered_map>
namespace monetix {
struct Bucket { double tokens; std::chrono::steady_clock::time_point at; };
static std::mutex m;
static std::unordered_map<std::string,Bucket> buckets;
bool rate_limit_allow(const std::string& key, int capacity, double refill, int& remaining) {
    std::lock_guard lock(m);
    auto now=std::chrono::steady_clock::now();
    auto& b=buckets[key];
    if (b.at.time_since_epoch().count()==0) { b.tokens=capacity; b.at=now; }
    const double seconds=std::chrono::duration<double>(now-b.at).count();
    b.tokens=std::min<double>(capacity,b.tokens+seconds*refill); b.at=now;
    if (b.tokens < 1.0) { remaining=static_cast<int>(b.tokens); return false; }
    b.tokens -= 1.0; remaining=static_cast<int>(b.tokens); return true;
}
}
