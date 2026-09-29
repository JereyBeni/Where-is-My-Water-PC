#include "core/level_loader.hpp"
#include "core/assets.hpp"
#include "platform/filesystem.hpp"

#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace wmw::core {

namespace {

// Minimal helpers for the simple, regular XML used by WMW levels.
// Not a general XML library — only what the format needs.

std::string trim(const std::string& s) {
    size_t a = 0, b = s.size();
    while (a < b && std::isspace(static_cast<unsigned char>(s[a]))) ++a;
    while (b > a && std::isspace(static_cast<unsigned char>(s[b - 1]))) --b;
    return s.substr(a, b - a);
}

// Extract attribute value for name="..." or name='...'
bool attr(const std::string& tag, const std::string& name, std::string& value) {
    const std::string key = name + "=";
    size_t p = tag.find(key);
    if (p == std::string::npos) return false;
    p += key.size();
    if (p >= tag.size()) return false;
    char quote = tag[p];
    if (quote != '"' && quote != '\'') return false;
    size_t end = tag.find(quote, p + 1);
    if (end == std::string::npos) return false;
    value = tag.substr(p + 1, end - p - 1);
    return true;
}

bool parseVec2(const std::string& s, float& x, float& y) {
    std::istringstream iss(s);
    if (!(iss >> x >> y)) return false;
    return true;
}

} // namespace

bool LevelLoader::load(const std::string& assetsRoot, const std::string& levelName, LevelData& out) {
    out = LevelData{};
    out.name = levelName;

    const std::string filePath = wmw::assets::path(
        assetsRoot, "Levels/" + levelName + ".xml");

    std::string xml;
    if (!wmw::platform::fs::readTextFile(filePath, xml)) {
        std::cerr << "[LevelLoader] failed to read: " << filePath << "\n";
        return false;
    }

    // Walk tags in order
    LevelObject current;
    bool inObject = false;
    bool inRoom = false;

    size_t pos = 0;
    while (pos < xml.size()) {
        size_t lt = xml.find('<', pos);
        if (lt == std::string::npos) break;
        size_t gt = xml.find('>', lt + 1);
        if (gt == std::string::npos) break;

        std::string tag = xml.substr(lt + 1, gt - lt - 1);
        pos = gt + 1;

        // Skip XML declaration / comments
        if (tag.empty() || tag[0] == '?' || tag[0] == '!') continue;

        const bool closing = tag[0] == '/';
        std::string name = closing ? tag.substr(1) : tag;
        // strip attributes from name
        size_t sp = name.find_first_of(" \t\r\n");
        if (sp != std::string::npos) name = name.substr(0, sp);
        name = trim(name);

        if (!closing && name == "Object") {
            current = LevelObject{};
            attr(tag, "name", current.name);
            inObject = true;
        } else if (closing && name == "Object") {
            if (inObject) {
                out.objects.push_back(current);
            }
            inObject = false;
        } else if (!closing && name == "AbsoluteLocation") {
            std::string val;
            if (attr(tag, "value", val)) {
                float x = 0, y = 0;
                if (parseVec2(val, x, y)) {
                    if (inObject) {
                        current.x = x;
                        current.y = y;
                    } else if (inRoom) {
                        out.roomX = x;
                        out.roomY = y;
                        out.hasRoom = true;
                    }
                }
            }
        } else if (!closing && name == "Property") {
            std::string pname, pval;
            if (attr(tag, "name", pname) && attr(tag, "value", pval)) {
                if (inObject) {
                    current.properties[pname] = pval;
                } else {
                    out.levelProperties[pname] = pval;
                }
            }
        } else if (!closing && name == "Room") {
            inRoom = true;
        } else if (closing && name == "Room") {
            inRoom = false;
        }
    }

    std::cout << "[LevelLoader] loaded '" << levelName << "' — "
              << out.objects.size() << " objects";
    if (out.hasRoom) {
        std::cout << ", room @ (" << out.roomX << ", " << out.roomY << ")";
    }
    std::cout << "\n";

    // Debug: type histogram
    std::unordered_map<std::string, int> types;
    for (const auto& o : out.objects) {
        types[o.get("Type", "(none)")]++;
    }
    for (const auto& kv : types) {
        std::cout << "  type '" << kv.first << "': " << kv.second << "\n";
    }

    return true;
}

std::vector<std::string> LevelLoader::listLevels(const std::string& assetsRoot) {
    std::vector<std::string> names;
    const std::string dir = wmw::assets::path(assetsRoot, "Levels");
    auto files = wmw::platform::fs::listFiles(dir);
    for (const auto& f : files) {
        if (f.size() > 4 && f.substr(f.size() - 4) == ".xml") {
            names.push_back(f.substr(0, f.size() - 4));
        }
    }
    std::sort(names.begin(), names.end());
    return names;
}

} // namespace wmw::core
