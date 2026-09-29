#pragma once

#include <string>

namespace wmw::assets {

// [REIMPLEMENTED] Asset root validation against original layout.
bool exists(const std::string& root);

std::string path(const std::string& root, const std::string& relative);

} // namespace wmw::assets
