#include "core/hs_loader.hpp"
#include "core/assets.hpp"
#include "platform/filesystem.hpp"

#include <iostream>
#include <sstream>
#include <cctype>

namespace wmw::core {

namespace {

std::string trim(const std::string& s) {
    size_t a = 0, b = s.size();
    while (a < b && std::isspace(static_cast<unsigned char>(s[a]))) ++a;
    while (b > a && std::isspace(static_cast<unsigned char>(s[b - 1]))) --b;
    return s.substr(a, b - a);
}

bool attr(const std::string& tag, const std::string& name, std::string& value) {
    const std::string key = name + "=";
    size_t p = tag.find(key);
    if (p == std::string::npos) return false;
    p += key.size();
    if (p >= tag.size()) return false;
    char q = tag[p];
    if (q != '"' && q != '\'') return false;
    size_t end = tag.find(q, p + 1);
    if (end == std::string::npos) return false;
    value = tag.substr(p + 1, end - p - 1);
    return true;
}

bool parseVec2(const std::string& s, Vec2& out) {
    std::istringstream iss(s);
    if (!(iss >> out.x >> out.y)) return false;
    return true;
}

std::string normalizeHsPath(const std::string& logical) {
    std::string p = logical;
    if (!p.empty() && p[0] == '/') p = p.substr(1);
    return p;
}

} // namespace

bool HsLoader::load(const std::string& assetsRoot, const std::string& hsLogicalPath,
                    InteractiveObject& out) {
    out = InteractiveObject{};
    out.hsPath = hsLogicalPath;

    const std::string rel = normalizeHsPath(hsLogicalPath);
    const std::string filePath = wmw::assets::path(assetsRoot, rel);

    std::string xml;
    if (!wmw::platform::fs::readTextFile(filePath, xml)) {
        std::cerr << "[HsLoader] failed to read: " << filePath << "\n";
        return false;
    }

    std::vector<Vec2> currentShape;
    bool inShape = false;

    size_t pos = 0;
    while (pos < xml.size()) {
        size_t lt = xml.find('<', pos);
        if (lt == std::string::npos) break;
        size_t gt = xml.find('>', lt + 1);
        if (gt == std::string::npos) break;
        std::string tag = xml.substr(lt + 1, gt - lt - 1);
        pos = gt + 1;

        if (tag.empty() || tag[0] == '?' || tag[0] == '!') continue;
        const bool closing = tag[0] == '/';
        std::string name = closing ? tag.substr(1) : tag;
        size_t sp = name.find_first_of(" \t\r\n");
        if (sp != std::string::npos) name = name.substr(0, sp);
        name = trim(name);

        if (!closing && name == "Shape") {
            currentShape.clear();
            inShape = true;
        } else if (closing && name == "Shape") {
            if (inShape && !currentShape.empty()) {
                out.shapes.push_back(currentShape);
            }
            inShape = false;
        } else if (!closing && name == "Point" && inShape) {
            std::string posAttr;
            if (attr(tag, "pos", posAttr)) {
                Vec2 v;
                if (parseVec2(posAttr, v)) {
                    currentShape.push_back(v);
                }
            }
        } else if (!closing && name == "Property") {
            std::string pname, pval;
            if (attr(tag, "name", pname) && attr(tag, "value", pval)) {
                out.properties[pname] = pval;
            }
        }
    }

    // Build Spout if this definition is a spout (Type or SpoutType present)
    const std::string type = out.properties.count("Type") ? out.properties["Type"] : "";
    const bool hasSpoutType = out.properties.count("SpoutType") > 0;
    if (type == "spout" || hasSpoutType) {
        Spout spout;
        spout.hsPath = hsLogicalPath;
        spout.applyProperties(out.properties);
        out.spout = spout;
    }

    return true;
}

} // namespace wmw::core
