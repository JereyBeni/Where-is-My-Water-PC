#include "platform/input.hpp"

#include <SDL.h>

namespace wmw::platform {

void Input::beginFrame() {
    // per-frame edge flags could go here later
}

void Input::handleEvent(const SDL_Event& e) {
    switch (e.type) {
    case SDL_MOUSEBUTTONDOWN:
        if (e.button.button == SDL_BUTTON_LEFT) {
            mouseDown_ = true;
            mouseX_ = static_cast<float>(e.button.x);
            mouseY_ = static_cast<float>(e.button.y);
        }
        break;
    case SDL_MOUSEBUTTONUP:
        if (e.button.button == SDL_BUTTON_LEFT) {
            mouseDown_ = false;
            mouseX_ = static_cast<float>(e.button.x);
            mouseY_ = static_cast<float>(e.button.y);
        }
        break;
    case SDL_MOUSEMOTION:
        mouseX_ = static_cast<float>(e.motion.x);
        mouseY_ = static_cast<float>(e.motion.y);
        if (e.motion.state & SDL_BUTTON_LMASK) {
            mouseDown_ = true;
        }
        break;
    case SDL_KEYDOWN:
        if (e.key.keysym.scancode < 512) {
            keys_[e.key.keysym.scancode] = true;
        }
        if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
            quit_ = true;
        }
        break;
    case SDL_KEYUP:
        if (e.key.keysym.scancode < 512) {
            keys_[e.key.keysym.scancode] = false;
        }
        break;
    default:
        break;
    }
}

bool Input::keyPressed(int scancode) const {
    if (scancode < 0 || scancode >= 512) return false;
    return keys_[scancode];
}

} // namespace wmw::platform
