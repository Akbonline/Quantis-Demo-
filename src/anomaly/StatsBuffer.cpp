#include "quantis/anomaly/StatsBuffer.hpp"
#include <cmath>
#include <numeric>

StatsBuffer::StatsBuffer(std::size_t capacity) : capacity_(capacity) {}

void StatsBuffer::addSample(const Quote &quote) {
    if (samples_.size() == capacity_) {
        samples_.pop_front();
    }
    samples_.push_back(quote);
}

std::size_t StatsBuffer::size() const { return samples_.size(); }

bool StatsBuffer::empty() const { return samples_.empty(); }

double StatsBuffer::latestPrice() const { return samples_.empty() ? 0.0 : samples_.back().price; }

double StatsBuffer::latestSpread() const {
    if (samples_.empty()) return 0.0;
    const auto &q = samples_.back();
    return q.ask - q.bid;
}

double StatsBuffer::priceReturn() const {
    if (samples_.size() < 2) return 0.0;
    const auto &last = samples_.back();
    const auto &prev = samples_[samples_.size() - 2];
    if (prev.price == 0.0) return 0.0;
    return (last.price - prev.price) / prev.price;
}

double StatsBuffer::recentVolatility() const {
    auto returns = computeReturns();
    if (returns.size() < 2) return 0.0;
    double mean = std::accumulate(returns.begin(), returns.end(), 0.0) / returns.size();
    double accum = 0.0;
    for (double r : returns) {
        double diff = r - mean;
        accum += diff * diff;
    }
    return std::sqrt(accum / returns.size());
}

double StatsBuffer::meanSpread() const {
    if (samples_.empty()) return 0.0;
    double sum = 0.0;
    for (const auto &q : samples_) {
        sum += (q.ask - q.bid);
    }
    return sum / static_cast<double>(samples_.size());
}

double StatsBuffer::shortTermSlope() const {
    if (samples_.size() < 2) return 0.0;
    std::size_t window = std::min<std::size_t>(10, samples_.size());
    const auto &first = samples_[samples_.size() - window];
    const auto &last = samples_.back();
    return (last.price - first.price) / static_cast<double>(window);
}

double StatsBuffer::longTermSlope() const {
    if (samples_.size() < 2) return 0.0;
    std::size_t window = std::min<std::size_t>(60, samples_.size());
    const auto &first = samples_[samples_.size() - window];
    const auto &last = samples_.back();
    return (last.price - first.price) / static_cast<double>(window);
}

std::vector<double> StatsBuffer::computeReturns() const {
    std::vector<double> returns;
    if (samples_.size() < 2) return returns;
    returns.reserve(samples_.size() - 1);
    for (std::size_t i = 1; i < samples_.size(); ++i) {
        double prev = samples_[i - 1].price;
        if (prev == 0.0) {
            returns.push_back(0.0);
        } else {
            returns.push_back((samples_[i].price - prev) / prev);
        }
    }
    return returns;
}

