#pragma once

namespace wmw::core {

// [STUB] Physics interface.
// Original rigid/soft body + terrain interaction lived in libwmw.so.
// Do not plug in a random engine and call it "original physics".
class PhysicsSystem {
public:
    bool init();
    void shutdown();
    void update(float dt);

    bool active() const { return false; }
};

} // namespace wmw::core
