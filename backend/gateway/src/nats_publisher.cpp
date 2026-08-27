#include "gateway.hpp"
#include <cstdlib>
#include <iostream>
#include <string>
namespace monetix {
bool publish_event(const std::string& subject, const std::string& payload) {
    // Production deployment should use the NATS C/C++ client and JetStream publish ACKs.
    // Keeping this adapter side-effect free makes the gateway runnable without a native NATS SDK.
    if (std::getenv("NATS_URL")) {
        std::clog << "event subject=" << subject << " payload=" << payload << '\n';
    }
    return true;
}
}
