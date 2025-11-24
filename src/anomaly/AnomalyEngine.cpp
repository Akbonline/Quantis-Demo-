#include "quantis/anomaly/AnomalyEngine.hpp"
#include <cmath>

namespace {
bool oppositeSigns(double a, double b) {
    return (a > 0 && b < 0) || (a < 0 && b > 0);
}
}

std::vector<std::string> AnomalyEngine::evaluate(const std::string &ticker, const Quote &quote) {
    auto &buffer = buffers_[ticker];
    buffer.addSample(quote);

    std::vector<std::string> alerts;

    // Rule A: Unusual volume
    if (quote.average_volume > 0 && static_cast<double>(quote.volume) / static_cast<double>(quote.average_volume) > 2.0) {
        alerts.emplace_back("VOL_SPIKE");
    }

    // Rule B: Volatility surge
    double price_ret = buffer.priceReturn();
    double recent_vol = buffer.recentVolatility();
    if (recent_vol > 0.0 && std::abs(price_ret) > 1.5 * recent_vol) {
        alerts.emplace_back("VOLATILITY_SURGE");
    }

    double spread = buffer.latestSpread();
    double mean_spread = buffer.meanSpread();

    // Rule C: Spread widening
    if (mean_spread > 0.0 && spread > mean_spread * 2.0) {
        alerts.emplace_back("SPREAD_WIDE");
    }

    // Rule D: Price breakout
    if (quote.price > quote.fiftytwo_week_high * 0.995) {
        alerts.emplace_back("BREAKOUT_UP");
    } else if (quote.price < quote.fiftytwo_week_low * 1.005) {
        alerts.emplace_back("BREAKOUT_DOWN");
    }

    // Rule E: Liquidity compression
    if (quote.average_volume > 0 && quote.volume < static_cast<long long>(quote.average_volume * 0.4) &&
        mean_spread > 0.0 && spread > mean_spread * 1.5) {
        alerts.emplace_back("LOW_LIQUIDITY");
    }

    // Rule F: Momentum shift
    double short_slope = buffer.shortTermSlope();
    double long_slope = buffer.longTermSlope();
    if (buffer.size() >= 10 && oppositeSigns(short_slope, long_slope) &&
        std::abs(short_slope) > std::abs(long_slope) * 1.5) {
        alerts.emplace_back("MOMENTUM_FLIP");
    }

    return alerts;
}

void AnomalyEngine::clear() { buffers_.clear(); }

