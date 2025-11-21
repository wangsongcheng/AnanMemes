#ifndef IMAGE_HPP
#define IMAGE_HPP
#include <cstring>
#include "glm/glm.hpp"
#ifndef MAX_BYTE
#define MAX_BYTE 0xff
#endif
#define ROW_COLUMN_TO_INDEX(ROW_INDEX, COLUMN_INDEX, COLUMN)((ROW_INDEX) * (COLUMN) + (COLUMN_INDEX))
namespace ananImage{
    // static inline void copy(const void *source, void *destination, uint32_t row, uint32_t column, const glm::uvec2&cutSzie, uint32_t srcWidth){
    //     //图片必须是一个像素4个字节或者rgba
    //     //如果要截取第2行的图片，需要跳过 row * 目标图片的高度 * 原图宽度
    //     const uint32_t uiLineSize = cutSzie.x * 4;
    //     const uint32_t uiSourceLineSize = srcWidth * 4;
    //     const uint32_t imageOffsetStart = row * cutSzie.y * uiSourceLineSize + column * uiLineSize;
    //     for (size_t dataOffset = 0; dataOffset < cutSzie.y; ++dataOffset) {
    //         memcpy((char *)destination + dataOffset * uiLineSize, (char *)source + imageOffsetStart + dataOffset * uiSourceLineSize, uiLineSize);
    //     }
    // }
    // static inline void copy(const stbi_uc *source, stbi_uc *destination, const glm::uvec2&srcSize, const glm::uvec2&dstSize, const glm::uvec2&destOffset){
    //     const uint32_t dstLineWidth = dstSize.x * 4, srcLineWidth = srcSize.x * 4;
    //     for (size_t dataOffset = 0; dataOffset < srcSize.y; ++dataOffset) {
    //         //TODO:还需要加入偏移
    //         memcpy(destination + dataOffset * dstLineWidth, source + dataOffset * srcLineWidth, srcLineWidth);
    //         // memcpy(destination + destOffset.y + dataOffset * dstLineWidth, source + destOffset.x * 4 + dataOffset * srcLineWidth, srcLineWidth);
    //     }
    // }
    // static inline void calculateImageSize(const glm::uvec2& extent, const glm::uvec2& size, glm::uvec2& newSize){    
    //     float widthRatio = static_cast<float>(extent.x - 1) / static_cast<float>(size.x);
    //     float heightRatio = static_cast<float>(extent.y - 1) / static_cast<float>(size.y);
    //     float scale = std::min(widthRatio, heightRatio);
    //     newSize.x = static_cast<uint32_t>(std::round(size.x * scale));
    //     newSize.y = static_cast<uint32_t>(std::round(size.y * scale));
    //     newSize.x = std::min(newSize.x, extent.x > 1 ? extent.x - 1 : 1u);
    //     newSize.y = std::min(newSize.y, extent.y > 1 ? extent.y - 1 : 1u);
    //     newSize.x = std::max(newSize.x, 1u);
    //     newSize.y = std::max(newSize.y, 1u);
    //     assert(newSize.x < extent.x && newSize.y < extent.y);
    // }

