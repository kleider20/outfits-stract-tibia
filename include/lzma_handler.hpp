#ifndef LZMA_HANDLER_HPP
#define LZMA_HANDLER_HPP

#include <string>
#include <vector>

class LzmaHandler {
public:
    static std::vector<uint8_t> decompress(const std::string& filePath);
};

#endif
