#pragma once

#include "Types.hpp"
#include "quantis/anomaly/StatsBuffer.hpp"
#include <string>
#include <unordered_map>
#include <vector>

class AnomalyEngine {
public:
    std::vector<std::string> evaluate(const std::string &ticker, const Quote &quote);
    void clear();

private:
    std::unordered_map<std::string, StatsBuffer> buffers_;
};
