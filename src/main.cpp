#include <iostream>
#include <string>
#include <vector>
#include "dat_parser.hpp"
#include "lzma_handler.hpp"
#include "image_processor.hpp"
#include "filesystem_mgr.hpp"

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: OutfitExtractor <assets_dir> <output_dir>" << std::endl;
        std::cout << "Example: OutfitExtractor \"C:\\Tibia\\assets\" \"C:\\myaac\\animatedOutfits\"" << std::endl;
        return 1;
    }

    std::string assetsDir = argv[1];
    std::string outputDir = argv[2];

    std::cout << "[1/4] Parsing staticdata... " << std::endl;
    DatParser parser;
    // We need to find the staticdata-*.dat file in the assetsDir
    // For now, we'll assume the user provides the path or we search for it.
    // (Simplified: searching for staticdata-*.dat)
    
    // This is a placeholder for the search logic
    std::string datPath = assetsDir + "/staticdata.dat"; 
    if (!parser.load(datPath)) {
        std::cerr << "Failed to load staticdata.dat" << std::endl;
        return 1;
    }

    std::vector<uint32_t> outfitIds = parser.getAllOutfitIds();
    std::cout << "Found " << outfitIds.size() << " outfits." << std::endl;

    for (uint32_t id : outfitIds) {
        const OutfitInfo* info = parser.getOutfit(id);
        if (!info) continue;

        std::cout << "Processing Outfit " << id << " (" << info->name << ")..." << std::endl;

        if (!FilesystemMgr::createOutfitFolder(id, outputDir)) continue;

        // Logic to iterate through animations, mounts, addons, directions
        // and extract from .lzma files...
        
        // This part will call LzmaHandler and ImageProcessor
        
        FilesystemMgr::writeOutfitData(id, info->frames, outputDir);
    }

    std::cout << "Extraction complete!" << std::endl;
    return 0;
}
