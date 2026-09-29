#include "platform/splash.hpp"
#include "platform/window.hpp"
#include "platform/input.hpp"
#include "platform/timer.hpp"
#include "platform/filesystem.hpp"

#include <SDL.h>
#include <SDL_image.h>

#include <iostream>
#include <vector>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace wmw::platform {

namespace {

std::string executableDir() {
#ifdef _WIN32
    char buf[MAX_PATH]{};
    DWORD n = GetModuleFileNameA(nullptr, buf, MAX_PATH);
    if (n == 0 || n >= MAX_PATH) return ".";
    std::string path(buf, n);
    size_t slash = path.find_last_of("\\/");
    if (slash == std::string::npos) return ".";
    return path.substr(0, slash);
#else
    return ".";
#endif
}

} // namespace

std::string findCopyrightImagePath() {
    const std::string exeDir = executableDir();
    const std::vector<std::string> candidates = {
        fs::join(exeDir, "Drawing.png"),
        fs::join(exeDir, "pc/Drawing.png"),
        "Drawing.png",
        "pc/Drawing.png",
        "../Drawing.png",
        "../pc/Drawing.png",
    };
    for (const auto& c : candidates) {
        if (fs::fileExists(c)) {
            return c;
        }
    }
    return {};
}

bool showCopyrightSplash(Window& window, Input& input,
                         const std::string& imagePath,
                         float minSeconds,
                         float maxSeconds) {
    if (imagePath.empty() || !fs::fileExists(imagePath)) {
        std::cerr << "[Splash] Drawing.png not found — skipping copyright splash.\n";
        return true;
    }

    // SDL2_image for PNG
    const int imgFlags = IMG_INIT_PNG;
    if ((IMG_Init(imgFlags) & imgFlags) != imgFlags) {
        std::cerr << "[Splash] IMG_Init PNG failed: " << IMG_GetError() << "\n";
        return true; // non-fatal
    }

    SDL_Surface* surface = IMG_Load(imagePath.c_str());
    if (!surface) {
        std::cerr << "[Splash] IMG_Load failed: " << IMG_GetError() << "\n";
        IMG_Quit();
        return true;
    }

    SDL_Renderer* r = window.renderer();
    SDL_Texture* tex = SDL_CreateTextureFromSurface(r, surface);
    const int imgW = surface->w;
    const int imgH = surface->h;
    SDL_FreeSurface(surface);

    if (!tex) {
        std::cerr << "[Splash] CreateTexture failed: " << SDL_GetError() << "\n";
        IMG_Quit();
        return true;
    }

    std::cout << "[Splash] copyright image: " << imagePath
              << " (" << imgW << "x" << imgH << ")\n";

    Timer timer;
    float elapsed = 0.f;
    bool running = true;
    bool dismissed = false;

    while (running) {
        const float dt = timer.tick();
        elapsed += dt;

        running = window.pollEvents(input);
        if (input.quitRequested()) {
            SDL_DestroyTexture(tex);
            IMG_Quit();
            return false; // user quit during splash
        }

        // Click or any key (except we already handle Esc as quit) dismisses after min time
        if (elapsed >= minSeconds) {
            if (input.mouseDown()) {
                dismissed = true;
            }
            // Space / Enter / any letter-ish: use a few scancodes
            if (input.keyPressed(SDL_SCANCODE_SPACE) ||
                input.keyPressed(SDL_SCANCODE_RETURN) ||
                input.keyPressed(SDL_SCANCODE_KP_ENTER)) {
                dismissed = true;
            }
        }
        if (elapsed >= maxSeconds) {
            dismissed = true;
        }
        if (dismissed) {
            break;
        }

        // Letterbox fit
        int sw = window.width();
        int sh = window.height();
        float scale = 1.f;
        if (imgW > 0 && imgH > 0) {
            const float sx = static_cast<float>(sw) / static_cast<float>(imgW);
            const float sy = static_cast<float>(sh) / static_cast<float>(imgH);
            scale = sx < sy ? sx : sy;
        }
        const int dw = static_cast<int>(imgW * scale);
        const int dh = static_cast<int>(imgH * scale);
        SDL_Rect dst{
            (sw - dw) / 2,
            (sh - dh) / 2,
            dw,
            dh
        };

        SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
        SDL_RenderClear(r);
        SDL_RenderCopy(r, tex, nullptr, &dst);
        window.swap();
    }

    SDL_DestroyTexture(tex);
    IMG_Quit();
    return true;
}

} // namespace wmw::platform
