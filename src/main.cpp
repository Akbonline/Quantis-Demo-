#include "ScreenerEngine.hpp"
#include "Storage.hpp"
#include "MarketDataProvider.hpp"
#include "TableRenderer.hpp"
#include <iostream>

int main(int argc, char **argv) {
    try {
        Storage storage("quantis.db");
        MarketDataProvider provider;
        TableRenderer renderer;
        ScreenerEngine engine(storage, provider, renderer);
        return engine.run(argc, argv);
    } catch (const std::exception &ex) {
        std::cerr << "Fatal error: " << ex.what() << "\n";
        return 1;
    }
}
