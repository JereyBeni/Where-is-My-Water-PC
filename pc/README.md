# Where's My Water? — Windows x64 PC Port

Native **Windows x64 (PE)** port skeleton.  
No ARM code, no JNI, no loading of `libwmw.so`.

Read **[ARCHITECTURE.md](ARCHITECTURE.md)** for binary analysis and status tags.

---

## Content scope: Swampy + optimized shared assets

The **PC ZIP** is size-optimized. Full Android tree in git is unchanged.

### Character strip (`tools/strip_allie_cranky.ps1`)
| Kept | Removed from package |
|------|----------------------|
| Swampy | Allie (~550 files), Cranky (~335 files) |
| Shared Objects (even if name contains "cranky") | DLC UI Data / upsell / character music |

### Shared optimize (`tools/optimize_shared_assets.ps1`)
| Action | Evidence / note |
|--------|-----------------|
| Delete `Textures/*-HD.*` when non-HD pair exists | **~23 MB**, 478 safe pairs [CONFIRMED inventory] |
| Delete localized texture variants (`-ru`, `-zh-hans`, …) | Default/un-suffixed kept |
| Strip Mystery Duck packs | Same policy as Allie/Cranky |
| Delete `*.psd`, `*.bak` | Editor leftovers |
| Delete `offline.html`, `wmw-extra.zip` | Not needed by PC exe |

Local:
```powershell
.\tools\strip_allie_cranky.ps1 -AssetsRoot .\assets
.\tools\optimize_shared_assets.ps1 -AssetsRoot .\assets
# preview:
.\tools\optimize_shared_assets.ps1 -AssetsRoot .\assets -WhatIf
```

Raw assets in-repo ≈ **82 MB** / 4025 files. Package is substantially smaller after both scripts.

---

## Build (Windows)

### Requirements
- VS 2019/2022 C++ workload or Build Tools
- CMake ≥ 3.16
- SDL2 via vcpkg: `vcpkg install sdl2:x64-windows`

```powershell
cd pc
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=[vcpkg]\scripts\buildsystems\vcpkg.cmake
cmake --build build
```

Output: `build\wmw_pc.exe` (x64 PE).

---

## Run

```powershell
.\wmw_pc.exe .\assets 01_CUT_FIRST
```

**Esc** = quit.

---

## Status

| Feature | Tag |
|---------|-----|
| Windows x64 window + loop | REIMPLEMENTED |
| External assets discovery | REIMPLEMENTED |
| Level XML parse + debug draw | REIMPLEMENTED |
| Physics / Water / FMOD | STUB |
| Original GLES renderer | UNKNOWN (libwmw.so) |

---

## CI artifact

```
wmw-pc-windows-x64.zip
  wmw_pc.exe
  SDL2.dll
  assets\     (Swampy-only + shared optimize)
  run.bat
  README.md
  ARCHITECTURE.md
```
