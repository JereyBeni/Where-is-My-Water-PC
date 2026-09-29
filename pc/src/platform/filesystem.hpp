#pragma once

#include <string>
#include <vector>

namespace wmw::platform {

// [REIMPLEMENTED] Portable filesystem helpers.
// Replaces Android assets / OBB / external storage paths.
namespace fs {

bool dirExists(const std::string& path);
bool fileExists(const std::string& path);

// Join with '/' (works on Windows APIs we use and on POSIX)
std::string join(const std::string& a, const std::string& b);

// Read entire text file into string. Returns false on failure.
bool readTextFile(const std::string& path, std::string& out);

// List filenames (not full paths) inside a directory. Non-recursive.
// Returns empty on failure.
std::vector<std::string> listFiles(const std::string& dir);

} // namespace fs
} // namespace wmw::platform
