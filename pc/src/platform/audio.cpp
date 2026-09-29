#include "platform/audio.hpp"

#include <iostream>

namespace wmw::platform {

bool Audio::init() {
    std::cout << "[Audio] STUB — original middleware is FMOD Ex (ARM64 libfmodex.so).\n";
    std::cout << "[Audio] Windows path requires FMOD Windows runtime + bank research.\n";
    initialized_ = true;
    return true;
}

void Audio::shutdown() {
    initialized_ = false;
}

void Audio::update() {
    // no-op stub
}

} // namespace wmw::platform
