# WMW PC Port — Architecture & Binary Analysis

Last updated with repository evidence only.

---

## 1. Binary inventory [CONFIRMED]

| File | Path | Arch (from path / type) | Role |
|------|------|-------------------------|------|
| `libwmw.so` | `app/libs/arm64-v8a/` | **ARM64** (AArch64 ELF shared) | Closed game engine (physics, water, render, gameplay) |
| `libfmodex.so` | `app/libs/arm64-v8a/` | **ARM64** | FMOD Ex audio runtime |
| `1.18.8.jar` | `app/libs/` | JVM bytecode | Disney Java layer (BaseActivity, WMWView, etc.) |
| `内购.jar` | `app/libs/` | JVM bytecode | IAP helper |
| `libmui` (built) | `jni/` → shared lib | ARM64 when built for Android | ImGui + Platinmods **modding overlay only** |

**No x86 / x64 / ARM32 game binaries exist in this repository.**

There is nothing to "rename to .dll". The real engine is ARM64-only closed source.

---

## 2. What depends on ARM / Android [CONFIRMED]

| Component | Dependency | PC action |
|-----------|------------|-----------|
| `libwmw.so` | ARM64 ELF + Android NDK ABIs | **Do not load.** Reimplement behaviour from assets + RE |
| `libfmodex.so` | ARM64 FMOD | Replace with Windows FMOD or alternative after research |
| JNI | Android Java ↔ C | Pure C++ interfaces on PC |
| Activity / SurfaceView | Android | SDL2 Window |
| IAP / Play services | Google | Not in offline core |

---

## 3. Original call chain (Android) [CONFIRMED]

```
MainActivity → WMWActivity → WMWView → libwmw.so
                     └→ FMODAudioDevice
                     └→ libmui (mod only)
```

---

## 4. Windows target architecture

```
wmw_pc.exe (Windows x64 PE)
  Game Core: LevelLoader | PhysicsSystem[STUB] | WaterSystem[STUB]
  Platform:  SDL2 Window/Input/FS/Timer/Renderer | Audio[STUB]
```

---

## 5. Level data format [CONFIRMED]

Level XML under `assets/Levels/*.xml`: `<Objects>` / `<Object>` / `<AbsoluteLocation>` /
`<Property>` / `<Room>`. Property `Filename` points at `/Objects/*.hs`.

---

## Reverse Engineering: Water and Physics

### water.db [CONFIRMED]

| Field | Value |
|-------|--------|
| Path | `assets/Data/water.db` |
| Size | 221184 bytes |
| Magic | `SQLite format 3\0` |
| Role | **Progress / catalog / meta** — NOT fluid simulation |

**Tables (evidence via Python sqlite3):**  
`LevelInfo` (671 rows), `LevelPackInfo` (46), `Achievements`, `Settings`
(`DatabaseVersion=23`), `PlayerData`, `CollectibleInfo`, `HubInfo`, `IAPInfo`,
challenge tables (Allie/Cranky/Mystery), etc.

**LevelInfo columns [CONFIRMED]:**  
`ID, Name, Filename, Stars, PackName, TimesPlayed, TimesFinished, Unlocked,
ParTime, BestScore, CollectibleFound, PlayTime, ...`

Example: `LN_CUT_FIRST` → `Filename=/Levels/01_CUT_FIRST` → pack `LP_CRANKY_FOUR`.

**Migration scripts:** `assets/Data/UpdateScripts/db_update_*.sql` mutate this schema
over versions 1→23.

**Also:** `water-Lite.db`, `water-demo.db` — same family [CONFIRMED files].

**What water.db is NOT [CONFIRMED by schema search]:**  
No tables/columns for particles, fluid grids, collision meshes, or spout runtime state.

Tool: `pc/tools/analyze_water_db.py` (read-only).

---

### `.hs` object definitions [CONFIRMED]

| Field | Value |
|-------|--------|
| Count | 345 under `assets/Objects/` |
| Format | **XML text** (not binary) |
| Root | `<InteractiveObject>` |

**Structure [CONFIRMED from multiple files]:**

```xml
<InteractiveObject>
  <Shapes>
    <Shape>
      <Point pos="x y" />  <!-- polygon in local object space -->
      ...
    </Shape>
  </Shapes>
  <Sprites>
    <Sprite filename="/Sprites/....sprite" pos="x y" angle="..." gridSize="w h" />
  </Sprites>
  <DefaultProperties>
    <Property name="Type" value="spout|star|yswitch|..." />
    <!-- type-specific defaults -->
  </DefaultProperties>
</InteractiveObject>
```

**Property evidence (samples):**

| Property | Example | Meaning [CONFIRMED text / INFERRED use] |
|----------|---------|----------------------------------------|
| `Type` | `spout`, `star`, `yswitch` | Gameplay class |
| `SpoutType` | `OpenSpout`, `TouchSpout`, `DrainSpout` | Spout behaviour class [CONFIRMED strings in .hs] |
| `FluidType` | `Water` | Fluid enum name [CONFIRMED in .hs; full enum in binary INFERRED] |
| `ParticlesPerSecond` | `5`, `8`, `15` | Emit rate [CONFIRMED] |
| `ParticleSpeed` | `6`–`50` | Emit speed [CONFIRMED] |
| `NumberParticles` | `-1` or `0` | Cap / infinite [CONFIRMED]; binary has `setInfiniteParticles` [CONFIRMED symbol] |
| `OffsetToMouth` | `1.8 0` | Emit origin offset [CONFIRMED] |
| `ExpulsionAngle` | `0` | Emit direction [CONFIRMED] |
| `Goal` / `GoalPreset` | `1` / `Swampy` | Drain is level goal [CONFIRMED in broken_pipe.hs] |
| `PlatinumType` | `normal` | Star variant [CONFIRMED] |

