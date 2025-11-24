#pragma once

#include "Types.hpp"
#include <sqlite3.h>
#include <string>
#include <vector>

class Storage {
public:
    explicit Storage(const std::string &db_path);
    ~Storage();

    bool addTicker(const std::string &ticker,
                   const std::string &name = "",
                   const std::string &sector = "",
                   const std::string &industry = "",
                   const std::string &notes = "");
    bool removeTicker(const std::string &ticker);
    std::vector<TickerRecord> listTickers();
    bool exportToCsv(const std::string &filename, const ScreenerRows &rows);

private:
    void initialize();
    bool tickerExists(const std::string &ticker);

    sqlite3 *db_{};
    std::string db_path_;
};
