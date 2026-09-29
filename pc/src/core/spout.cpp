#include "core/spout.hpp"

#include <cmath>
#include <cstdlib>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace wmw::core {

namespace {

float parseFloat(const std::string& s, float def) {
    if (s.empty()) return def;
    char* end = nullptr;
    float v = std::strtof(s.c_str(), &end);
    if (end == s.c_str()) return def;
    return v;
}

int parseInt(const std::string& s, int def) {
    if (s.empty()) return def;
    char* end = nullptr;
    long v = std::strtol(s.c_str(), &end, 10);
    if (end == s.c_str()) return def;
    return static_cast<int>(v);
}

bool parseVec2(const std::string& s, Vec2& out) {
    if (s.empty()) return false;
    float x = 0.f, y = 0.f;
    if (std::sscanf(s.c_str(), "%f %f", &x, &y) != 2) return false;
    out.x = x;
    out.y = y;
    return true;
}

} // namespace

void Spout::applyProperties(const std::unordered_map<std::string, std::string>& props) {
    auto get = [&](const char* k) -> std::string {
        auto it = props.find(k);
        return it != props.end() ? it->second : std::string();
    };

    type = parseSpoutType(get("SpoutType"));
    // Level XML often sets Type=spout without SpoutType; hs supplies SpoutType.
    fluidType = parseFluidType(get("FluidType"));
    particlesPerSecond = parseFloat(get("ParticlesPerSecond"), particlesPerSecond);
    particleSpeed = parseFloat(get("ParticleSpeed"), particleSpeed);
    numberParticles = parseInt(get("NumberParticles"), numberParticles);
    infiniteParticles = (numberParticles < 0);

    Vec2 off;
    if (parseVec2(get("OffsetToMouth"), off)) {
        offsetToMouth = off;
    }
    expulsionAngleDeg = parseFloat(get("ExpulsionAngle"), expulsionAngleDeg);

    const std::string goal = get("Goal");
    if (!goal.empty() && goal != "0") {
        isGoal = true;
    }
    goalPreset = get("GoalPreset");
}

Vec2 Spout::mouthWorld() const {
    // Rotate offset by world angle (degrees). Convention matches 2D object Angle in levels.
    const float rad = worldAngleDeg * static_cast<float>(M_PI / 180.0);
    const float c = std::cos(rad);
    const float s = std::sin(rad);
    const float lx = offsetToMouth.x;
    const float ly = offsetToMouth.y;
    Vec2 out;
    out.x = worldPos.x + c * lx - s * ly;
    out.y = worldPos.y + s * lx + c * ly;
    return out;
}

std::vector<ParticleDescription> Spout::emit(float dt) {
    std::vector<ParticleDescription> out;

    // Drains collect fluid — they are not continuous emitters in authoring data
    // [INFERRED from SpoutType=DrainSpout + Goal]. Do not spawn from drains.
    if (type == SpoutType::DrainSpout) {
        return out;
    }

    if (particlesPerSecond <= 0.f || dt <= 0.f) {
        return out;
    }
    if (!infiniteParticles && emittedTotal_ >= numberParticles && numberParticles >= 0) {
        return out;
    }

    emitAccum_ += particlesPerSecond * dt;
    int n = static_cast<int>(emitAccum_);
    if (n <= 0) return out;
    emitAccum_ -= static_cast<float>(n);

    if (!infiniteParticles && numberParticles >= 0) {
        const int remain = numberParticles - emittedTotal_;
        if (n > remain) n = remain;
    }

    // Direction: object Angle + ExpulsionAngle [INFERRED combination].
    const float dirDeg = worldAngleDeg + expulsionAngleDeg;
    const float dirRad = dirDeg * static_cast<float>(M_PI / 180.0);
    // Asset Y often grows up; emission along local +X of spout mouth is common for
    // OpenSpout defaults (OffsetToMouth ~1.8 0). [INFERRED]
    const float vx = std::cos(dirRad) * particleSpeed;
    const float vy = std::sin(dirRad) * particleSpeed;

    const Vec2 mouth = mouthWorld();

    out.reserve(static_cast<size_t>(n));
    for (int i = 0; i < n; ++i) {
        ParticleDescription p;
        p.position = mouth;
        p.velocity = {vx, vy};
        p.fluid = fluidType;
        out.push_back(p);
        ++emittedTotal_;
    }
    return out;
}

void Spout::log() const {
    std::cout << "[Spout]\n"
              << "  object: " << objectName << "\n"
              << "  hs: " << hsPath << "\n"
              << "  type: " << spoutTypeName(type) << "\n"
              << "  fluid: " << fluidTypeName(fluidType) << "\n"
              << "  particles/sec: " << particlesPerSecond << "\n"
              << "  speed: " << particleSpeed << "\n"
              << "  number: " << numberParticles
              << (infiniteParticles ? " (infinite)" : "") << "\n"
              << "  offsetToMouth: " << offsetToMouth.x << " " << offsetToMouth.y << "\n"
              << "  expulsionAngle: " << expulsionAngleDeg << "\n"
              << "  world: " << worldPos.x << " " << worldPos.y
              << " angle=" << worldAngleDeg << "\n";
    if (isGoal) {
        std::cout << "  goal: yes preset=" << goalPreset << "\n";
    }
}

} // namespace wmw::core
