#include "core/image/png_bmp_converter.h"
#include "core/file/file_utils.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstring>
#include <filesystem>
#include <png.h>
#include <vector>
#include <stdexcept>

// 禁用不安全函数警告
#ifdef _MSC_VER
    #define _CRT_SECURE_NO_WARNINGS
    #pragma warning(disable: 4996)  // 禁用fopen等不安全函数的警告
#endif

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

PngBmpConverter::PngBmpConverter() {
}

PngBmpConverter::~PngBmpConverter() {
}

bool PngBmpConverter::pngToBmp(const std::string& pngFilename, const std::string& bmpFilename, int bpp) {
    // 打开PNG文件
    FILE* fp = fopen(pngFilename.c_str(), "rb");
    if (!fp) {
        std::cerr << "Error: Cannot open PNG file: " << pngFilename << std::endl;
        return false;
    }

    // 创建PNG读取结构
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) {
        fclose(fp);
        std::cerr << "Error: Cannot create PNG read struct" << std::endl;
        return false;
    }

    // 创建PNG信息结构
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, nullptr, nullptr);
        fclose(fp);
        std::cerr << "Error: Cannot create PNG info struct" << std::endl;
        return false;
    }

    // 设置错误处理
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        fclose(fp);
        std::cerr << "Error: Error during PNG reading" << std::endl;
        return false;
    }

    // 初始化PNG I/O
    png_init_io(png_ptr, fp);

    // 读取PNG信息
    png_read_info(png_ptr, info_ptr);

    // 获取PNG图像信息
    png_uint_32 width, height;
    int bit_depth, color_type, interlace_type;
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, nullptr, nullptr);

    // 转换PNG格式为RGB或RGBA
    if (color_type == PNG_COLOR_TYPE_PALETTE) {
        png_set_palette_to_rgb(png_ptr);
    }

    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
        png_set_expand_gray_1_2_4_to_8(png_ptr);
    }

    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(png_ptr);
    }

    if (bit_depth == 16) {
        png_set_strip_16(png_ptr);
    }

    if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
        png_set_gray_to_rgb(png_ptr);
    }

    // 更新PNG信息
    png_read_update_info(png_ptr, info_ptr);

    // 获取更新后的PNG图像信息
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, nullptr, nullptr);

    // 计算每行字节数
    png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);

    // 分配内存存储PNG像素数据
    std::vector<png_byte> png_data(rowbytes * height);
    std::vector<png_bytep> row_pointers(height);

    // 设置行指针
    for (png_uint_32 i = 0; i < height; ++i) {
        row_pointers[i] = &png_data[i * rowbytes];
    }

    // 读取PNG像素数据
    png_read_image(png_ptr, row_pointers.data());

    // 读取PNG结束信息
    png_read_end(png_ptr, nullptr);

    // 清理PNG结构
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    fclose(fp);

    // 创建BMP文件头和信息头
    BitmapFileHeader fileHeader = {0};
    BitmapInfoHeader infoHeader = {0};

    // 计算BMP每行字节数（需要4字节对齐）
    int bmpRowBytes = ((width * (bpp / 8) + 3) / 4) * 4;

    // 计算BMP像素数据大小
    int bmpDataSize = bmpRowBytes * height;

    // 计算BMP调色板大小
    int paletteSize = 0;
    if (bpp == 8) {
        paletteSize = 256 * 4;  // 256色调色板，每个颜色4字节
    }

    // 计算BMP文件大小
    int bmpFileSize = sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader) + paletteSize + bmpDataSize;

    // 填充BMP文件头
    fileHeader.bfType = 0x4D42;  // "BM"
    fileHeader.bfSize = bmpFileSize;
    fileHeader.bfReserved1 = 0;
    fileHeader.bfReserved2 = 0;
    fileHeader.bfOffBits = sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader) + paletteSize;

    // 填充BMP信息头
    infoHeader.biSize = sizeof(BitmapInfoHeader);
    infoHeader.biWidth = width;
    infoHeader.biHeight = height;  // 正值表示图像是上下颠倒的
    infoHeader.biPlanes = 1;
    infoHeader.biBitCount = bpp;
    infoHeader.biCompression = 0;  // BI_RGB
    infoHeader.biSizeImage = bmpDataSize;
    infoHeader.biXPelsPerMeter = 0;
    infoHeader.biYPelsPerMeter = 0;
    infoHeader.biClrUsed = (bpp == 8) ? 256 : 0;
    infoHeader.biClrImportant = 0;

    // 创建BMP调色板
    std::vector<uint8_t> palette;
    if (bpp == 8) {
        palette = createBmpPalette(bpp);
    }

    // 创建BMP像素数据
    std::vector<uint8_t> bmpData(bmpDataSize);

    // 转换PNG像素数据为BMP格式
    for (png_uint_32 y = 0; y < height; ++y) {
        for (png_uint_32 x = 0; x < width; ++x) {
            // PNG数据索引
            size_t pngIndex = y * rowbytes + x * (color_type == PNG_COLOR_TYPE_RGB_ALPHA ? 4 : 3);

            // BMP数据索引（BMP图像是上下颠倒的）
            size_t bmpIndex = (height - 1 - y) * bmpRowBytes + x * (bpp / 8);

            if (bpp == 24 || bpp == 32) {
                // 24位或32位BMP
                if (color_type == PNG_COLOR_TYPE_RGB_ALPHA) {
                    // RGBA格式
                    bmpData[bmpIndex + 0] = png_data[pngIndex + 2];  // B
                    bmpData[bmpIndex + 1] = png_data[pngIndex + 1];  // G
                    bmpData[bmpIndex + 2] = png_data[pngIndex + 0];  // R
                    if (bpp == 32) {
                        bmpData[bmpIndex + 3] = png_data[pngIndex + 3];  // A
                    }
                } else {
                    // RGB格式
                    bmpData[bmpIndex + 0] = png_data[pngIndex + 2];  // B
                    bmpData[bmpIndex + 1] = png_data[pngIndex + 1];  // G
                    bmpData[bmpIndex + 2] = png_data[pngIndex + 0];  // R
                    if (bpp == 32) {
                        bmpData[bmpIndex + 3] = 255;  // A
                    }
                }
            } else if (bpp == 8) {
                // 8位BMP（灰度）
                if (color_type == PNG_COLOR_TYPE_RGB_ALPHA) {
                    // 计算灰度值
                    bmpData[bmpIndex] = static_cast<uint8_t>(
                        0.299 * png_data[pngIndex + 0] +
                        0.587 * png_data[pngIndex + 1] +
                        0.114 * png_data[pngIndex + 2]
                    );
                } else {
                    // 计算灰度值
                    bmpData[bmpIndex] = static_cast<uint8_t>(
                        0.299 * png_data[pngIndex + 0] +
                        0.587 * png_data[pngIndex + 1] +
                        0.114 * png_data[pngIndex + 2]
                    );
                }
            }
        }
    }

    // 写入BMP文件
    std::ofstream outFile(bmpFilename, std::ios::binary);
    if (!outFile) {
        std::cerr << "Error: Cannot open output file: " << bmpFilename << std::endl;
        return false;
    }

    // 写入BMP文件头
    outFile.write(reinterpret_cast<const char*>(&fileHeader), sizeof(fileHeader));

    // 写入BMP信息头
    outFile.write(reinterpret_cast<const char*>(&infoHeader), sizeof(infoHeader));

    // 写入BMP调色板
    if (!palette.empty()) {
        outFile.write(reinterpret_cast<const char*>(palette.data()), palette.size());
    }

    // 写入BMP像素数据
    outFile.write(reinterpret_cast<const char*>(bmpData.data()), bmpData.size());

    outFile.close();

    return true;
}