    static inline void calculateImageSize(const glm::uvec2& extent, const glm::uvec2& size, glm::uvec2& newSize, float scaleFactor = 0.7f) {
        scaleFactor = glm::clamp(scaleFactor, 0.1f, 1.0f);
        
        glm::uvec2 targetSize = extent;
        targetSize.x = static_cast<uint32_t>(std::round(extent.x * scaleFactor));
        targetSize.y = static_cast<uint32_t>(std::round(extent.y * scaleFactor));
        
        targetSize.x = glm::clamp(targetSize.x, 1u, extent.x > 1 ? extent.x - 1 : 1u);
        targetSize.y = glm::clamp(targetSize.y, 1u, extent.y > 1 ? extent.y - 1 : 1u);
        
        float widthRatio = static_cast<float>(targetSize.x) / static_cast<float>(size.x);
        float heightRatio = static_cast<float>(targetSize.y) / static_cast<float>(size.y);
        
        float scale = std::min(widthRatio, heightRatio);
        
        if (scale > 1.0f) {
            scale = 1.0f;
        }
        
        newSize.x = static_cast<uint32_t>(std::round(size.x * scale));
        newSize.y = static_cast<uint32_t>(std::round(size.y * scale));
        
        newSize.x = glm::clamp(newSize.x, 1u, extent.x > 1 ? extent.x - 1 : 1u);
        newSize.y = glm::clamp(newSize.y, 1u, extent.y > 1 ? extent.y - 1 : 1u);
    }
    static inline void removeInvalidPixel(unsigned char *data, const glm::uvec2&size){
        for (uint32_t i = 0; i < size.x * size.y; ++i){
            if(data[i * 4] == MAX_BYTE){
                data[i * 4 + 3] = 0;
            }
        }
    }
    static inline void convertFontToRGBA(const unsigned char *fontData, const glm::uvec2 &size, unsigned char *data){
        uint32_t width = size.x;
        uint32_t height = size.y;
        uint32_t totalPixels = width * height;
        memset(data, 0, totalPixels * 4);
        for (uint32_t i = 0; i < totalPixels; ++i) {
            uint32_t dstIndex = i * 4;
            data[dstIndex] = fontData[i];
            // data[dstIndex + 3] = MAX_BYTE;
        }
    }
    static inline void copy(const unsigned char *source, unsigned char *destination, const glm::uvec2&cutSize, const glm::uvec2& srcSize, const glm::uvec2& dstSize, const glm::uvec2& srcOffset, const glm::uvec2& destOffset) {
        const uint64_t channels = 4ull;
        if (srcSize.x == 0 || srcSize.y == 0 || dstSize.x == 0 || dstSize.y == 0) return;
        if (srcOffset.x >= srcSize.x || srcOffset.y >= srcSize.y) return;
        if (destOffset.x >= dstSize.x || destOffset.y >= dstSize.y) return;

        uint64_t srcTotalPixels = (uint64_t)srcSize.x * (uint64_t)srcSize.y;
        uint64_t dstTotalPixels = (uint64_t)dstSize.x * (uint64_t)dstSize.y;

        if (srcTotalPixels > SIZE_MAX / channels || dstTotalPixels > SIZE_MAX / channels) return;

        size_t srcTotalBytes = static_cast<size_t>(srcTotalPixels * channels);
        size_t dstTotalBytes = static_cast<size_t>(dstTotalPixels * channels);

        uint32_t effW = cutSize.x;
        uint32_t effH = cutSize.y;
        if (srcOffset.x + effW > srcSize.x) effW = srcSize.x - srcOffset.x;
        if (srcOffset.y + effH > srcSize.y) effH = srcSize.y - srcOffset.y;
        if (destOffset.x + effW > dstSize.x) effW = dstSize.x - destOffset.x;
        if (destOffset.y + effH > dstSize.y) effH = dstSize.y - destOffset.y;

        if (effW == 0 || effH == 0) return;

        for (uint32_t y = 0; y < effH; ++y) {
            for (uint32_t x = 0; x < effW; ++x) {
                size_t srcIndex = ROW_COLUMN_TO_INDEX((uint32_t)(y + srcOffset.y), (uint32_t)(x + srcOffset.x), srcSize.x) * channels;
                size_t dstIndex = ROW_COLUMN_TO_INDEX((uint32_t)(destOffset.y + y), (uint32_t)(destOffset.x + x), dstSize.x) * channels;
                
                if (srcIndex + 3 >= srcTotalBytes || dstIndex + 3 >= dstTotalBytes)continue;

                if (source[srcIndex + 3]) {
                    destination[dstIndex]     = source[srcIndex];
                    destination[dstIndex + 1] = source[srcIndex + 1];
                    destination[dstIndex + 2] = source[srcIndex + 2];
                    destination[dstIndex + 3] = MAX_BYTE;
                }
            }
        }
    }
};
#endif