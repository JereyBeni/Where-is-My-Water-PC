#include "core/assets.hpp"
#include "platform/filesystem.hpp"

#include <iostream>

namespace wmw::assets {

bool exists(const std::string& root) {
    using wmw::platform::fs::dirExists;
    using wmw::platform::fs::join;

    const char* required[] = {
        "Levels", "Sprites", "Textures", "Audio", "Data",
        "Animations", "Skeletons", "Curves", "Emitters", "Script", "Objects"
    };

    if (!dirExists(root)) {
        std::cerr << "[assets] root missing: " << root << "\n";
        return false;
    }

    bool ok = true;
    for (const char* sub : required) {
        if (!dirExists(join(root, sub))) {
            std::cerr << "[assets] missing: " << join(root, sub) << "\n";
            ok = false;
        }
    }
    return ok;
}

std::string path(const std::string& root, const std::string& relative) {
    return wmw::platform::fs::join(root, relative);
}

} // namespace wmw::assets