**Shapes:** convex/concave polygons in local coordinates — used as object geometry for
interaction with fluid [INFERRED from `InteractiveObject::particleHasCollided` +
shape presence; exact collision algorithm UNKNOWN].

Tool: `pc/tools/analyze_hs.py` (read-only).

**Related (not .hs):** `assets/Emitters/*.emitter` — XML **visual** particle FX
(`Walaber::ParticleEmitter` style), separate from `WaterConcept::Fluids` gameplay fluid
[CONFIRMED XML; relationship to Fluids = INFERRED separate systems].

---

### libwmw.so [CONFIRMED]

| Field | Value |
|-------|--------|
| Path | `app/libs/arm64-v8a/libwmw.so` |
| Size | 7451432 bytes |
| ELF | `\x7fELF`, class 2 (64-bit), `e_machine=183` (AArch64) |
| Policy | **Never execute / never load on Windows x64** |

**Engine branding [CONFIRMED symbols/strings]:**
- Namespace `WaterConcept`
- Middleware/math `Walaber` (`Vector2`, `SpriteBatch`, `ParticleEmitter`, …)
- Class `WaterConcept::Fluids` with render texture buffers
- `WaterConcept::ParticleDescription`
- `WaterConcept::Spout` (+ `SpoutType`, `setInfiniteParticles`, `addParticles`)
- `WaterConcept::InteractiveObject::particleHasCollided(Fluids*, ParticleDescription const&, ...)`
- Also: `StarSeed`, `Switch`, `YSwitch`, `Fan`, `Bomb`, `WaterBalloon`, `FluidConverter`,
  `Floater`, `DirtyWall`, `MysteryCave`, `IcyHot`, `AlgaeHider`, `World`, …
- UI/data: `PlayerDataSerializer` ↔ `LevelInfo` / `LevelPackInfo` (ties to water.db)
- Debug/test: `Screen_WaterTest` with `_screenToWorld` / `_worldToScreen`

**Strings tying assets to binary [CONFIRMED]:**
- Paths `/Objects/star.hs`, `/Objects/balloon.hs`, `/Levels/...`
- `/checked_water_tmp.db` (runtime copy of progress DB [INFERRED])
- Fluid material names in textures: Water, Mud, Steam, Poison, Ooze, …
- Tunables: `CollisionFrictionFluid`, `CollisionElasticityFluid`, `FluidsCollide`,
  `AllowedFluid`, `IgnoreFluid`, `StartingFluidType`

**Fluid simulation algorithm [UNKNOWN]:**  
Particle-based interaction is confirmed by API surface; whether SPH, grid, custom
Walaber solver, neighbor sets (`_addParticleAndNeighbors`), etc. is **not** fully
determined from strings alone.

**Terrain dig model [UNKNOWN]:**  
No clear `Terrain`/`Diggable` symbols found in string scan; dig may be image-mask or
other subsystem not named that way [UNKNOWN].

---

### Relationship chain (evidence-based)

```
water.db (SQLite catalog)
    └─ LevelInfo.Filename → "/Levels/01_CUT_FIRST"
              ↓
Level XML (placement, overrides)
    └─ Object.Filename → "/Objects/shower_head.hs"
              ↓
.hs InteractiveObject (default Type, SpoutType, FluidType, Shape polygon, sprites)
              ↓
libwmw.so WaterConcept::World / Fluids / Spout / InteractiveObject
    └─ particleHasCollided, addParticles, FluidType conversions
              ↓
[STUB on PC] WaterSystem + PhysicsSystem  ← reimplement later from this evidence
```

**C++-shaped targets for a future port (names from binary — not yet implemented):**

```text
enum class FluidType { ... };          // Water, Mud, Steam, Poison, ... [INFERRED set]
struct ParticleDescription;            // [CONFIRMED type name]
class Fluids;                          // [CONFIRMED]
class Spout;                           // SpoutType [CONFIRMED]
class InteractiveObject;               // Shape + particleHasCollided [CONFIRMED]
```

---

## 6. System status legend

| Tag | Meaning |
|-----|---------|
| **[CONFIRMED]** | Directly observed |
| **[REIMPLEMENTED]** | Recreated from confirmed data |
| **[INFERRED]** | Reasonable, not proven |
| **[STUB]** | Interface only |
| **[UNKNOWN]** | Insufficient evidence |

---

## 7. Per-system status

| System | Status | Notes |
|--------|--------|-------|
| Window / Input / FS / Timer | REIMPLEMENTED | SDL2 |
| Level XML loader | REIMPLEMENTED | |
| Debug object dots | REIMPLEMENTED | |
| `.hs` loader | **not yet** — format CONFIRMED XML |
| water.db reader | **not yet** — schema CONFIRMED SQLite |
| PhysicsSystem | STUB | |
| WaterSystem | STUB | |
| Audio FMOD | STUB | |
| Original GLES fluid renderer | UNKNOWN inside libwmw.so |

---

## 8. Tools

| Tool | Path | Env |
|------|------|-----|
| water.db analyzer | `pc/tools/analyze_water_db.py` | Python 3, read-only |
| .hs analyzer | `pc/tools/analyze_hs.py` | Python 3, read-only |

```bash
python pc/tools/analyze_water_db.py app/src/main/assets/Data/water.db
python pc/tools/analyze_hs.py app/src/main/assets/Objects
```

---

## 9. What this port will never do

- Load `libwmw.so` / `libfmodex.so` as Windows code
- JNI in the Windows binary
- Fake fluid sold as original water sim
