#include "core/assets.hpp"

#include <sys/stat.h>
#include <iostream>

namespace wmw::assets {

namespace {

bool dirExists(const std::string& p) {
    struct stat st {};
    if (stat(p.c_str(), &st) != 0) {
        return false;
    }
#ifdef _WIN32
    return (st.st_mode & _S_IFDIR) != 0;
#else
    return S_ISDIR(st.st_mode);
#endif
}

} // namespace

bool exists(const std::string& root) {
    // Minimal sanity check against the original asset layout
    const char* required[] = {
        "Levels",
        "Sprites",
        "Textures",
        "Audio",
        "Data"
    };

    if (!dirExists(root)) {
        return false;
    }

    for (const char* sub : required) {
        if (!dirExists(root + "/" + sub)) {
            std::cerr << "[assets] missing expected folder: " << root << "/" << sub << "\n";
            return false;
        }
    }
    return true;
}

std::string path(const std::string& root, const std::string& relative) {
    if (root.empty()) return relative;
    if (relative.empty()) return root;
    if (root.back() == '/' || root.back() == '\\') {
        return root + relative;
    }
    return root + "/" + relative;
}

} // namespace wmw::assets
