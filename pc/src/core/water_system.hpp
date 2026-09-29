#pragma once

namespace wmw::core {

// [STUB] Water / fluid interface.
// Original particle/grid fluid is inside libwmw.so.
// Spout properties in Level XML (FluidType, ParticlesPerSecond, ...)
// are CONFIRMED data — simulation itself is UNKNOWN.
class WaterSystem {
public:
    bool init();
    void shutdown();
    void update(float dt);

    bool active() const { return false; }
};

} // namespace wmw::core
