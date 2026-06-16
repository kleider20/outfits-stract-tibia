#include "image_processor.hpp"
#include <iostream>
#include <vector>

// We use stb_image_write for saving PNGs
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../external/stb_image_write.h"

bool ImageProcessor::extractAndSavePng(const std::vector<uint8_t>& bmpData, 
                                      uint32_t spriteId, 
                                      const std::string& outputPath) {
    if (bmpData.empty()) return false;

    // In a real BMP, we'd parse the header to find the pixel data.
    // For this example, we assume we have the raw pixels.
    
    // Dummy image data (64x64 RGBA)
    int width = 64;
    int height = 64;
    int channels = 4;
    std::vector<uint8_t> pixels(width * height * channels, 255); // White image

    // Save as PNG
    int result = stbi_write_png(outputPath.c_str(), width, height, channels, pixels.data(), width * channels);
    
    return result != 0;
}
