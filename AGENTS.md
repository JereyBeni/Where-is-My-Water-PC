# Where's My Water PC Port Research

## Project Goal

This repository is being investigated as a source for creating a faithful PC port of the original **Where's My Water?**

The goal is NOT to make a new remake from scratch.

The goal is to understand, preserve, and where legally appropriate reimplement the original game's architecture, gameplay behavior, physics, level system, rendering pipeline, and data formats for a native PC implementation.

The repository may contain decompiled/reconstructed Java code as well as native Android/NDK components.

---

## IMPORTANT RULES

### 1. Do not invent code

Never claim that a class, function, file, native library, algorithm, or subsystem exists unless it has actually been inspected in this repository.

If something is inferred from naming or references, explicitly label it:

> INFERENCE

If something cannot be determined from the repository, say:

> NOT DETERMINED FROM AVAILABLE SOURCE

---

### 2. Analyze the entire repository

Do not limit the investigation to:

```text
app/src/main/java/
```

Also inspect:

```text
app/src/main/cpp/
app/src/main/jni/
app/src/main/jniLibs/
app/src/main/assets/
app/src/main/res/
app/src/main/
setupNDK.sh
build.gradle
settings.gradle
gradle.properties
.github/
```

Search recursively when necessary.

---

## Primary Investigation Targets

### Java

Pay special attention to:

```text
app/src/main/java/com/disney/WMW/WMWActivity.java
```

and investigate related classes such as:

```text
BaseActivity
WMWView
Renderer
Game
Level
Physics
Water
Input
Audio
Save
```

Do not assume these exact classes exist. Search for them and report the actual structure.

---

# Native Code Investigation

This is extremely important.

Search for:

```text
System.loadLibrary
libmui
mui
native
JNI
NDK
C++
C
.so
.cpp
.c
.h
```

In particular investigate:

```java
System.loadLibrary("mui");
```

Determine:

1. Where `libmui` comes from.
2. Whether the repository contains its source.
3. Whether only compiled `.so` binaries are present.
4. Which Java methods call into it.
5. Which native methods are declared.
6. Whether rendering, physics, gameplay, audio, or other systems are implemented there.

Do not assume `libmui` is the entire game engine. Prove what can be proven from the repository.

---

# Android Architecture

Map the architecture:

```text
Android Activity
        ↓
Android View
        ↓
Renderer
        ↓
JNI
        ↓
Native library
        ↓
Game systems
```

Correct this diagram if the actual repository shows a different architecture.

Identify which parts are Android-specific and which parts represent game logic.

---

# Renderer Investigation

Determine:

* Rendering API
* OpenGL / OpenGL ES / other API
* Renderer classes
* Surface/View classes
* Texture loading
* Sprite rendering
* Frame/update loop
* Screen scaling
* Resolution handling

Identify what would need to be replaced for PC.

---

# Game Loop

Find the actual implementation of:

* update
* render
* input
* timing
* pause
* resume
* frame scheduling
* fixed timestep, if present

Document the call chain.

Do not infer a game loop merely from `Activity` lifecycle methods.

---

# Physics and Water

This is one of the highest-priority investigations.

Search for implementation or references involving:

```text
water
fluid
physics
gravity
collision
terrain
dig
sand
mud
particle
liquid
pipe
duck
Swampy
Cranky
Allie
```

Determine whether these systems are implemented in:

* Java
* native C/C++
* external libraries
* data files
* compiled `.so` libraries

Pay particular attention to the actual water simulation and terrain interaction.

---

# Level System

Investigate how levels are represented and loaded.

Search for:

```text
level
stage
puzzle
xml
json
binary
asset
map
terrain
object
```

Determine:

* level file format
* object format
* terrain representation
* level loading code
* level metadata
* save/progress data
* challenge systems

The goal is to determine whether the original level data can be loaded by a future PC runtime.

---

# Assets

Inspect:

```text
assets/
res/
textures/
sprites/
sounds/
music/
```

Identify formats and loading code.

Do not assume that Android resources are the same as gameplay assets.

---

# Audio

Investigate:

```text
FMOD
FMODAudioDevice
sound
music
audio
```

