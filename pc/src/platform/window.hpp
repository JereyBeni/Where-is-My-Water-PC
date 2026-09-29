#pragma once

#include <string>

struct SDL_Window;
struct SDL_Renderer;

namespace wmw::platform {

class Input;

// [REIMPLEMENTED] SDL2 window + renderer host for Windows x64.
// Replaces Android Activity + SurfaceView surface.
class Window {
public:
    Window() = default;
    ~Window();

    bool create(const std::string& title, int width, int height);
    void destroy();

    // Pump OS events into Input. Returns false on SDL_QUIT.
    bool pollEvents(Input& input);
    void swap();

    SDL_Renderer* renderer() const { return renderer_; }
    int width() const { return width_; }
    int height() const { return height_; }

private:
    SDL_Window* window_ = nullptr;
    SDL_Renderer* renderer_ = nullptr;
    int width_ = 0;
    int height_ = 0;
};

} // namespace wmw::platform
