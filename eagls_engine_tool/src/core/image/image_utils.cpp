#include "core/image/image_utils.h"
#include "core/image/png_bmp_converter.h"
#include "core/file/file_utils.h"
#include "core/compression/lzss.h"
#include "core/encryption/eagls_encryption.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstring>
#include <filesystem>

namespace fs = std::filesystem;

namespace eagls {
namespace image {

// BMP文件头结构
#pragma pack(push, 1)
struct BitmapFileHeader {
    uint16_t bfType;       // 文件类型，必须为"BM"
    uint32_t bfSize;       // 文件大小
    uint16_t bfReserved1;  // 保留，必须为0
    uint16_t bfReserved2;  // 保留，必须为0
    uint32_t bfOffBits;    // 从文件头到实际位图数据的偏移量
};

struct BitmapInfoHeader {
    uint32_t biSize;          // 结构体大小
    int32_t  biWidth;         // 图像宽度
    int32_t  biHeight;        // 图像高度
    uint16_t biPlanes;        // 平面数，必须为1
    uint16_t biBitCount;      // 每像素位数
    uint32_t biCompression;   // 压缩类型
    uint32_t biSizeImage;     // 图像大小
    int32_t  biXPelsPerMeter; // 水平分辨率
    int32_t  biYPelsPerMeter; // 垂直分辨率
    uint32_t biClrUsed;       // 使用的颜色数
    uint32_t biClrImportant;  // 重要的颜色数
};
#pragma pack(pop)

ImageInfo ImageUtils::getBmpInfo(const std::string& filename) {
    ImageInfo info = {0};

    // 读取BMP文件头
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error: Cannot open BMP file: " << filename << std::endl;
        return info;
    }

    BitmapFileHeader fileHeader;
    BitmapInfoHeader infoHeader;

    // 读取文件头
    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    if (!file) {
        std::cerr << "Error: Failed to read BMP file header: " << filename << std::endl;
        return info;
    }

    // 检查BMP签名
    if (fileHeader.bfType != 0x4D42) {  // "BM"
        std::cerr << "Error: Invalid BMP signature: " << filename << std::endl;
        return info;
    }

    // 读取信息头
    file.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));
    if (!file) {
        std::cerr << "Error: Failed to read BMP info header: " << filename << std::endl;
        return info;
    }

    // 填充图像信息
    info.width = infoHeader.biWidth;
    info.height = infoHeader.biHeight;
    info.bpp = infoHeader.biBitCount;
    info.imageSize = infoHeader.biSizeImage;

    return info;
}

ImageInfo ImageUtils::getGrInfo(const std::string& filename) {
    ImageInfo info = {0};

    // 读取GR文件
    std::vector<uint8_t> data = file::FileUtils::readFile(filename);
    if (data.empty()) {
        std::cerr << "Error: Failed to read GR file: " << filename << std::endl;
        return info;
    }

    // 解密数据
    encryption::LehmerEncryption enc;
    std::vector<uint8_t> compressedData = enc.decrypt(data);

    // 解压数据
    compression::LZSS lzss(7);  // 使用7位前向缓冲区
    std::vector<uint8_t> bmpData = lzss.decode(compressedData);

    // 检查解压后的数据是否为有效的BMP
    if (bmpData.size() < sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader)) {
        std::cerr << "Error: Invalid BMP data after decompression" << std::endl;
        return info;
    }

    // 从解压后的数据中提取BMP信息
    BitmapFileHeader* fileHeader = reinterpret_cast<BitmapFileHeader*>(bmpData.data());
    BitmapInfoHeader* infoHeader = reinterpret_cast<BitmapInfoHeader*>(bmpData.data() + sizeof(BitmapFileHeader));

    // 检查BMP头
    if (fileHeader->bfType != 0x4D42) {  // "BM"
        std::cerr << "Error: Invalid BMP signature" << std::endl;
        return info;
    }

    // 填充图像信息
    info.width = infoHeader->biWidth;
    info.height = infoHeader->biHeight;
    info.bpp = infoHeader->biBitCount;
    info.imageSize = infoHeader->biSizeImage;

    return info;
}

bool ImageUtils::pngToBmp(const std::string& pngFilename, const std::string& bmpFilename) {
    // 使用PngBmpConverter类来实现PNG到BMP的转换
    // 使用默认的24位色深
    PngBmpConverter converter;
    return converter.pngToBmp(pngFilename, bmpFilename, 24);
}

bool ImageUtils::bmpToPng(const std::string& bmpFilename, const std::string& pngFilename) {
    // 使用PngBmpConverter类来实现BMP到PNG的转换
    PngBmpConverter converter;
    return converter.bmpToPng(bmpFilename, pngFilename);
}

int ImageUtils::batchPngToBmp(const std::string& inputDir, const std::string& outputDir) {
    // 使用PngBmpConverter类来实现批量PNG到BMP的转换
    // 使用默认的24位色深
    PngBmpConverter converter;
    return converter.batchPngToBmp(inputDir, outputDir, 24);
}

int ImageUtils::batchBmpToPng(const std::string& inputDir, const std::string& outputDir) {
    // 使用PngBmpConverter类来实现批量BMP到PNG的转换
    PngBmpConverter converter;
    return converter.batchBmpToPng(inputDir, outputDir);
}

} // namespace image
} // namespace eagls