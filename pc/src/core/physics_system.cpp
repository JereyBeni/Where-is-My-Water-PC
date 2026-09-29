#include "core/physics_system.hpp"

#include <iostream>

namespace wmw::core {

bool PhysicsSystem::init() {
    std::cout << "[PhysicsSystem] STUB — awaiting RE of libwmw.so / object .hs behaviour.\n";
    return true;
}

void PhysicsSystem::shutdown() {}

void PhysicsSystem::update(float /*dt*/) {
    // intentionally empty
}

} // namespace wmw::core
