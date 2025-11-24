#include "MarketDataProvider.hpp"
#include <chrono>
#include <random>

MarketDataProvider::MarketDataProvider() {
    auto seed = static_cast<unsigned long>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    rng_ = std::mt19937(seed);
}

Quote MarketDataProvider::getQuote(const std::string &ticker) {
    std::uniform_real_distribution<double> price_dist(10.0, 500.0);
    std::uniform_real_distribution<double> pct_dist(-5.0, 5.0);
    std::uniform_real_distribution<double> spread_dist(0.01, 1.0);
    std::uniform_real_distribution<double> cap_dist(1e9, 5e12);
    std::uniform_int_distribution<long long> vol_dist(100000, 50000000);
    std::uniform_real_distribution<double> fiftytwo_dist(5.0, 550.0);

    double price = price_dist(rng_);
    double change = pct_dist(rng_);

    Quote q;
    q.name = ticker + " Corp";
    q.price = price;
    q.market_cap = cap_dist(rng_);
    q.daily_percent_change = change;
    q.volume = vol_dist(rng_);
    q.average_volume = vol_dist(rng_);
    double high = fiftytwo_dist(rng_);
    double low = fiftytwo_dist(rng_);
    if (high < low) std::swap(high, low);
    q.fiftytwo_week_high = high;
    q.fiftytwo_week_low = low;
    double spread = spread_dist(rng_);
    q.bid = price - spread;
    q.ask = price + spread;
    return q;
}

