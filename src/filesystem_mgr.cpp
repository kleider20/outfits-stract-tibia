#include "filesystem_mgr.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

bool FilesystemMgr::createOutfitFolder(uint32_t outfitId, const std::string& baseDir) {
    std::string path = baseDir + "/" + std::to_string(outfitId);
    try {
        if (!fs::exists(path)) {
            fs::create_directories(path);
        }
        return true;
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error creating folder: " << e.what() << std::endl;
        return false;
    }
}

bool FilesystemMgr::writeOutfitData(uint32_t outfitId, uint32_t frames, const std::string& baseDir) {
    std::string path = baseDir + "/" + std::to_string(outfitId) + "/outfit.data.txt";
    std::ofstream file(path);
    if (!file.is_open()) return false;

    // MyAAC expects a serialized PHP array format: a:X:{...}
    // For simplicity, we'll write a basic version that MyAAC can parse or that mimics the format.
    // Example: a:1:{s:12:"framesNumber";i:10;}
    file << "a:1:{s:12:\"framesNumber\";i:" << frames << ";}";
    file.close();
    return true;
}

std::string FilesystemMgr::getOutputPath(const std::string& baseDir, uint32_t outfitId, 
                                       int anim, int mount, int addon, int dir) {
    return baseDir + "/" + std::to_string(outfitId) + "/" + 
           std::to_string(anim) + "_" + std::to_string(mount) + "_" + 
           std::to_string(addon) + "_" + std::to_string(dir) + ".png";
}
