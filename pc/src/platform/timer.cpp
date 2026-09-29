#include "platform/timer.hpp"

#include <SDL.h>

namespace wmw::platform {

Timer::Timer() {
    lastTicks_ = SDL_GetPerformanceCounter();
}

float Timer::tick() {
    const std::uint64_t now = SDL_GetPerformanceCounter();
    const std::uint64_t freq = SDL_GetPerformanceFrequency();
    double dt = 0.0;
    if (freq > 0 && now > lastTicks_) {
        dt = static_cast<double>(now - lastTicks_) / static_cast<double>(freq);
    }
    lastTicks_ = now;
    // Clamp absurd hitches
    if (dt > 0.1) dt = 0.1;
    totalSec_ += dt;
    return static_cast<float>(dt);
}

} // namespace wmw::platform
