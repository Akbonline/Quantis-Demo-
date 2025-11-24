#include "MarketDataProvider.hpp"
#include "ScreenerEngine.hpp"
#include "Storage.hpp"
#include "TableRenderer.hpp"
#include "quantis/anomaly/AnomalyEngine.hpp"
#include <iostream>

int main(int argc, char **argv) {
    try {
        Storage storage("quantis.db");
        MarketDataProvider provider;
        TableRenderer renderer;
        AnomalyEngine anomaly;
        ScreenerEngine engine(storage, provider, renderer, anomaly);
        return engine.run(argc, argv);
    } catch (const std::exception &ex) {
        std::cerr << "Fatal error: " << ex.what() << "\n";
        return 1;
    }
}
