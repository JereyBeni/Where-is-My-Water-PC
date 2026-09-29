#pragma once

#include "core/spout.hpp"
#include "core/types.hpp"

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>

namespace wmw::core {

// WaterConcept::InteractiveObject — class name [CONFIRMED].
// particleHasCollided(Fluids*, ParticleDescription const&, int, bool&) [CONFIRMED symbol].
// Shape polygons from .hs [CONFIRMED]. Collision response algorithm [UNKNOWN] — not implemented.
struct InteractiveObject {
    std::string name;
    std::string hsPath;
    Vec2 worldPos;
    float worldAngleDeg = 0.f;

    // Local-space polygon(s) from .hs <Shapes> [CONFIRMED]
    std::vector<std::vector<Vec2>> shapes;

    std::unordered_map<std::string, std::string> properties;

    std::optional<Spout> spout; // present when Type/SpoutType indicates spout

    std::string typeName() const {
        auto it = properties.find("Type");
        return it != properties.end() ? it->second : std::string();
    }
};

} // namespace wmw::core
