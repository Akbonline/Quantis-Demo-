#pragma once

#include "Types.hpp"
#include <vector>
#include <string>

class TableRenderer {
public:
    void render(const ScreenerRows &rows);
    void renderWithAlerts(const ScreenerRows &rows, const std::vector<std::vector<std::string>> &alerts,
                          bool alertsOnly);

private:
    static std::string formatNumber(double value, int precision = 2);
    static std::string formatLargeNumber(double value);
    static std::string colorize(const std::string &alert);
    static std::string joinAlerts(const std::vector<std::string> &alerts);
};