bool PngBmpConverter::bmpToPng(const std::string& bmpFilename, const std::string& pngFilename) {
    // 读取BMP文件
    std::ifstream inFile(bmpFilename, std::ios::binary);
    if (!inFile) {
        std::cerr << "Error: Cannot open BMP file: " << bmpFilename << std::endl;
        return false;
    }

    // 读取BMP文件头
    BitmapFileHeader fileHeader;
    inFile.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    if (!inFile) {
        std::cerr << "Error: Failed to read BMP file header: " << bmpFilename << std::endl;
        return false;
    }

    // 检查BMP签名
    if (fileHeader.bfType != 0x4D42) {  // "BM"
        std::cerr << "Error: Invalid BMP signature: " << bmpFilename << std::endl;
        return false;
    }

    // 读取BMP信息头
    BitmapInfoHeader infoHeader;
    inFile.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));
    if (!inFile) {
        std::cerr << "Error: Failed to read BMP info header: " << bmpFilename << std::endl;
        return false;
    }

    // 检查BMP格式
    if (infoHeader.biCompression != 0) {  // BI_RGB
        std::cerr << "Error: Compressed BMP not supported: " << bmpFilename << std::endl;
        return false;
    }

    // 获取BMP信息
    int width = infoHeader.biWidth;
    int height = infoHeader.biHeight;
    int bpp = infoHeader.biBitCount;

    // 检查BMP位深度
    if (bpp != 24 && bpp != 32 && bpp != 8) {
        std::cerr << "Error: Unsupported BMP bit depth: " << bpp << std::endl;
        return false;
    }

    // 计算BMP每行字节数（需要4字节对齐）
    int bmpRowBytes = ((width * (bpp / 8) + 3) / 4) * 4;

    // 跳过调色板（如果有）
    if (bpp == 8) {
        int paletteSize = (fileHeader.bfOffBits - sizeof(BitmapFileHeader) - sizeof(BitmapInfoHeader));
        inFile.seekg(paletteSize, std::ios::cur);
    } else {
        inFile.seekg(fileHeader.bfOffBits, std::ios::beg);
    }

    // 读取BMP像素数据
    std::vector<uint8_t> bmpData(bmpRowBytes * std::abs(height));
    inFile.read(reinterpret_cast<char*>(bmpData.data()), bmpData.size());
    if (!inFile) {
        std::cerr << "Error: Failed to read BMP pixel data: " << bmpFilename << std::endl;
        return false;
    }

    inFile.close();

    // 创建PNG文件
    FILE* fp = fopen(pngFilename.c_str(), "wb");
    if (!fp) {
        std::cerr << "Error: Cannot open PNG file for writing: " << pngFilename << std::endl;
        return false;
    }

    // 创建PNG写入结构
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) {
        fclose(fp);
        std::cerr << "Error: Cannot create PNG write struct" << std::endl;
        return false;
    }

    // 创建PNG信息结构
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_write_struct(&png_ptr, nullptr);
        fclose(fp);
        std::cerr << "Error: Cannot create PNG info struct" << std::endl;
        return false;
    }

    // 设置错误处理
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(fp);
        std::cerr << "Error: Error during PNG writing" << std::endl;
        return false;
    }

    // 初始化PNG I/O
    png_init_io(png_ptr, fp);

    // 设置PNG图像信息
    int color_type = (bpp == 32) ? PNG_COLOR_TYPE_RGB_ALPHA : PNG_COLOR_TYPE_RGB;
    png_set_IHDR(png_ptr, info_ptr, width, std::abs(height), 8, color_type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    // 写入PNG信息
    png_write_info(png_ptr, info_ptr);

    // 创建PNG行指针
    std::vector<png_bytep> row_pointers(std::abs(height));

    // 分配内存存储PNG像素数据
    std::vector<png_byte> png_data(width * std::abs(height) * (bpp == 32 ? 4 : 3));

    // 转换BMP像素数据为PNG格式
    for (int y = 0; y < std::abs(height); ++y) {
        // 设置行指针
        row_pointers[y] = &png_data[y * width * (bpp == 32 ? 4 : 3)];

        for (int x = 0; x < width; ++x) {
            // BMP数据索引（BMP图像是上下颠倒的，除非height为负值）
            int bmpY = (height > 0) ? (std::abs(height) - 1 - y) : y;
            size_t bmpIndex = bmpY * bmpRowBytes + x * (bpp / 8);

            // PNG数据索引
            size_t pngIndex = x * (bpp == 32 ? 4 : 3);

            if (bpp == 24 || bpp == 32) {
                // 24位或32位BMP
                row_pointers[y][pngIndex + 0] = bmpData[bmpIndex + 2];  // R
                row_pointers[y][pngIndex + 1] = bmpData[bmpIndex + 1];  // G
                row_pointers[y][pngIndex + 2] = bmpData[bmpIndex + 0];  // B
                if (bpp == 32) {
                    row_pointers[y][pngIndex + 3] = bmpData[bmpIndex + 3];  // A
                }
            } else if (bpp == 8) {
                // 8位BMP（灰度）
                row_pointers[y][pngIndex + 0] = bmpData[bmpIndex];  // R
                row_pointers[y][pngIndex + 1] = bmpData[bmpIndex];  // G
                row_pointers[y][pngIndex + 2] = bmpData[bmpIndex];  // B
            }
        }
    }

    // 写入PNG像素数据
    png_write_image(png_ptr, row_pointers.data());

    // 写入PNG结束信息
    png_write_end(png_ptr, nullptr);

    // 清理PNG结构
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);

    return true;
}

