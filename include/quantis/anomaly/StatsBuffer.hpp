#pragma once

#include "Types.hpp"
#include <deque>
#include <vector>

class StatsBuffer {
public:
    explicit StatsBuffer(std::size_t capacity = 60);

    void addSample(const Quote &quote);
    std::size_t size() const;
    bool empty() const;

    double latestPrice() const;
    double latestSpread() const;

    double priceReturn() const;
    double recentVolatility() const;
    double meanSpread() const;

    double shortTermSlope() const;
    double longTermSlope() const;

private:
    std::vector<double> computeReturns() const;

    std::deque<Quote> samples_;
    std::size_t capacity_;
};
