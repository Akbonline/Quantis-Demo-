#include "Storage.hpp"
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

Storage::Storage(const std::string &db_path) : db_path_(db_path) {
    if (sqlite3_open(db_path_.c_str(), &db_) != SQLITE_OK) {
        throw std::runtime_error("Failed to open database: " + std::string(sqlite3_errmsg(db_)));
    }
    initialize();
}

Storage::~Storage() {
    if (db_) {
        sqlite3_close(db_);
    }
}

void Storage::initialize() {
    const char *sql = R"SQL(
        CREATE TABLE IF NOT EXISTS tickers (
            ticker TEXT PRIMARY KEY,
            name TEXT,
            sector TEXT,
            industry TEXT,
            notes TEXT,
            date_added TEXT
        );
    )SQL";

    char *errmsg = nullptr;
    if (sqlite3_exec(db_, sql, nullptr, nullptr, &errmsg) != SQLITE_OK) {
        std::string message = errmsg ? errmsg : "unknown error";
        sqlite3_free(errmsg);
        throw std::runtime_error("Failed to initialize database: " + message);
    }
}

bool Storage::tickerExists(const std::string &ticker) {
    const char *sql = "SELECT COUNT(*) FROM tickers WHERE ticker = ?";
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, ticker.c_str(), -1, SQLITE_TRANSIENT);
    bool exists = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int count = sqlite3_column_int(stmt, 0);
        exists = count > 0;
    }
    sqlite3_finalize(stmt);
    return exists;
}

bool Storage::addTicker(const std::string &ticker, const std::string &name, const std::string &sector,
                        const std::string &industry, const std::string &notes) {
    if (tickerExists(ticker)) {
        std::cerr << "Ticker already exists: " << ticker << "\n";
        return false;
    }

    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");

    const char *sql = "INSERT INTO tickers (ticker, name, sector, industry, notes, date_added) VALUES (?, ?, ?, ?, ?, ?)";
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare insert statement: " << sqlite3_errmsg(db_) << "\n";
        return false;
    }

    sqlite3_bind_text(stmt, 1, ticker.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, sector.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, industry.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, notes.c_str(), -1, SQLITE_TRANSIENT);
    auto timestamp = oss.str();
    sqlite3_bind_text(stmt, 6, timestamp.c_str(), -1, SQLITE_TRANSIENT);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    if (!success) {
        std::cerr << "Failed to add ticker: " << sqlite3_errmsg(db_) << "\n";
    }
    sqlite3_finalize(stmt);
    return success;
}

bool Storage::removeTicker(const std::string &ticker) {
    const char *sql = "DELETE FROM tickers WHERE ticker = ?";
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare delete statement: " << sqlite3_errmsg(db_) << "\n";
        return false;
    }
    sqlite3_bind_text(stmt, 1, ticker.c_str(), -1, SQLITE_TRANSIENT);
    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    if (!success) {
        std::cerr << "Failed to remove ticker: " << sqlite3_errmsg(db_) << "\n";
    }
    sqlite3_finalize(stmt);
    return success;
}

std::vector<TickerRecord> Storage::listTickers() {
    const char *sql = "SELECT ticker, name, sector, industry, notes, date_added FROM tickers ORDER BY ticker";
    sqlite3_stmt *stmt = nullptr;
    std::vector<TickerRecord> records;

    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare select statement: " << sqlite3_errmsg(db_) << "\n";
        return records;
    }

    auto readText = [](sqlite3_stmt *statement, int col) {
        const unsigned char *text = sqlite3_column_text(statement, col);
        return text ? reinterpret_cast<const char *>(text) : "";
    };

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        TickerRecord rec;
        rec.ticker = readText(stmt, 0);
        rec.name = readText(stmt, 1);
        rec.sector = readText(stmt, 2);
        rec.industry = readText(stmt, 3);
        rec.notes = readText(stmt, 4);
        rec.date_added = readText(stmt, 5);
        records.push_back(std::move(rec));
    }
    sqlite3_finalize(stmt);
    return records;
}

bool Storage::exportToCsv(const std::string &filename, const ScreenerRows &rows) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Unable to open file for writing: " << filename << "\n";
        return false;
    }

    file << "ticker,name,sector,industry,notes,date_added,price,market_cap,daily_percent_change,volume,average_volume,52w_high,52w_low,bid,ask\n";
    for (const auto &row : rows) {
        const auto &meta = row.first;
        const auto &quote = row.second;
        auto writeField = [&file](const auto &value) {
            file << '"' << value << '"';
        };

        writeField(meta.ticker); file << ',';
        writeField(meta.name); file << ',';
        writeField(meta.sector); file << ',';
        writeField(meta.industry); file << ',';
        writeField(meta.notes); file << ',';
        writeField(meta.date_added); file << ',';
        writeField(quote.price); file << ',';
        writeField(quote.market_cap); file << ',';
        writeField(quote.daily_percent_change); file << ',';
        writeField(quote.volume); file << ',';
        writeField(quote.average_volume); file << ',';
        writeField(quote.fiftytwo_week_high); file << ',';
        writeField(quote.fiftytwo_week_low); file << ',';
        writeField(quote.bid); file << ',';
        writeField(quote.ask);
        file << "\n";
    }
    return true;
}
