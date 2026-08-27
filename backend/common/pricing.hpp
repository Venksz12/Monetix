#pragma once
#include <algorithm>
#include <cstdint>
#include <vector>

namespace monetix {
enum class PricingModel { Payg, Subscription, Tiered, Prepaid, AiTokens };
struct PriceRule {
    int64_t unit_minor{0};
    int64_t included{0};
    int64_t tier_limit{0};
};
inline int64_t calculate_charge(PricingModel model, int64_t requests,
                                int64_t unit_minor, int64_t included,
                                const std::vector<PriceRule>& tiers = {},
                                int64_t input_tokens = 0, int64_t output_tokens = 0) {
    requests = std::max<int64_t>(0, requests);
    switch (model) {
        case PricingModel::Payg:
            return requests * unit_minor;
        case PricingModel::Subscription:
            return std::max<int64_t>(0, requests - included) * unit_minor;
        case PricingModel::Tiered: {
            int64_t remaining = requests, previous = 0, total = 0;
            for (const auto& t : tiers) {
                const int64_t upper = t.tier_limit <= 0 ? requests : t.tier_limit;
                const int64_t in_tier = std::max<int64_t>(0, std::min(remaining, upper - previous));
                total += in_tier * t.unit_minor;
                remaining -= in_tier; previous = upper;
                if (remaining <= 0) break;
            }
            if (remaining > 0 && !tiers.empty()) total += remaining * tiers.back().unit_minor;
            return total;
        }
        case PricingModel::Prepaid:
            return requests * unit_minor;
        case PricingModel::AiTokens:
            return (input_tokens + output_tokens) * unit_minor;
    }
    return 0;
}
}
