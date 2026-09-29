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
| JNI (`System.loadLibrary`, native methods) | Android Java ↔ C | Replaced by pure C++ interfaces |
| `WMWActivity` / `BaseActivity` | Android Activity lifecycle | Outside Windows core |
| `SurfaceView` / `mySurfaceView` | Android graphics surface | Replaced by SDL2 Window |
| Permissions / storage / OBB | Android APIs | Replaced by Win32 / SDL filesystem |
| Google Play Billing / IAP JARs | Android + Google | **NOT RELEVANT** for offline PC core |
| ImGui / Platinmods in `jni/` | Mod menu for Android | **NOT RELEVANT** to game core |

---

## 3. Original call chain (Android) [CONFIRMED]

```
MainActivity (launcher UI)
    → WMWActivity (extends BaseActivity from JAR)
        → WMWView (from JAR)
            → native surface / libwmw.so
        → FMODAudioDevice
        → loadLibrary("mui")  // mod layer only
```

Game logic, water sim, level runtime and primary renderer live **inside `libwmw.so`**, not in the Java sources present in the tree.

---

## 4. Windows target architecture

```
                    wmw_pc.exe  (Windows x64 PE)
                           │
              ┌────────────┴────────────┐
              │                         │
         Game Core                  Platform Layer
              │                         │
   ┌──────────┼──────────┐         SDL2 + Win32
   │          │          │              │
 Levels   Physics    Water         Window
 Loader   System    System         Input
   │                               FileSystem
 Assets                            Timer
 (external)                        Audio (stub)
                                   Renderer (SDL2)
```

- **No JNI**
- **No ARM code paths**
- **No loading of `.so` files**

---

## 5. Level data format [CONFIRMED]

Sample: `assets/Levels/01_CUT_FIRST.xml`, `02_TOUCH_FOR_WATER.xml`

```xml
<Objects>
  <Object name="...">
    <AbsoluteLocation value="x y"/>
    <Properties>
      <Property name="Angle" value="..."/>
      <Property name="Filename" value="/Objects/....hs"/>
      <Property name="Type" value="spout|star|switch|yswitch|..."/>
      ...
    </Properties>
  </Object>
  <Room>
    <AbsoluteLocation value="x y"/>
  </Room>
  <Properties> ... optional level-wide ... </Properties>
</Objects>
```

Also present:
- `assets/Data/water.db` (SQLite — progress / level DB) [CONFIRMED file exists; schema UNKNOWN until opened]
- Object definitions under `assets/Objects/*.hs` [format partially UNKNOWN]

---

## 6. System status legend

| Tag | Meaning |
|-----|---------|
| **[CONFIRMED]** | Directly observed in repo |
| **[REIMPLEMENTED]** | Behaviour recreated from confirmed data/formats |
| **[INFERRED]** | Reasonable from naming/structure, not proven |
| **[STUB]** | Interface only; no claim of original behaviour |
| **[UNKNOWN]** | Insufficient evidence in repo |

---

## 7. Per-system status

| System | Status | Evidence / notes |
|--------|--------|------------------|
| Window | REIMPLEMENTED | SDL2 on Windows x64 |
| Input | REIMPLEMENTED | SDL2 events (mouse/keyboard as dig input later) |
| FileSystem | REIMPLEMENTED | Portable paths, no Android storage |
| Timer | REIMPLEMENTED | SDL_GetTicks64 / high-res |
| Renderer (clear + debug draw) | REIMPLEMENTED | SDL2 renderer; not original GLES pipeline |
| Level XML parse | REIMPLEMENTED | Real XML schema from assets |
| Level runtime / simulation | STUB | Lived in libwmw.so |
| PhysicsSystem | STUB | Interface only |
| WaterSystem | STUB | Interface only; no fake fluid |
| Audio | STUB | Original = FMOD; banks/layout need study |
| Object `.hs` format | UNKNOWN | Referenced by Filename property |
| water.db schema | UNKNOWN | File present |
| Full original renderer | UNKNOWN | Inside libwmw.so |

---

## 8. Endianness / sizes / ARM specifics

| Topic | Finding |
|-------|---------|
| ARM NEON / assembly in **source** | None in `jni/` game path (only ImGui/mod) |
| Binary `libwmw.so` | May contain NEON; **not executed on PC** |
| Level XML | Text ASCII — endian-safe |
| `sizeof(long)` Android vs Windows | Avoid relying on `long`; use fixed-width types (`int32_t`, `int64_t`) |
| Pointer size | Android arm64 = 8 bytes; Windows x64 = 8 bytes — OK if no 32-bit assumptions |

Any future binary struct parsers must be written with explicit layout and tested; none exist yet.

---

## 9. What this port will never do

- Load or translate `libwmw.so` / `libfmodex.so` as Windows DLLs
- Keep JNI in the Windows binary
- Ship Android Activity / SurfaceView / permission code in the core
- Pretend a placeholder fluid is "the original water sim"
