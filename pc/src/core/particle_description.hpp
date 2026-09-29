#pragma once

#include "core/types.hpp"

namespace wmw::core {

// WaterConcept::ParticleDescription — type name [CONFIRMED] in libwmw.so symbols.
// Binary memory layout is [UNKNOWN]. We only store fields required to express
// emission data that is [CONFIRMED] or strongly [INFERRED] from .hs + symbol use.
//
// Symbol evidence (libwmw.so):
//   Fluids::removeParticle(ParticleDescription const&)
//   Fluids::changeParticleToFluidType(..., ParticleDescription const&)
//   Spout::addParticles(ParticleDescription const&, ...)
//   InteractiveObject::particleHasCollided(Fluids*, ParticleDescription const&, int, bool&)
//
// Therefore a particle carries at least identity for collision + fluid typing.
// Position/velocity are [INFERRED] as necessary for emission from
// ParticleSpeed / OffsetToMouth / ExpulsionAngle / object AbsoluteLocation.
struct ParticleDescription {
    Vec2 position;   // [INFERRED] world-space; required for emission + collision tests
    Vec2 velocity;   // [INFERRED] from ParticleSpeed + angles; solver integration UNKNOWN
    FluidType fluid = FluidType::Unknown; // [CONFIRMED] concept via FluidType props + changeParticleToFluidType

    // Explicitly not claimed from binary layout:
    // mass, radius, lifetime, temperature, damping — [UNKNOWN]
};

} // namespace wmw::core
