#include "core/engine.hpp"
#include "core/assets.hpp"
#include "platform/window.hpp"

#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    std::string assetsRoot = "assets";

    // Allow override: ./wmw_pc /path/to/assets
    if (argc > 1) {
        assetsRoot = argv[1];
    }

    std::cout << "=== Where's My Water? PC Port (skeleton) ===\n";
    std::cout << "Assets root: " << assetsRoot << "\n";

    if (!wmw::assets::exists(assetsRoot)) {
        std::cerr << "[ERROR] assets/ folder not found at: " << assetsRoot << "\n";
        std::cerr << "Place the original game assets next to the executable (or pass the path).\n";
        return 1;
    }

    wmw::platform::Window window;
    if (!window.create("Where's My Water? PC (stub)", 960, 640)) {
        std::cerr << "[ERROR] Failed to create window.\n";
        return 1;
    }

    wmw::core::Engine engine;
    if (!engine.init(assetsRoot)) {
        std::cerr << "[ERROR] Engine init failed.\n";
        return 1;
    }

    bool running = true;
    while (running) {
        running = window.pollEvents();
        engine.update(1.0f / 60.0f);
        engine.render();
        window.swap();
    }

    engine.shutdown();
    window.destroy();
    return 0;
}
