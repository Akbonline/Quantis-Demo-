#pragma once

#include "Types.hpp"
#include <random>
#include <string>

class MarketDataProvider {
public:
    MarketDataProvider();
    Quote getQuote(const std::string &ticker);

private:
    std::mt19937 rng_;
};



