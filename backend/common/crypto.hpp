#pragma once
#include <openssl/sha.h>
#include <iomanip>
#include <sstream>
#include <string>

namespace monetix {
inline std::string sha256_hex(const std::string& input) {
    unsigned char digest[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(input.data()), input.size(), digest);
    std::ostringstream out;
    for (auto c : digest) out << std::hex << std::setw(2) << std::setfill('0') << (int)c;
    return out.str();
}
}
