#include "platform/filesystem.hpp"

#include <fstream>
#include <sstream>
#include <sys/stat.h>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#include <dirent.h>
#endif

namespace wmw::platform::fs {

bool dirExists(const std::string& path) {
    struct stat st{};
    if (stat(path.c_str(), &st) != 0) return false;
#ifdef _WIN32
    return (st.st_mode & _S_IFDIR) != 0;
#else
    return S_ISDIR(st.st_mode);
#endif
}

bool fileExists(const std::string& path) {
    struct stat st{};
    if (stat(path.c_str(), &st) != 0) return false;
#ifdef _WIN32
    return (st.st_mode & _S_IFREG) != 0;
#else
    return S_ISREG(st.st_mode);
#endif
}

std::string join(const std::string& a, const std::string& b) {
    if (a.empty()) return b;
    if (b.empty()) return a;
    char last = a.back();
    if (last == '/' || last == '\\') return a + b;
    return a + "/" + b;
}

bool readTextFile(const std::string& path, std::string& out) {
    std::ifstream ifs(path, std::ios::in | std::ios::binary);
    if (!ifs) return false;
    std::ostringstream ss;
    ss << ifs.rdbuf();
    out = ss.str();
    return true;
}

std::vector<std::string> listFiles(const std::string& dir) {
    std::vector<std::string> result;
#ifdef _WIN32
    std::string pattern = join(dir, "*");
    WIN32_FIND_DATAA fd{};
    HANDLE h = FindFirstFileA(pattern.c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) return result;
    do {
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
        result.emplace_back(fd.cFileName);
    } while (FindNextFileA(h, &fd));
    FindClose(h);
#else
    DIR* d = opendir(dir.c_str());
    if (!d) return result;
    while (dirent* ent = readdir(d)) {
        if (ent->d_name[0] == '.') continue;
        result.emplace_back(ent->d_name);
    }
    closedir(d);
#endif
    return result;
}

} // namespace wmw::platform::fs
