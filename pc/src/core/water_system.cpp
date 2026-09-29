#include "core/water_system.hpp"

#include <iostream>

namespace wmw::core {

bool WaterSystem::init() {
    std::cout << "[WaterSystem] STUB — no fake fluid. Study particle/spout path in engine binary first.\n";
    return true;
}

void WaterSystem::shutdown() {}

void WaterSystem::update(float /*dt*/) {
    // intentionally empty
}

} // namespace wmw::core
