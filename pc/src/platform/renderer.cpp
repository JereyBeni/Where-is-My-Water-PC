#include "platform/renderer.hpp"

#include <SDL.h>

namespace wmw::platform {

void Renderer::begin(SDL_Renderer* sdl, int screenW, int screenH) {
    sdl_ = sdl;
    sw_ = screenW;
    sh_ = screenH;
}

void Renderer::clear(Color c) {
    if (!sdl_) return;
    SDL_SetRenderDrawColor(sdl_, c.r, c.g, c.b, c.a);
    SDL_RenderClear(sdl_);
}

void Renderer::setCamera(float cx, float cy, float zoom) {
    camX_ = cx;
    camY_ = cy;
    zoom_ = zoom;
}

void Renderer::worldToScreen(float wx, float wy, float& sx, float& sy) const {
    // Level coords appear centered-ish; Y grows up in data → flip for screen
    sx = (wx - camX_) * zoom_ + sw_ * 0.5f;
    sy = (camY_ - wy) * zoom_ + sh_ * 0.5f;
}

void Renderer::drawPoint(float x, float y, Color c, int size) {
    if (!sdl_) return;
    float sx, sy;
    worldToScreen(x, y, sx, sy);
    SDL_SetRenderDrawColor(sdl_, c.r, c.g, c.b, c.a);
    SDL_Rect r{
        static_cast<int>(sx) - size / 2,
        static_cast<int>(sy) - size / 2,
        size, size
    };
    SDL_RenderFillRect(sdl_, &r);
}

void Renderer::drawLine(float x0, float y0, float x1, float y1, Color c) {
    if (!sdl_) return;
    float sx0, sy0, sx1, sy1;
    worldToScreen(x0, y0, sx0, sy0);
    worldToScreen(x1, y1, sx1, sy1);
    SDL_SetRenderDrawColor(sdl_, c.r, c.g, c.b, c.a);
    SDL_RenderDrawLine(sdl_,
        static_cast<int>(sx0), static_cast<int>(sy0),
        static_cast<int>(sx1), static_cast<int>(sy1));
}

void Renderer::drawRect(float x, float y, float w, float h, Color c, bool filled) {
    if (!sdl_) return;
    float sx, sy;
    worldToScreen(x, y, sx, sy);
    SDL_SetRenderDrawColor(sdl_, c.r, c.g, c.b, c.a);
    SDL_Rect r{
        static_cast<int>(sx),
        static_cast<int>(sy),
        static_cast<int>(w * zoom_),
        static_cast<int>(h * zoom_)
    };
    if (filled) SDL_RenderFillRect(sdl_, &r);
    else SDL_RenderDrawRect(sdl_, &r);
}

void Renderer::end() {
    // Present is done by Window::swap
    sdl_ = nullptr;
}

} // namespace wmw::platform
