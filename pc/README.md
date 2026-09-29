# Where's My Water? — Windows x64 PC Port

Native **Windows x64 (PE)** port skeleton.  
No ARM code, no JNI, no loading of `libwmw.so`.

Read **[ARCHITECTURE.md](ARCHITECTURE.md)** for binary analysis and status tags.

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
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE=[vcpkg]/cmake\scripts\buildsystems\vcpkg.cmake
cmake --build . --config Release
```

Output: `Release\wmw_pc.exe` (or `wmw_pc.exe` depending on generator).

Verify PE x64 (Developer Command Prompt):
```powershell
dumpbin /headers wmw_pc.exe | findstr machine
```
Expect: `8664 machine (x64)`.

---

## Run

```
wmw_pc.exe
assets\          ← copy from app/src/main/assets
```

Or:
```powershell
.\wmw_pc.exe F:\path\to\assets 01_CUT_FIRST
```

Controls: **Esc** quit. Mouse position tracked (dig not wired yet).

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
  assets\n  run.bat
  README.md
```

---

## Hard rules (from project research)

1. Never claim fidelity without evidence.
2. Never run or rename ARM `.so` as Windows DLL.
3. Physics / water stay STUB until `libwmw.so` behaviour is understood.
4. Level loader only parses formats observed in assets.
