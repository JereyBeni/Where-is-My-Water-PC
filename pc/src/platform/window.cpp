#include "platform/window.hpp"
#include "platform/input.hpp"

#include <SDL.h>
#include <iostream>

namespace wmw::platform {

Window::~Window() {
    destroy();
}

bool Window::create(const std::string& title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return false;
    }

    window_ = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    if (!window_) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << "\n";
        SDL_Quit();
        return false;
    }

    renderer_ = SDL_CreateRenderer(
        window_, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if (!renderer_) {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window_);
        window_ = nullptr;
        SDL_Quit();
        return false;
    }

    width_ = width;
    height_ = height;

    SDL_RendererInfo info{};
    if (SDL_GetRendererInfo(renderer_, &info) == 0) {
        std::cout << "[Window] renderer: " << info.name << "\n";
    }
    std::cout << "[Window] " << width_ << "x" << height_ << " (Windows x64 / SDL2)\n";
    return true;
}

void Window::destroy() {
    if (renderer_) {
        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
    }
    if (window_) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }
    SDL_Quit();
}

bool Window::pollEvents(Input& input) {
    input.beginFrame();

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            input.setQuit(true);
            return false;
        }
        if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
            width_ = e.window.data1;
            height_ = e.window.data2;
        }
        input.handleEvent(e);
    }
    return true;
}

void Window::swap() {
    if (renderer_) {
        SDL_RenderPresent(renderer_);
    }
}

} // namespace wmw::platform
