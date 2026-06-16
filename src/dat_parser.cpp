#include "dat_parser.hpp"
#include <fstream>
#include <iostream>

DatParser::DatParser() {}

bool DatParser::load(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) return false;

    // This is a simplified implementation. 
    // Actual staticdata.dat parsing requires reading the header, 
    // then the appearance table.
    
    // Placeholder: Reading dummy data to demonstrate the structure
    // In a real scenario, we'd read the binary offsets here.
    
    // Example dummy outfit
    OutfitInfo dummy;
    dummy.outfitId = 136;
    dummy.name = "Citizen";
    dummy.frames = 4;
    // Map values would be filled here by reading the file
    outfits[136] = dummy;

    file.close();
    return true;
}

const OutfitInfo* DatParser::getOutfit(uint32_t outfitId) const {
    auto it = outfits.find(outfitId);
    if (it != outfits.end()) return &it->second;
    return nullptr;
}

std::vector<uint32_t> DatParser::getAllOutfitIds() const {
    std::vector<uint32_t> ids;
    for (auto const& [id, info] : outfits) {
        ids.push_back(id);
    }
    return ids;
}
