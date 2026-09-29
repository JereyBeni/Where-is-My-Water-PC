# Where's My Water? — PC Port Skeleton

This is the **starting point** of a faithful PC port.

The goal is to preserve as much of the original architecture, data formats and gameplay behaviour as possible.  
We are **not** rewriting the game from imagination.

---

## What is implemented right now

| System              | Status   | Notes |
|---------------------|----------|-------|
| Window / input loop | Done     | SDL2, resizable window, quit handling |
| Assets path check   | Done     | Looks for `Levels/`, `Sprites/`, `Textures/`, `Audio/`, `Data/` |
| Engine façade       | Done     | Clear init / update / render / shutdown |
| Physics             | **STUB** | Waiting for `libwmw.so` + format study |
| Water simulation    | **STUB** | Waiting for research |
| Level loader        | **STUB** | Original levels are XML + PNG in `assets/Levels/` |
| Gameplay            | **STUB** | — |
| Audio               | **STUB** | Original uses FMOD (`libfmodex.so`) |
| Real game renderer  | **STUB** | Original pipeline lives inside `libwmw.so` |

---

## What is deliberately NOT invented

- No fake water physics
- No fake level parser that “guesses” the XML
- No fake object system
- No claims that we already understand the binary engine

See root `AGENTS.md` for the full research rules.

---

## How to build (local)

### Dependencies
- CMake ≥ 3.16
- C++17 compiler
- SDL2 development package

Ubuntu/Debian:
```bash
sudo apt update
sudo apt install build-essential cmake libsdl2-dev
```

### Build
```bash
cd pc
mkdir build && cd build
cmake ..
cmake --build .
```

### Run
The executable expects an `assets/` folder next to it (or you pass the path):

```bash
# from repo root after packaging, or:
./wmw_pc ../../app/src/main/assets
```

---

## Asset layout expected by the executable

```
assets/
  Levels/
  Sprites/
  Textures/
  Audio/
  Data/
  Animations/
  ... (rest of original assets)
```

These come from `app/src/main/assets/` of this repository.  
They are **not** copied into `pc/src`.

---

## Original engine location (important)

| Component          | Location                          | PC relevance |
|--------------------|-----------------------------------|--------------|
| Real game engine   | `app/libs/arm64-v8a/libwmw.so`    | Closed binary — needs RE / reimplementation |
| Disney Java layer  | `app/libs/1.18.8.jar`             | Closed — Activity / View glue |
| Mod / ImGui layer  | `app/src/main/jni/` → `libmui`    | Not the game |
| Assets             | `app/src/main/assets/`            | **Reusable** |

---

## Next research targets (before writing real systems)

1. Level XML schema (`assets/Levels/*.xml`)
2. Object / emitter / skeleton formats
3. Strings and symbols inside `libwmw.so`
4. How the original renderer and water sim talked to each other
5. FMOD bank / sound layout

Only after that we start filling the stubs.

---

## GitHub Actions

The workflow builds this PC target on `ubuntu-latest` and uploads a ZIP artifact containing:

- `wmw_pc` binary
- `assets/` (copied from the original Android assets)

Download it from the Actions tab.
