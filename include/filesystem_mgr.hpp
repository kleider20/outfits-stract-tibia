#ifndef FILESYSTEM_MGR_HPP
#define FILESYSTEM_MGR_HPP

#include <string>
#include <vector>
#include "dat_parser.hpp"

class FilesystemMgr {
public:
    static bool createOutfitFolder(uint32_t outfitId, const std::string& baseDir);
    static bool writeOutfitData(uint32_t outfitId, uint32_t frames, const std::string& baseDir);
    static std::string getOutputPath(const std::string& baseDir, uint32_t outfitId, 
                                   int anim, int mount, int addon, int dir);
};

#endif
