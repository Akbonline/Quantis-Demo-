#pragma once

#include <string>
#include <vector>

struct TickerRecord {
    std::string ticker;
    std::string name;
    std::string sector;
    std::string industry;
    std::string notes;
    std::string date_added;
};

struct Quote {
    std::string name;
    double price{};
    double market_cap{};
    double daily_percent_change{};
    long long volume{};
    long long average_volume{};
    double fiftytwo_week_high{};
    double fiftytwo_week_low{};
    double bid{};
    double ask{};
};

using ScreenerRow = std::pair<TickerRecord, Quote>;
using ScreenerRows = std::vector<ScreenerRow>;
