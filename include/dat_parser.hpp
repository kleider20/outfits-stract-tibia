#ifndef DAT_PARSER_HPP
#define DAT_PARSER_HPP

#include <string>
#include <vector>
#include <map>
#include <iostream>

struct OutfitInfo {
    uint32_t outfitId;
    std::string name;
    uint32_t spriteId;
    uint32_t frames;
    // Mapping: [animation][mount][addon][direction] -> sprite_offset/id
    std::map<int, std::map<int, std::map<int, std::map<int, uint32_t>>>> spriteMap;
};

class DatParser {
public:
    DatParser();
    bool load(const std::string& filePath);
    const OutfitInfo* getOutfit(uint32_t outfitId) const;
    std::vector<uint32_t> getAllOutfitIds() const;

private:
    std::map<uint32_t, OutfitInfo> outfits;
};

#endif
