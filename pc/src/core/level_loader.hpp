#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace wmw::core {

// [CONFIRMED] fields observed in real Level XML files.
struct LevelObject {
    std::string name;
    float x = 0.f;
    float y = 0.f;
    std::unordered_map<std::string, std::string> properties;

    std::string get(const std::string& key, const std::string& def = "") const {
        auto it = properties.find(key);
        return it != properties.end() ? it->second : def;
    }
};

struct LevelData {
    std::string name;
    std::vector<LevelObject> objects;
    float roomX = 0.f;
    float roomY = 0.f;
    bool hasRoom = false;
    std::unordered_map<std::string, std::string> levelProperties;
};

// [REIMPLEMENTED] Parses original Level XML (Objects / AbsoluteLocation / Properties / Room).
// Does NOT simulate the level — only loads authoring data.
class LevelLoader {
public:
    // levelName without path/extension, e.g. "01_CUT_FIRST"
    bool load(const std::string& assetsRoot, const std::string& levelName, LevelData& out);

    // List *.xml basenames in Levels/
    static std::vector<std::string> listLevels(const std::string& assetsRoot);
};

} // namespace wmw::core
