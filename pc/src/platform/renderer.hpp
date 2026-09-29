#pragma once

#include <string>
#include <vector>

struct SDL_Renderer;

namespace wmw::platform {

struct Color {
    std::uint8_t r = 255, g = 255, b = 255, a = 255;
};

struct DebugPoint {
    float x = 0, y = 0;
    Color color{};
    std::string label;
};

// [REIMPLEMENTED] Minimal 2D debug renderer on SDL2.
// NOT the original GLES pipeline from libwmw.so.
// Pipeline target for later:
//   SDL2 → Window → Renderer → Texture → Sprite → Level
class Renderer {
public:
    void begin(SDL_Renderer* sdl, int screenW, int screenH);
    void clear(Color c);
    void drawPoint(float x, float y, Color c, int size = 4);
    void drawLine(float x0, float y0, float x1, float y1, Color c);
    void drawRect(float x, float y, float w, float h, Color c, bool filled = false);
    void end();

    // World ↔ screen helpers (simple ortho for level debug view)
    void setCamera(float cx, float cy, float zoom);
    void worldToScreen(float wx, float wy, float& sx, float& sy) const;

private:
    SDL_Renderer* sdl_ = nullptr;
    int sw_ = 0, sh_ = 0;
    float camX_ = 0, camY_ = 0, zoom_ = 8.f;
};

} // namespace wmw::platform