Determine:

* audio middleware
* Android-specific components
* native components
* file formats
* what would need to change on PC

---

# Android-only Systems

Identify systems that are irrelevant to a PC port, including where applicable:

* Activity lifecycle
* Android permissions
* Google Play
* In-app purchases
* Android sharing
* Twitter integration
* Android display metrics
* touch/sensor APIs
* Android-specific audio
* Android-specific threading
* Android-specific graphics context

Do not delete or ignore them during analysis. Explain their role first.

---

# PC Port Architecture

After investigating the source, propose a realistic architecture.

The target is:

```text
                WMW PC
                  │
        ┌─────────┴─────────┐
        │                   │
     Game Core          Platform Layer
        │                   │
        │              Windows / PC
        │
 ┌──────┼────────┬─────────┐
 │      │        │         │
Physics Water  Levels   Gameplay
 │
Renderer
```

This is only a starting concept.

Modify it according to evidence from the repository.

Possible PC technologies may include:

* C++
* C#
* SDL
* OpenGL
* DirectX
* Vulkan
* another appropriate framework

Do not choose a technology merely because it is popular. Explain why it fits the source architecture.

---

# Porting Strategy

Classify each important component as one of:

### REUSE

Can potentially be used directly.

### PORT

Can be translated/adapted to PC.

### REIMPLEMENT

The original implementation is platform-specific or unavailable.

### RESEARCH

More investigation is required.

### NOT RELEVANT

Android-only or unrelated to gameplay.

---

# Evidence Levels

For every important conclusion, distinguish:

### CONFIRMED

Directly demonstrated by inspected source code.

### STRONG EVIDENCE

Supported by multiple source references but not completely proven.

### INFERENCE

Reasonable interpretation based on naming/structure.

### UNKNOWN

The repository does not provide enough information.

---

# Required Final Report

Produce a technical report with these sections:

## 1. Repository Tree

Show the important files and directories.

## 2. Architecture

Explain how the original application works.

## 3. Java Layer

List important Java classes and their roles.

## 4. Native Layer

Explain JNI, NDK, `.so` libraries, and especially `libmui`.

## 5. Renderer

Explain the rendering pipeline.

## 6. Game Loop

Explain update/render/input/timing.

## 7. Physics and Water

Explain where the actual simulation lives.

## 8. Levels and Data

Explain how levels are stored and loaded.

## 9. Assets

Explain asset formats and loading.

## 10. Audio

Explain FMOD and other audio systems.

## 11. Android-only Systems

Explain what can be discarded for PC.

## 12. PC Port Plan

Give a concrete architecture for a PC implementation.

## 13. File-by-file Priority

Use this table:

| File    | Role    | Evidence  | PC Relevance | Action |
| ------- | ------- | --------- | ------------ | ------ |
| example | example | Confirmed | High         | Port   |

Do not fabricate entries.

## 14. Missing Components

Clearly identify anything that is unavailable because it exists only inside compiled binaries or outside the repository.

## 15. Most Important Discovery

Identify the single most important technical discovery for creating a faithful PC port, based only on repository evidence.

---

# Critical Principle

The most important question is:

> How much of the original game's actual implementation is available here?

Do NOT confuse:

```text
Android application wrapper
```

with:

```text
actual game engine
```

The investigation must determine where the real gameplay implementation lives.

---

# Legal / Preservation Context

This is a research and preservation project.

Do not recommend distributing proprietary copyrighted assets or source code without authorization.

For implementation guidance, prioritize:

* understanding architecture
* interoperability
* preservation
* clean-room reimplementation where appropriate
* loading legally obtained game data

Do not claim that decompiled code is automatically legally redistributable.

---

# Working Style

Be extremely technical.

Prefer concrete evidence over speculation.

When possible, quote short relevant code snippets and give exact file paths and line numbers.

If a binary cannot be meaningfully inspected as source code, explicitly state that and explain what can still be learned from:

* strings
* symbols
* JNI exports
* references
* load paths
* callers
* file formats
* surrounding source code

The objective is to turn this repository into a **technical map of Where's My Water** and determine the shortest realistic path toward a faithful PC implementation.
