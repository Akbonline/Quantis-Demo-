#pragma once

#include "Types.hpp"
#include <string>

class TableRenderer {
public:
    void render(const ScreenerRows &rows);

private:
    static std::string formatNumber(double value, int precision = 2);
    static std::string formatLargeNumber(double value);
};
