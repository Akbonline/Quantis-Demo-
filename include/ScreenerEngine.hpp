#pragma once

#include "MarketDataProvider.hpp"
#include "Storage.hpp"
#include "TableRenderer.hpp"
#include "quantis/anomaly/AnomalyEngine.hpp"
#include <atomic>
#include <memory>
#include <string>
#include <vector>

class ScreenerEngine {
public:
    ScreenerEngine(Storage &storage, MarketDataProvider &provider, TableRenderer &renderer);
    ScreenerEngine(Storage &storage, MarketDataProvider &provider, TableRenderer &renderer, AnomalyEngine &anomaly);
    int run(int argc, char **argv);

private:
    int handleScreener(const std::vector<std::string> &args);
    int handleList(bool realtime);
    int handleAlerts(bool realtime, bool alertsOnly);
    int handleAlertsClear();
    int handleAdd(const std::string &ticker);
    int handleRemove(const std::string &ticker);
    int handleExport();

    ScreenerRows collectRows();

    Storage &storage_;
    MarketDataProvider &provider_;
    TableRenderer &renderer_;
    AnomalyEngine *anomaly_;
    std::unique_ptr<AnomalyEngine> owned_anomaly_;
};
