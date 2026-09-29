#pragma once

#include <string>

struct SDL_Window;
struct SDL_Renderer;

namespace wmw::platform {

class Window {
public:
    Window() = default;
    ~Window();

    bool create(const std::string& title, int width, int height);
    void destroy();

    // Returns false when the user requests quit
    bool pollEvents();
    void swap();

    int width() const { return width_; }
    int height() const { return height_; }

private:
    SDL_Window* window_ = nullptr;
    SDL_Renderer* renderer_ = nullptr;
    int width_ = 0;
    int height_ = 0;
};

} // namespace wmw::platform
