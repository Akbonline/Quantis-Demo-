#include "TableRenderer.hpp"
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace {
std::string truncate(const std::string &text, std::size_t width) {
    if (text.size() <= width) return text;
    return text.substr(0, width - 3) + "...";
}
}

void TableRenderer::render(const ScreenerRows &rows) {
    const int ticker_w = 8;
    const int name_w = 20;
    const int price_w = 10;
    const int cap_w = 14;
    const int pct_w = 8;
    const int vol_w = 14;
    const int avg_vol_w = 14;
    const int level_w = 12;

    std::cout << std::left
              << std::setw(ticker_w) << "Ticker"
              << std::setw(name_w) << "Name"
              << std::setw(price_w) << "Price"
              << std::setw(cap_w) << "Market Cap"
              << std::setw(pct_w) << "%Chg"
              << std::setw(vol_w) << "Volume"
              << std::setw(avg_vol_w) << "Avg Volume"
              << std::setw(level_w) << "52W High"
              << std::setw(level_w) << "52W Low"
              << std::setw(price_w) << "Bid"
              << std::setw(price_w) << "Ask"
              << "Notes"
              << "\n";

    std::cout << std::string(ticker_w + name_w + price_w * 3 + cap_w + pct_w + vol_w * 2 + level_w * 2 + avg_vol_w + 10, '-')
              << "\n";

    for (const auto &row : rows) {
        const auto &meta = row.first;
        const auto &q = row.second;
        std::cout << std::left
                  << std::setw(ticker_w) << truncate(meta.ticker, ticker_w)
                  << std::setw(name_w) << truncate(q.name.empty() ? meta.name : q.name, name_w)
                  << std::right
                  << std::setw(price_w) << formatNumber(q.price)
                  << std::setw(cap_w) << formatLargeNumber(q.market_cap)
                  << std::setw(pct_w) << formatNumber(q.daily_percent_change, 2)
                  << std::setw(vol_w) << static_cast<long long>(q.volume)
                  << std::setw(avg_vol_w) << static_cast<long long>(q.average_volume)
                  << std::setw(level_w) << formatNumber(q.fiftytwo_week_high)
                  << std::setw(level_w) << formatNumber(q.fiftytwo_week_low)
                  << std::setw(price_w) << formatNumber(q.bid)
                  << std::setw(price_w) << formatNumber(q.ask)
                  << " " << truncate(meta.notes, 30)
                  << "\n";
    }
}

void TableRenderer::renderWithAlerts(const ScreenerRows &rows, const std::vector<std::vector<std::string>> &alerts, bool alertsOnly) {
    const int ticker_w = 8;
    const int alerts_w = 40;
    if (alertsOnly) {
        std::cout << std::left << std::setw(ticker_w) << "Ticker" << "Alerts" << "\n";
        std::cout << std::string(ticker_w + alerts_w, '-') << "\n";
        for (std::size_t i = 0; i < rows.size(); ++i) {
            const auto &meta = rows[i].first;
            std::cout << std::left << std::setw(ticker_w) << truncate(meta.ticker, ticker_w)
                      << truncate(joinAlerts(alerts[i]), alerts_w) << "\n";
        }
        return;
    }

    const int name_w = 20;
    const int price_w = 10;
    const int cap_w = 14;
    const int pct_w = 8;
    const int vol_w = 14;
    const int avg_vol_w = 14;
    const int level_w = 12;

    std::cout << std::left
              << std::setw(ticker_w) << "Ticker"
              << std::setw(name_w) << "Name"
              << std::setw(price_w) << "Price"
              << std::setw(cap_w) << "Market Cap"
              << std::setw(pct_w) << "%Chg"
              << std::setw(vol_w) << "Volume"
              << std::setw(avg_vol_w) << "Avg Volume"
              << std::setw(level_w) << "52W High"
              << std::setw(level_w) << "52W Low"
              << std::setw(price_w) << "Bid"
              << std::setw(price_w) << "Ask"
              << std::setw(alerts_w) << "Alerts"
              << "Notes"
              << "\n";

    std::cout << std::string(ticker_w + name_w + price_w * 3 + cap_w + pct_w + vol_w * 2 + level_w * 2 + avg_vol_w + alerts_w + 10, '-')
              << "\n";

    for (std::size_t i = 0; i < rows.size(); ++i) {
        const auto &meta = rows[i].first;
        const auto &q = rows[i].second;
        std::cout << std::left
                  << std::setw(ticker_w) << truncate(meta.ticker, ticker_w)
                  << std::setw(name_w) << truncate(q.name.empty() ? meta.name : q.name, name_w)
                  << std::right
                  << std::setw(price_w) << formatNumber(q.price)
                  << std::setw(cap_w) << formatLargeNumber(q.market_cap)
                  << std::setw(pct_w) << formatNumber(q.daily_percent_change, 2)
                  << std::setw(vol_w) << static_cast<long long>(q.volume)
                  << std::setw(avg_vol_w) << static_cast<long long>(q.average_volume)
                  << std::setw(level_w) << formatNumber(q.fiftytwo_week_high)
                  << std::setw(level_w) << formatNumber(q.fiftytwo_week_low)
                  << std::setw(price_w) << formatNumber(q.bid)
                  << std::setw(price_w) << formatNumber(q.ask)
                  << std::left << std::setw(alerts_w) << truncate(joinAlerts(alerts[i]), alerts_w)
                  << truncate(meta.notes, 30)
                  << "\n";
    }
}

std::string TableRenderer::formatNumber(double value, int precision) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision) << value;
    return oss.str();
}

std::string TableRenderer::formatLargeNumber(double value) {
    const char *suffixes[] = {"", "K", "M", "B", "T"};
    int idx = 0;
    while (value >= 1000.0 && idx < 4) {
        value /= 1000.0;
        ++idx;
    }
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(idx == 0 ? 0 : 2) << value << suffixes[idx];
    return oss.str();
}

std::string TableRenderer::colorize(const std::string &alert) {
    const std::string reset = "\033[0m";
    std::string color;
    if (alert == "VOL_SPIKE" || alert == "VOLATILITY_SURGE") {
        color = "\033[33m"; // yellow
    } else if (alert == "BREAKOUT_UP") {
        color = "\033[32m"; // green
    } else if (alert == "MOMENTUM_FLIP") {
        color = "\033[34m"; // blue
    } else {
        color = "\033[31m"; // red for negative conditions
    }
    return color + alert + reset;
}

std::string TableRenderer::joinAlerts(const std::vector<std::string> &alerts) {
    if (alerts.empty()) return "-";
    std::ostringstream oss;
    for (std::size_t i = 0; i < alerts.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << colorize(alerts[i]);
    }
    return oss.str();
}

