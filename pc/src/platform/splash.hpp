#pragma once

#include <string>

struct SDL_Renderer;

namespace wmw::platform {

class Input;
class Window;

// [REIMPLEMENTED] Startup copyright splash using pc/Drawing.png
// Shows until timeout or any key/click / Esc still quits.
bool showCopyrightSplash(Window& window, Input& input,
                         const std::string& imagePath,
                         float minSeconds = 2.0f,
                         float maxSeconds = 8.0f);

// Resolve Drawing.png next to the executable or under common relative paths.
std::string findCopyrightImagePath();

} // namespace wmw::platform
