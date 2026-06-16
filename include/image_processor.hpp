#ifndef IMAGE_PROCESSOR_HPP
#define IMAGE_PROCESSOR_HPP

#include <string>
#include <vector>

class ImageProcessor {
public:
    static bool extractAndSavePng(const std::vector<uint8_t>& bmpData, 
                                 uint32_t spriteId, 
                                 const std::string& outputPath);
};

#endif
