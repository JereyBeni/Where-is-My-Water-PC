# Phase 4 — ParticleDescription + Spout emission

## [CONFIRMED]

### libwmw.so
- Type `WaterConcept::ParticleDescription` used across Fluids / Spout / InteractiveObject.
- `WaterConcept::Spout` with `SpoutType`, `addParticles`, `setInfiniteParticles`.
- `InteractiveObject::particleHasCollided(Fluids*, ParticleDescription const&, int, bool&)`.
- Tunable name strings: `CollisionFrictionFluid`, `CollisionElasticityFluid`, `FluidsCollide`, `AllowedFluid`, `IgnoreFluid`.

### .hs / level XML
- Spout-related properties: `SpoutType`, `FluidType`, `ParticlesPerSecond`, `ParticleSpeed`, `NumberParticles`, `OffsetToMouth`, `ExpulsionAngle`.
- `SpoutType` values seen: `OpenSpout`, `TouchSpout`, `DrainSpout`.
- `FluidType` value seen in spout hs: `Water`.
- Level places objects with `AbsoluteLocation`, `Angle`, `Filename` → `/Objects/*.hs`.

### water.db
- Not used for particle emission (catalog only) — see Phase 2.

---

## [INFERRED]

- `NumberParticles < 0` means infinite emission (matches `setInfiniteParticles`).
- `DrainSpout` does not continuously emit; it is a goal/collector (`Goal` / `GoalPreset` on broken_pipe.hs).
- Particle needs world **position** and **velocity** for any emission model driven by `ParticleSpeed` + angles.
- Mouth world position ≈ object position + rotated `OffsetToMouth`.
- Emission direction ≈ `Angle` (level) + `ExpulsionAngle` (.hs).

---

## [UNKNOWN]

- Binary layout / size of `ParticleDescription`.
- Fields: mass, radius, lifetime, temperature, damping coefficients.
- Exact `Spout::addParticles` parameter meanings beyond the mangled signature fragment.
- Original integration / neighbor / pressure solver inside `Fluids`.
- Whether TouchSpout requires player input before emitting (gameplay rule not fully proven from assets alone).
- Precise collision response for `particleHasCollided` (friction/elasticity application).

---

## [IMPLEMENTED] (Windows x64 port)

| Piece | Location | Notes |
|-------|----------|-------|
| `ParticleDescription` | `pc/src/core/particle_description.hpp` | pos, vel, fluid only |
| `Spout` | `pc/src/core/spout.*` | props from .hs + level |
| `HsLoader` | `pc/src/core/hs_loader.*` | XML InteractiveObject |
| `InteractiveObject` | `pc/src/core/interactive_object.hpp` | shapes + optional Spout |
| Emission | `Spout::emit` + `WaterSystem::update` | real pps/speed from files |
| Debug dots | `Engine::render` | **[DEBUG PARTICLE VISUALIZATION]** |
| Console dump | `Spout::log` / `WaterDebug` | per-spout props |

---

## [STUB]

- Full `WaterConcept::Fluids` solver
- `particleHasCollided` implementation
- PhysicsSystem / dig terrain
- FMOD audio
- Original fluid rendering (metaballs / RT from binary strings)

---

## Tests (manual / runtime)

| ID | Check |
|----|--------|
| A | HsLoader reads `shower_head.hs` |
| B | Props map into Spout (pps, speed, …) |
| C | `emit(dt)` produces particles when pps > 0 |
| D | Load `01_CUT_FIRST` |
| E | Log reports 4 spout-class objects (emitters + drains) |
| F | Console `[WaterDebug] Spouts found: N` + emitted count on exit |

Run:
```text
wmw_pc.exe assets 01_CUT_FIRST
```
