#pragma once

#include <string>

namespace wmw::core {

// High-level engine façade.
// Real systems (Physics, Water, Levels, Gameplay) are intentionally stubs.
// Do NOT invent implementations until libwmw.so and asset formats are studied.

class Engine {
public:
    bool init(const std::string& assetsRoot);
    void update(float dt);
    void render();
    void shutdown();

private:
    std::string assetsRoot_;
    bool initialized_ = false;

    // --- Stubs for future systems (intentionally empty) ---
    // void physics_;
    // void water_;
    // void levels_;
    // void gameplay_;
    // void audio_;   // original used FMOD
    // void renderer_; // original used custom GLES pipeline inside libwmw.so
};

} // namespace wmw::core