int PngBmpConverter::batchPngToBmp(const std::string& inputDir, const std::string& outputDir, int bpp) {
    // 确保输出目录存在
    if (!file::FileUtils::createDirectory(outputDir)) {
        std::cerr << "Error: Failed to create output directory: " << outputDir << std::endl;
        return 0;
    }

    int count = 0;

    try {
        // 遍历输入目录
        for (const auto& entry : fs::directory_iterator(inputDir)) {
            if (!entry.is_regular_file()) {
                continue;
            }

            // 检查是否为PNG文件
            std::string extension = entry.path().extension().string();
            if (extension != ".png" && extension != ".PNG") {
                continue;
            }

            // 构造输出文件名
            std::string outputFilename = file::FileUtils::combinePath(
                outputDir,
                entry.path().stem().string() + ".bmp"
            );

            // 转换文件
            if (pngToBmp(entry.path().string(), outputFilename, bpp)) {
                count++;
                std::cout << "Converted: " << entry.path().string() << " -> " << outputFilename << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return count;
}

int PngBmpConverter::batchBmpToPng(const std::string& inputDir, const std::string& outputDir) {
    // 确保输出目录存在
    if (!file::FileUtils::createDirectory(outputDir)) {
        std::cerr << "Error: Failed to create output directory: " << outputDir << std::endl;
        return 0;
    }

    int count = 0;

    try {
        // 遍历输入目录
        for (const auto& entry : fs::directory_iterator(inputDir)) {
            if (!entry.is_regular_file()) {
                continue;
            }

            // 检查是否为BMP文件
            std::string extension = entry.path().extension().string();
            if (extension != ".bmp" && extension != ".BMP") {
                continue;
            }

            // 构造输出文件名
            std::string outputFilename = file::FileUtils::combinePath(
                outputDir,
                entry.path().stem().string() + ".png"
            );

            // 转换文件
            if (bmpToPng(entry.path().string(), outputFilename)) {
                count++;
                std::cout << "Converted: " << entry.path().string() << " -> " << outputFilename << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return count;
}

bool PngBmpConverter::pngToBmp8(const std::string& pngFilename, const std::string& bmpFilename) {
    // 这是一个特殊的转换，将PNG转换为8位BMP
    return pngToBmp(pngFilename, bmpFilename, 8);
}

bool PngBmpConverter::bmp8ToPng(const std::string& bmpFilename, const std::string& pngFilename) {
    // 读取BMP文件
    std::vector<uint8_t> data = file::FileUtils::readFile(bmpFilename);
    if (data.size() < sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader)) {
        std::cerr << "Error: Invalid BMP file size: " << bmpFilename << std::endl;
        return false;
    }

    // 解析BMP头
    BitmapFileHeader* fileHeader = reinterpret_cast<BitmapFileHeader*>(data.data());
    BitmapInfoHeader* infoHeader = reinterpret_cast<BitmapInfoHeader*>(data.data() + sizeof(BitmapFileHeader));

    // 检查BMP头
    if (fileHeader->bfType != 0x4D42) {  // "BM"
        std::cerr << "Error: Invalid BMP signature: " << bmpFilename << std::endl;
        return false;
    }

    // 检查是否为8位BMP
    if (infoHeader->biBitCount != 8) {
        std::cerr << "Error: Not an 8-bit BMP file: " << bmpFilename << std::endl;
        return false;
    }

    // 调用通用的BMP到PNG转换函数
    return bmpToPng(bmpFilename, pngFilename);
}

int PngBmpConverter::batchPngToBmp8(const std::string& inputDir, const std::string& outputDir) {
    return batchPngToBmp(inputDir, outputDir, 8);
}

int PngBmpConverter::batchBmp8ToPng(const std::string& inputDir, const std::string& outputDir) {
    // 确保输出目录存在
    if (!file::FileUtils::createDirectory(outputDir)) {
        std::cerr << "Error: Failed to create output directory: " << outputDir << std::endl;
        return 0;
    }

    int count = 0;

    try {
        // 遍历输入目录
        for (const auto& entry : fs::directory_iterator(inputDir)) {
            if (!entry.is_regular_file()) {
                continue;
            }

            // 检查是否为BMP文件
            std::string extension = entry.path().extension().string();
            if (extension != ".bmp" && extension != ".BMP") {
                continue;
            }

            // 读取BMP文件头
            std::ifstream file(entry.path(), std::ios::binary);
            if (!file) {
                continue;
            }

            BitmapFileHeader fileHeader;
            BitmapInfoHeader infoHeader;

            file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
            file.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));
            file.close();

            // 检查是否为8位BMP
            if (infoHeader.biBitCount != 8) {
                continue;
            }

            // 构造输出文件名
            std::string outputFilename = file::FileUtils::combinePath(
                outputDir,
                entry.path().stem().string() + ".png"
            );

            // 转换文件
            if (bmp8ToPng(entry.path().string(), outputFilename)) {
                count++;
                std::cout << "Converted: " << entry.path().string() << " -> " << outputFilename << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return count;
}

std::vector<uint8_t> PngBmpConverter::createBmpHeader(int width, int height, int bpp) {
    // 计算每行字节数（需要4字节对齐）
    int bytesPerRow = ((width * bpp + 31) / 32) * 4;

    // 计算像素数据大小
    int imageSize = bytesPerRow * height;

    // 计算调色板大小
    int paletteSize = 0;
    if (bpp == 8) {
        paletteSize = 256 * 4;  // 256色调色板，每个颜色4字节
    }

    // 计算文件大小
    int fileSize = sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader) + paletteSize + imageSize;

    // 创建BMP头
    std::vector<uint8_t> header(sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader));

    // 填充文件头
    BitmapFileHeader* fileHeader = reinterpret_cast<BitmapFileHeader*>(header.data());
    fileHeader->bfType = 0x4D42;  // "BM"
    fileHeader->bfSize = fileSize;
    fileHeader->bfReserved1 = 0;
    fileHeader->bfReserved2 = 0;
    fileHeader->bfOffBits = sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader) + paletteSize;

    // 填充信息头
    BitmapInfoHeader* infoHeader = reinterpret_cast<BitmapInfoHeader*>(header.data() + sizeof(BitmapFileHeader));
    infoHeader->biSize = sizeof(BitmapInfoHeader);
    infoHeader->biWidth = width;
    infoHeader->biHeight = height;
    infoHeader->biPlanes = 1;
    infoHeader->biBitCount = bpp;
    infoHeader->biCompression = 0;  // BI_RGB
    infoHeader->biSizeImage = imageSize;
    infoHeader->biXPelsPerMeter = 0;
    infoHeader->biYPelsPerMeter = 0;
    infoHeader->biClrUsed = (bpp == 8) ? 256 : 0;
    infoHeader->biClrImportant = 0;

    return header;
}

std::vector<uint8_t> PngBmpConverter::createBmpPalette(int bpp) {
    // 只有8位BMP需要调色板
    if (bpp != 8) {
        return {};
    }

    // 创建256色调色板
    std::vector<uint8_t> palette(256 * 4);

    // 填充调色板
    for (int i = 0; i < 256; ++i) {
        palette[i * 4 + 0] = i;  // 蓝色分量
        palette[i * 4 + 1] = i;  // 绿色分量
        palette[i * 4 + 2] = i;  // 红色分量
        palette[i * 4 + 3] = 0;  // 保留
    }

    return palette;
}

} // namespace image
} // namespace eagls