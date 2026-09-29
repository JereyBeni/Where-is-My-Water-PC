#include "core/water_system.hpp"
#include "core/hs_loader.hpp"

#include <iostream>
#include <cstdlib>

namespace wmw::core {

bool WaterSystem::init() {
    std::cout << "[WaterSystem] emission path active; fluid solver still STUB\n";
    return true;
}

void WaterSystem::shutdown() {
    objects_.clear();
    particles_.clear();
    bound_ = false;
    spoutCount_ = 0;
    totalEmitted_ = 0;
}

bool WaterSystem::bindLevel(const std::string& assetsRoot, const LevelData& level) {
    objects_.clear();
    particles_.clear();
    spoutCount_ = 0;
    totalEmitted_ = 0;
    bound_ = false;

    HsLoader hsLoader;

    for (const auto& lo : level.objects) {
        InteractiveObject io;
        io.name = lo.name;
        io.worldPos = {lo.x, lo.y};
        io.worldAngleDeg = 0.f;
        {
            auto it = lo.properties.find("Angle");
            if (it != lo.properties.end()) {
                io.worldAngleDeg = std::strtof(it->second.c_str(), nullptr);
            }
        }

        const std::string filename = lo.get("Filename");
        if (!filename.empty()) {
            InteractiveObject fromHs;
            if (hsLoader.load(assetsRoot, filename, fromHs)) {
                io.hsPath = fromHs.hsPath;
                io.shapes = std::move(fromHs.shapes);
                // Merge: hs defaults first, level properties override [CONFIRMED authoring pattern]
                io.properties = fromHs.properties;
                for (const auto& kv : lo.properties) {
                    io.properties[kv.first] = kv.second;
                }
                if (fromHs.spout.has_value() || io.properties.count("SpoutType") ||
                    io.properties["Type"] == "spout") {
                    Spout spout;
                    spout.objectName = io.name;
                    spout.hsPath = io.hsPath;
                    spout.worldPos = io.worldPos;
                    spout.worldAngleDeg = io.worldAngleDeg;
                    spout.applyProperties(io.properties);
                    io.spout = spout;
                    ++spoutCount_;
                }
            } else {
                // Keep level props only
                io.properties = lo.properties;
            }
        } else {
            io.properties = lo.properties;
        }

        objects_.push_back(std::move(io));
    }

    bound_ = true;
    std::cout << "[WaterDebug]\n"
              << "  objects bound: " << objects_.size() << "\n"
              << "  spouts found: " << spoutCount_ << "\n";
    logSpouts();
    return true;
}

void WaterSystem::logSpouts() const {
    for (const auto& io : objects_) {
        if (io.spout.has_value()) {
            io.spout->log();
        }
    }
}

void WaterSystem::update(float dt) {
    if (!bound_) return;

    for (auto& io : objects_) {
        if (!io.spout.has_value()) continue;
        // Keep spout pose in sync
        io.spout->worldPos = io.worldPos;
        io.spout->worldAngleDeg = io.worldAngleDeg;

        auto spawned = io.spout->emit(dt);
        totalEmitted_ += static_cast<int>(spawned.size());

        for (auto& p : spawned) {
            if (particles_.size() >= kMaxDebugParticles) {
                // drop oldest
                particles_.erase(particles_.begin(), particles_.begin() + static_cast<long>(particles_.size() / 4));
            }
            particles_.push_back(p);
        }
    }

    // [DEBUG PARTICLE VISUALIZATION] — constant-velocity integration only.
    // NOT the original Fluids solver (algorithm UNKNOWN).
    for (auto& p : particles_) {
        p.position.x += p.velocity.x * dt;
        p.position.y += p.velocity.y * dt;
    }
}

} // namespace wmw::core
