#pragma once

#include <cstdint>

namespace wmw::platform {

// [REIMPLEMENTED] High-resolution frame timer (SDL).
class Timer {
public:
    Timer();

    // Call once per frame. Returns delta seconds since previous tick.
    float tick();

    double totalSeconds() const { return totalSec_; }

private:
    std::uint64_t lastTicks_ = 0;
    double totalSec_ = 0.0;
};

} // namespace wmw::platform
