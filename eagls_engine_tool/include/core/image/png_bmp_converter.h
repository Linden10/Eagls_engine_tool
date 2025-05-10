#pragma once

#include <string>
#include <vector>
#include <cstdint>

// DLL导出宏定义
#ifdef _WIN32
    #ifdef EAGLS_IMAGE_EXPORTS
        #define EAGLS_IMAGE_API __declspec(dllexport)
    #else
        #define EAGLS_IMAGE_API __declspec(dllimport)
    #endif
#else
    #define EAGLS_IMAGE_API
#endif

namespace eagls {
namespace image {

/**
 * @brief PNG/BMP转换器
 */
class EAGLS_IMAGE_API PngBmpConverter {
public:
    /**
     * @brief 构造函数
     */
    PngBmpConverter();
    
    /**
     * @brief 析构函数
     */
    ~PngBmpConverter();
    
    /**
     * @brief PNG转BMP
     * @param pngFilename PNG文件名
     * @param bmpFilename BMP文件名
     * @param bpp 每像素位数（默认为24）
     * @return 是否成功
     */
    bool pngToBmp(const std::string& pngFilename, const std::string& bmpFilename, int bpp = 24);
    
    /**
     * @brief BMP转PNG
     * @param bmpFilename BMP文件名
     * @param pngFilename PNG文件名
     * @return 是否成功
     */
    bool bmpToPng(const std::string& bmpFilename, const std::string& pngFilename);
    
    /**
     * @brief 批量PNG转BMP
     * @param inputDir 输入目录
     * @param outputDir 输出目录
     * @param bpp 每像素位数（默认为24）
     * @return 成功转换的文件数
     */
    int batchPngToBmp(const std::string& inputDir, const std::string& outputDir, int bpp = 24);
    
    /**
     * @brief 批量BMP转PNG
     * @param inputDir 输入目录
     * @param outputDir 输出目录
     * @return 成功转换的文件数
     */
    int batchBmpToPng(const std::string& inputDir, const std::string& outputDir);
    
    /**
     * @brief PNG转BMP8
     * @param pngFilename PNG文件名
     * @param bmpFilename BMP文件名
     * @return 是否成功
     */
    bool pngToBmp8(const std::string& pngFilename, const std::string& bmpFilename);
    
    /**
     * @brief BMP8转PNG
     * @param bmpFilename BMP文件名
     * @param pngFilename PNG文件名
     * @return 是否成功
     */
    bool bmp8ToPng(const std::string& bmpFilename, const std::string& pngFilename);
    
    /**
     * @brief 批量PNG转BMP8
     * @param inputDir 输入目录
     * @param outputDir 输出目录
     * @return 成功转换的文件数
     */
    int batchPngToBmp8(const std::string& inputDir, const std::string& outputDir);
    
    /**
     * @brief 批量BMP8转PNG
     * @param inputDir 输入目录
     * @param outputDir 输出目录
     * @return 成功转换的文件数
     */
    int batchBmp8ToPng(const std::string& inputDir, const std::string& outputDir);

private:
    /**
     * @brief 创建BMP文件头
     * @param width 宽度
     * @param height 高度
     * @param bpp 每像素位数
     * @return BMP文件头
     */
    std::vector<uint8_t> createBmpHeader(int width, int height, int bpp);
    
    /**
     * @brief 创建BMP调色板
     * @param bpp 每像素位数
     * @return BMP调色板
     */
    std::vector<uint8_t> createBmpPalette(int bpp);
};

} // namespace image
} // namespace eagls