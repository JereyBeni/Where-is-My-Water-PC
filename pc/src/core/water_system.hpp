#pragma once

#include "core/interactive_object.hpp"
#include "core/particle_description.hpp"
#include "core/level_loader.hpp"

#include <string>
#include <vector>

namespace wmw::core {

// Partial WaterConcept::Fluids stand-in [STUB solver / IMPLEMENTED emission path].
// Holds debug particles only. No SPH / grid / original integration.
class WaterSystem {
public:
    bool init();
    void shutdown();

    // Build InteractiveObjects + Spouts from level + .hs (real file data).
    bool bindLevel(const std::string& assetsRoot, const LevelData& level);

    void update(float dt);

    const std::vector<InteractiveObject>& objects() const { return objects_; }
    const std::vector<ParticleDescription>& particles() const { return particles_; }
    int spoutCount() const { return spoutCount_; }
    int totalEmitted() const { return totalEmitted_; }

    void logSpouts() const;

private:
    std::vector<InteractiveObject> objects_;
    std::vector<ParticleDescription> particles_; // DEBUG store
    int spoutCount_ = 0;
    int totalEmitted_ = 0;
    bool bound_ = false;

    static constexpr size_t kMaxDebugParticles = 4000;
};

} // namespace wmw::core
