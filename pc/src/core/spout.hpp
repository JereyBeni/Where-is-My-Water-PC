#pragma once

#include "core/particle_description.hpp"
#include "core/types.hpp"

#include <string>
#include <vector>
#include <unordered_map>

namespace wmw::core {

// WaterConcept::Spout — class name [CONFIRMED] in libwmw.so.
// Field set below mirrors [CONFIRMED] .hs DefaultProperties + level overrides.
// Binary member layout [UNKNOWN].
class Spout {
public:
    std::string objectName;
    std::string hsPath;

    SpoutType type = SpoutType::Unknown;
    FluidType fluidType = FluidType::Unknown;

    float particlesPerSecond = 0.f; // [CONFIRMED] ParticlesPerSecond
    float particleSpeed = 0.f;      // [CONFIRMED] ParticleSpeed
    int numberParticles = 0;        // [CONFIRMED] NumberParticles; -1 => infinite [INFERRED]

    Vec2 offsetToMouth;             // [CONFIRMED] OffsetToMouth "x y"
    float expulsionAngleDeg = 0.f;  // [CONFIRMED] ExpulsionAngle

    Vec2 worldPos;
    float worldAngleDeg = 0.f;

    bool infiniteParticles = false;

    bool isGoal = false;
    std::string goalPreset;

    void applyProperties(const std::unordered_map<std::string, std::string>& props);

    // [DEBUG] emission using file-backed rates; not Fluids solver
    std::vector<ParticleDescription> emit(float dt);

    Vec2 mouthWorld() const;
    void log() const;

private:
    float emitAccum_ = 0.f;
    int emittedTotal_ = 0;
};

} // namespace wmw::core
