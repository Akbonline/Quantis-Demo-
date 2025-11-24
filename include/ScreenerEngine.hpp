#pragma once

#include "MarketDataProvider.hpp"
#include "Storage.hpp"
#include "TableRenderer.hpp"
#include <atomic>
#include <string>
#include <vector>

class ScreenerEngine {
public:
    ScreenerEngine(Storage &storage, MarketDataProvider &provider, TableRenderer &renderer);
    int run(int argc, char **argv);

private:
    int handleScreener(const std::vector<std::string> &args);
    int handleList(bool realtime);
    int handleAdd(const std::string &ticker);
    int handleRemove(const std::string &ticker);
    int handleExport();

    Storage &storage_;
    MarketDataProvider &provider_;
    TableRenderer &renderer_;
};
