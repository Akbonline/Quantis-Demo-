#include "ScreenerEngine.hpp"
#include "Types.hpp"
#include <chrono>
#include <csignal>
#include <iostream>
#include <thread>

namespace {
std::vector<std::string> sliceArgs(int argc, char **argv, int start) {
    std::vector<std::string> args;
    for (int i = start; i < argc; ++i) {
        args.emplace_back(argv[i]);
    }
    return args;
}

std::atomic_bool *g_running_flag = nullptr;

void handleSignal(int) {
    if (g_running_flag) {
        g_running_flag->store(false);
    }
}
}

ScreenerEngine::ScreenerEngine(Storage &storage, MarketDataProvider &provider, TableRenderer &renderer)
    : storage_(storage), provider_(provider), renderer_(renderer) {}

int ScreenerEngine::run(int argc, char **argv) {
    if (argc < 2) {
        std::cout << "Usage: quantis screener <command> [options]\n"
                  << "Commands:\n"
                  << "  list [realtime]\n"
                  << "  add SYMBOL\n"
                  << "  remove SYMBOL\n"
                  << "  export csv\n";
        return 1;
    }

    std::string command = argv[1];
    if (command == "screener") {
        return handleScreener(sliceArgs(argc, argv, 2));
    }

    std::cerr << "Unknown command: " << command << "\n";
    return 1;
}

int ScreenerEngine::handleScreener(const std::vector<std::string> &args) {
    if (args.empty()) {
        std::cerr << "Missing screener subcommand\n";
        return 1;
    }

    const std::string &sub = args[0];
    if (sub == "list") {
        bool realtime = args.size() > 1 && args[1] == "realtime";
        return handleList(realtime);
    }
    if (sub == "add") {
        if (args.size() < 2) {
            std::cerr << "Usage: quantis screener add SYMBOL\n";
            return 1;
        }
        return handleAdd(args[1]);
    }
    if (sub == "remove") {
        if (args.size() < 2) {
            std::cerr << "Usage: quantis screener remove SYMBOL\n";
            return 1;
        }
        return handleRemove(args[1]);
    }
    if (sub == "export") {
        if (args.size() < 2 || args[1] != "csv") {
            std::cerr << "Usage: quantis screener export csv\n";
            return 1;
        }
        return handleExport();
    }

    std::cerr << "Unknown screener subcommand: " << sub << "\n";
    return 1;
}

int ScreenerEngine::handleList(bool realtime) {
    if (!realtime) {
        ScreenerRows rows;
        for (const auto &ticker : storage_.listTickers()) {
            rows.emplace_back(ticker, provider_.getQuote(ticker.ticker));
        }
        if (rows.empty()) {
            std::cout << "No tickers tracked. Add one with 'quantis screener add SYMBOL'.\n";
            return 0;
        }
        renderer_.render(rows);
        return 0;
    }

    std::atomic_bool running{true};
    g_running_flag = &running;
    std::signal(SIGINT, handleSignal);

    while (running.load()) {
        std::cout << "\033[2J\033[H"; // clear screen and move cursor home
        ScreenerRows rows;
        for (const auto &ticker : storage_.listTickers()) {
            rows.emplace_back(ticker, provider_.getQuote(ticker.ticker));
        }
        if (rows.empty()) {
            std::cout << "No tickers tracked. Add one with 'quantis screener add SYMBOL'.\n";
            std::cout.flush();
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }
        renderer_.render(rows);
        std::cout.flush();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    g_running_flag = nullptr;
    return 0;
}

int ScreenerEngine::handleAdd(const std::string &ticker) {
    if (storage_.addTicker(ticker)) {
        std::cout << "Added ticker " << ticker << "\n";
        return 0;
    }
    return 1;
}

int ScreenerEngine::handleRemove(const std::string &ticker) {
    if (storage_.removeTicker(ticker)) {
        std::cout << "Removed ticker " << ticker << "\n";
        return 0;
    }
    return 1;
}

int ScreenerEngine::handleExport() {
    ScreenerRows rows;
    for (const auto &ticker : storage_.listTickers()) {
        rows.emplace_back(ticker, provider_.getQuote(ticker.ticker));
    }
    const std::string filename = "quantis_export.csv";
    if (storage_.exportToCsv(filename, rows)) {
        std::cout << "Exported to " << filename << "\n";
        return 0;
    }
    return 1;
}
