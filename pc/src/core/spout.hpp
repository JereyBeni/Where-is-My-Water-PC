#pragma once

#include "core/particle_description.hpp"
#include "core/types.hpp"

#include <string>
#include <vector>

namespace wmw::core {

// WaterConcept::Spout — class name [CONFIRMED] in libwmw.so.
// Field set below mirrors [CONFIRMED] .hs DefaultProperties + level overrides.
// Binary member layout [UNKNOWN].
class Spout {
public:
    // Identity / debug
    std::string objectName;
    std::string hsPath;

    SpoutType type = SpoutType::Unknown;
    FluidType fluidType = FluidType::Unknown;

    float particlesPerSecond = 0.f; // [CONFIRMED] ParticlesPerSecond
    float particleSpeed = 0.f;      // [CONFIRMED] ParticleSpeed
    int numberParticles = 0;        // [CONFIRMED] NumberParticles; -1 => infinite [INFERRED + setInfiniteParticles symbol]

    Vec2 offsetToMouth;             // [CONFIRMED] OffsetToMouth "x y"
    float expulsionAngleDeg = 0.f;  // [CONFIRMED] ExpulsionAngle (degrees in assets)

    // World pose from level XML AbsoluteLocation + Angle [CONFIRMED level fields]
    Vec2 worldPos;
    float worldAngleDeg = 0.f;

    bool infiniteParticles = false; // [INFERRED] from NumberParticles < 0 + setInfiniteParticles

    // Drain goal markers from .hs [CONFIRMED when present]
    bool isGoal = false;
    std::string goalPreset;

    // Fill from merged property map (hs defaults + level overrides).
    void applyProperties(const std::unordered_map<std::string, std::string>& props);

    // Emit based on real pps/speed/offsets. Returns newly spawned particles.
    // Motion after emission is DEBUG only until Fluids solver is RE'd.
    std::vector<ParticleDescription> emit(float dt);

    // Mouth position in world space (object pos + rotated offset) [INFERRED math]
    Vec2 mouthWorld() const;

    void log() const;

private:
    float emitAccum_ = 0.f;
    int emittedTotal_ = 0;
};

} // namespace wmw::core

// Need full map include for applyProperties signature in header consumers
#include <unordered_map>
