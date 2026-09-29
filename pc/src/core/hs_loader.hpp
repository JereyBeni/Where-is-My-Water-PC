#pragma once

#include "core/interactive_object.hpp"

#include <string>

namespace wmw::core {

// Loads assets/Objects/*.hs XML InteractiveObject definitions [CONFIRMED format].
class HsLoader {
public:
    // hsLogicalPath e.g. "/Objects/shower_head.hs" or "Objects/shower_head.hs"
    bool load(const std::string& assetsRoot, const std::string& hsLogicalPath,
              InteractiveObject& out);
};

} // namespace wmw::core
