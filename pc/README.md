# Where's My Water? — Windows x64 PC Port

Native **Windows x64 (PE)** port skeleton.  
No ARM code, no JNI, no loading of `libwmw.so`.

Read **[ARCHITECTURE.md](ARCHITECTURE.md)** for binary analysis and status tags.

---

## Content scope: Swampy-only

For size/optimization the **PC ZIP artifact** ships **Swampy only**.

| Kept | Removed from PC package |
|------|-------------------------|
| Swampy animations / audio / levels | Allie packs (~550 files) |
| Shared Objects (incl. names with "cranky" used by Swampy levels) | Cranky packs (~335 files) |
| Core Data / Sprites / Textures (shared) | Allie/Cranky UI Data XML, upsell art, DLC music |

Local strip script (does not touch git history):

```powershell
.\tools\strip_allie_cranky.ps1 -AssetsRoot .\assets
.\tools\strip_allie_cranky.ps1 -AssetsRoot .\app\src\main\assets -WhatIf
```

The full Android tree in the repo can still contain Allie/Cranky for research.

---

## Build (Windows)

### Requirements
- Visual Studio 2019/2022 with C++ desktop workload **or** Build Tools
- CMake ≥ 3.16
- SDL2 (recommended: [vcpkg](https://vcpkg.io))

```powershell
vcpkg install sdl2:x64-windows
```

### Configure & build
```powershell
cd pc
mkdir build
cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=[vcpkg]\scripts\buildsystems\vcpkg.cmake
cmake --build .
```

Output: `wmw_pc.exe`.

Verify PE x64:
```powershell
dumpbin /headers wmw_pc.exe | findstr machine
```
Expect: `8664 machine (x64)`.

---

## Run

```
wmw_pc.exe
assets\          ← Swampy-oriented tree (see strip script)
```

Or:
```powershell
.\wmw_pc.exe F:\path\to\assets 01_CUT_FIRST
```

Controls: **Esc** quit.

---

## What works now

| Feature | Tag |
|---------|-----|
| Windows x64 window + loop | REIMPLEMENTED |
| External assets/ discovery | REIMPLEMENTED |
| Parse real Level XML + list objects | REIMPLEMENTED |
| Debug draw objects by type | REIMPLEMENTED |
| Physics / Water / FMOD audio | STUB |
| Original GLES game renderer | UNKNOWN (was in libwmw.so) |

---

## CI artifact

GitHub Actions (`windows-latest`) uploads:

```
wmw-pc-windows-x64.zip
  wmw_pc.exe
  SDL2.dll
  assets\          (Swampy-only after strip)
  run.bat
  README.md
  ARCHITECTURE.md
```

---

## Hard rules

1. Never claim fidelity without evidence.
2. Never run or rename ARM `.so` as Windows DLL.
3. Physics / water stay STUB until `libwmw.so` behaviour is understood.
4. Level loader only parses formats observed in assets.
