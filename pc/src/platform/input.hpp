#pragma once

union SDL_Event;

namespace wmw::platform {

// [REIMPLEMENTED] Pointer/keyboard state for PC.
// Original game used touch; mouse maps to dig/touch later.
class Input {
public:
    void beginFrame();
    void handleEvent(const SDL_Event& e);

    bool quitRequested() const { return quit_; }
    void setQuit(bool v) { quit_ = v; }

    bool mouseDown() const { return mouseDown_; }
    float mouseX() const { return mouseX_; }
    float mouseY() const { return mouseY_; }

    bool keyPressed(int scancode) const;

private:
    bool quit_ = false;
    bool mouseDown_ = false;
    float mouseX_ = 0.f;
    float mouseY_ = 0.f;
    // Simple key state for a few debug keys
    bool keys_[512]{};
};

} // namespace wmw::platform
