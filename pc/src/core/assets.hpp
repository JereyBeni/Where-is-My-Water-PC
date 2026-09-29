#pragma once

#include <string>

namespace wmw::assets {

// Returns true if the given path looks like a valid assets root
// (contains expected subfolders such as Levels, Sprites, etc.)
bool exists(const std::string& root);

// Join root + relative path in a platform-friendly way
std::string path(const std::string& root, const std::string& relative);

} // namespace wmw::assets
