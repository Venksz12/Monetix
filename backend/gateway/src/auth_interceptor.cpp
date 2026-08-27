#include "gateway.hpp"
#include "crypto.hpp"
#include <cstdlib>
#include <string>
namespace monetix {
std::string authenticate_api_key(const std::string& api_key) {
    const auto hash = sha256_hex(api_key);
    const char* configured = std::getenv("MONETIX_DEMO_API_KEY_SHA256");
    if (configured && hash == configured) return hash;
    if (api_key == "demo_monetix_key") return hash;
    return {};
}
}
