#include "core/engine.hpp"
#include "core/assets.hpp"
#include "platform/window.hpp"
#include "platform/input.hpp"
#include "platform/filesystem.hpp"
#include "platform/timer.hpp"
#include "platform/splash.hpp"

#include <iostream>
#include <string>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

static void printBanner() {
    std::cout << "================================================\n";
    std::cout << "  Where's My Water?  —  PC Port (Windows x64)\n";
    std::cout << "  Native PE target · no ARM · no JNI\n";
    std::cout << "================================================\n";
}

int main(int argc, char* argv[]) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    printBanner();

    std::string assetsRoot = "assets";
    if (argc > 1) {
        assetsRoot = argv[1];
    }

    std::string levelName = "01_CUT_FIRST";
    if (argc > 2) {
        levelName = argv[2];
    }

    std::cout << "[main] assets root : " << assetsRoot << "\n";
    std::cout << "[main] start level : " << levelName << "\n";

    if (!wmw::assets::exists(assetsRoot)) {
        std::cerr << "[ERROR] assets/ not found or incomplete at: " << assetsRoot << "\n";
        std::cerr << "Expected subfolders: Levels, Sprites, Textures, Audio, Data, ...\n";
        return 1;
    }

    wmw::platform::Window window;
    if (!window.create("Where's My Water? PC (x64)", 1280, 720)) {
        std::cerr << "[ERROR] Window creation failed.\n";
        return 1;
    }

    wmw::platform::Input input;
    wmw::platform::Timer timer;

    // Copyright splash FIRST (Drawing.png from commit d085394)
    {
        const std::string splashPath = wmw::platform::findCopyrightImagePath();
        if (!wmw::platform::showCopyrightSplash(window, input, splashPath)) {
            window.destroy();
            return 0; // quit during splash
        }
    }

    wmw::core::Engine engine;
    if (!engine.init(assetsRoot)) {
        std::cerr << "[ERROR] Engine init failed.\n";
        return 1;
    }

    if (!engine.loadLevel(levelName)) {
        std::cerr << "[WARN] Could not load level '" << levelName << "' — continuing with empty scene.\n";
    }

    bool running = true;
    while (running) {
        float dt = timer.tick();

        running = window.pollEvents(input);
        if (input.quitRequested()) {
            running = false;
        }

        engine.update(dt, input);
        engine.render(window.renderer());
        window.swap();
    }

    engine.shutdown();
    window.destroy();
    return 0;
}
