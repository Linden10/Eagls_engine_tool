#include "core/image/bmp_gr_converter.h"
#include "core/file/file_utils.h"
#include "core/compression/lzss.h"
#include "core/encryption/eagls_encryption.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstring>

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

BmpGrConverter::BmpGrConverter() {
}

BmpGrConverter::~BmpGrConverter() {
}

bool BmpGrConverter::bmpToGr(const std::string& bmpFilename, const std::string& grFilename) {
    // 读取BMP文件
    int width, height, bpp;
    std::vector<uint8_t> bmpData = readBmp(bmpFilename, width, height, bpp);
    if (bmpData.empty()) {
        std::cerr << "Error: Failed to read BMP file: " << bmpFilename << std::endl;
        return false;
    }
    
    // 压缩BMP数据
    compression::LZSS lzss(7);  // 使用7位前向缓冲区
    std::vector<uint8_t> compressedData = lzss.encode(bmpData);
    
    // 加密压缩后的数据
    encryption::LehmerEncryption enc;
    std::vector<uint8_t> encryptedData = enc.encrypt(compressedData);
    
    // 写入GR文件
    return writeGr(grFilename, encryptedData);
}

bool BmpGrConverter::grToBmp(const std::string& grFilename, const std::string& bmpFilename) {
    // 读取GR文件
    std::vector<uint8_t> encryptedData = readGr(grFilename);
    if (encryptedData.empty()) {
        std::cerr << "Error: Failed to read GR file: " << grFilename << std::endl;
        return false;
    }
    
    // 解密数据
    encryption::LehmerEncryption enc;
    std::vector<uint8_t> compressedData = enc.decrypt(encryptedData);
    
    // 解压数据
    compression::LZSS lzss(7);  // 使用7位前向缓冲区
    std::vector<uint8_t> bmpData = lzss.decode(compressedData);
    
    // 检查解压后的数据是否为有效的BMP
    if (bmpData.size() < sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader)) {
        std::cerr << "Error: Invalid BMP data after decompression" << std::endl;
        return false;
    }
    
    // 从解压后的数据中提取BMP信息
    BitmapFileHeader* fileHeader = reinterpret_cast<BitmapFileHeader*>(bmpData.data());
    BitmapInfoHeader* infoHeader = reinterpret_cast<BitmapInfoHeader*>(bmpData.data() + sizeof(BitmapFileHeader));
    
    // 检查BMP头
    if (fileHeader->bfType != 0x4D42) {  // "BM"
        std::cerr << "Error: Invalid BMP signature" << std::endl;
        return false;
    }
    
    // 写入BMP文件
    return file::FileUtils::writeFile(bmpFilename, bmpData);
}

int BmpGrConverter::batchBmpToGr(const std::string& inputDir, const std::string& outputDir) {
    // 确保输出目录存在
    if (!file::FileUtils::createDirectory(outputDir)) {
        std::cerr << "Error: Failed to create output directory: " << outputDir << std::endl;
        return 0;
    }
    
    int count = 0;
    
    // 获取输入目录中的所有BMP文件
    std::vector<std::string> files = file::FileUtils::getFileList(inputDir);
    
    // 处理每个文件
    for (const auto& file : files) {
        // 检查是否为BMP文件
        if (file::FileUtils::getFileExtension(file) != ".bmp") {
            continue;
        }
        
        // 构造输出文件名
        std::string outputFilename = file::FileUtils::combinePath(
            outputDir, 
            file::FileUtils::getFileName(file) + ".gr"
        );
        
        // 转换文件
        if (bmpToGr(file, outputFilename)) {
            count++;
            std::cout << "Converted: " << file << " -> " << outputFilename << std::endl;
        }
    }
    
    return count;
}

int BmpGrConverter::batchGrToBmp(const std::string& inputDir, const std::string& outputDir) {
    // 确保输出目录存在
    if (!file::FileUtils::createDirectory(outputDir)) {
        std::cerr << "Error: Failed to create output directory: " << outputDir << std::endl;
        return 0;
    }
    
    int count = 0;
    
    // 获取输入目录中的所有GR文件
    std::vector<std::string> files = file::FileUtils::getFileList(inputDir);
    
    // 处理每个文件
    for (const auto& file : files) {
        // 检查是否为GR文件
        if (file::FileUtils::getFileExtension(file) != ".gr") {
            continue;
        }
        
        // 构造输出文件名
        std::string outputFilename = file::FileUtils::combinePath(
            outputDir, 
            file::FileUtils::getFileName(file) + ".bmp"
        );
        
        // 转换文件
        if (grToBmp(file, outputFilename)) {
            count++;
            std::cout << "Converted: " << file << " -> " << outputFilename << std::endl;
        }
    }
    
    return count;
}

std::vector<uint8_t> BmpGrConverter::readBmp(const std::string& filename, int& width, int& height, int& bpp) {
    // 读取BMP文件
    std::vector<uint8_t> data = file::FileUtils::readFile(filename);
    if (data.size() < sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader)) {
        std::cerr << "Error: Invalid BMP file size: " << filename << std::endl;
        return {};
    }
    
    // 解析BMP头
    BitmapFileHeader* fileHeader = reinterpret_cast<BitmapFileHeader*>(data.data());
    BitmapInfoHeader* infoHeader = reinterpret_cast<BitmapInfoHeader*>(data.data() + sizeof(BitmapFileHeader));
    
    // 检查BMP头
    if (fileHeader->bfType != 0x4D42) {  // "BM"
        std::cerr << "Error: Invalid BMP signature: " << filename << std::endl;
        return {};
    }
    
    // 获取图像信息
    width = infoHeader->biWidth;
    height = infoHeader->biHeight;
    bpp = infoHeader->biBitCount / 8;
    
    return data;
}

bool BmpGrConverter::writeBmp(const std::string& filename, const std::vector<uint8_t>& data, int width, int height, int bpp) {
    // 检查数据大小
    if (data.size() < sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader)) {
        std::cerr << "Error: Invalid BMP data size" << std::endl;
        return false;
    }
    
    // 写入BMP文件
    return file::FileUtils::writeFile(filename, data);
}

std::vector<uint8_t> BmpGrConverter::readGr(const std::string& filename) {
    // 读取GR文件
    return file::FileUtils::readFile(filename);
}

bool BmpGrConverter::writeGr(const std::string& filename, const std::vector<uint8_t>& data) {
    // 写入GR文件
    return file::FileUtils::writeFile(filename, data);
}

} // namespace image
} // namespace eagls