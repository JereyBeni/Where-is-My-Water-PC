#pragma once

#include <string>

namespace wmw::platform {

// [STUB] Audio backend.
// Original game uses FMOD Ex (libfmodex.so on ARM64).
// Do NOT implement fake beeps as "game audio".
// Research targets: asset banks under assets/Audio/, FMOD version, bank format.
class Audio {
public:
    bool init();
    void shutdown();

    // Future: playCue(name), setListener, etc.
    void update();

    bool available() const { return false; } // not wired yet

private:
    bool initialized_ = false;
};

} // namespace wmw::platform
