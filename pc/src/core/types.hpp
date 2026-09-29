#pragma once

#include <string>

namespace wmw::core {

// Simple 2D vector used across loaders / debug emission.
// Not claimed to match Walaber::Vector2 layout — only the conceptual 2-float pair
// used in .hs "x y" strings [CONFIRMED in assets].
struct Vec2 {
    float x = 0.f;
    float y = 0.f;
};

// Fluid type names observed in .hs FluidType property and libwmw texture/strings.
// Full enum ordinals inside libwmw.so remain [UNKNOWN].
enum class FluidType {
    Unknown = 0,
    Water,   // [CONFIRMED] .hs value "Water"
    Mud,     // [CONFIRMED] string/texture evidence in libwmw / assets
    Steam,   // [CONFIRMED] string/texture evidence
    Poison,  // [CONFIRMED] string/texture + star_poison.hs context
    Ooze,    // [CONFIRMED] string/texture evidence
    Other
};

inline FluidType parseFluidType(const std::string& s) {
    if (s == "Water") return FluidType::Water;
    if (s == "Mud") return FluidType::Mud;
    if (s == "Steam") return FluidType::Steam;
    if (s == "Poison") return FluidType::Poison;
    if (s == "Ooze") return FluidType::Ooze;
    if (s.empty()) return FluidType::Unknown;
    return FluidType::Other;
}

inline const char* fluidTypeName(FluidType t) {
    switch (t) {
    case FluidType::Water: return "Water";
    case FluidType::Mud: return "Mud";
    case FluidType::Steam: return "Steam";
    case FluidType::Poison: return "Poison";
    case FluidType::Ooze: return "Ooze";
    case FluidType::Other: return "Other";
    default: return "Unknown";
    }
}

// SpoutType strings from .hs DefaultProperties [CONFIRMED].
enum class SpoutType {
    Unknown = 0,
    OpenSpout,   // shower_head.hs
    TouchSpout,  // touch_spout.hs
    DrainSpout,  // basic_drain.hs / broken_pipe.hs
    Other
};

inline SpoutType parseSpoutType(const std::string& s) {
    if (s == "OpenSpout") return SpoutType::OpenSpout;
    if (s == "TouchSpout") return SpoutType::TouchSpout;
    if (s == "DrainSpout") return SpoutType::DrainSpout;
    if (s.empty()) return SpoutType::Unknown;
    return SpoutType::Other;
}

inline const char* spoutTypeName(SpoutType t) {
    switch (t) {
    case SpoutType::OpenSpout: return "OpenSpout";
    case SpoutType::TouchSpout: return "TouchSpout";
    case SpoutType::DrainSpout: return "DrainSpout";
    case SpoutType::Other: return "Other";
    default: return "Unknown";
    }
}

} // namespace wmw::core
