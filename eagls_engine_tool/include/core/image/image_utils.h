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
 * @brief 图像信息
 */
struct EAGLS_IMAGE_API ImageInfo {
    int width;       // 宽度
    int height;      // 高度
    int bpp;         // 每像素位数
    int imageSize;   // 图像大小
};

/**
 * @brief 图像工具类
 */
class EAGLS_IMAGE_API ImageUtils {
public:
    /**
     * @brief 获取BMP图像信息
     * @param filename 文件名
     * @return 图像信息
     */
    static ImageInfo getBmpInfo(const std::string& filename);
    
    /**
     * @brief 获取GR图像信息
     * @param filename 文件名
     * @return 图像信息
     */
    static ImageInfo getGrInfo(const std::string& filename);
    
    /**
     * @brief PNG转BMP
     * @param pngFilename PNG文件名
     * @param bmpFilename BMP文件名
     * @return 是否成功
     */
    static bool pngToBmp(const std::string& pngFilename, const std::string& bmpFilename);
    
    /**
     * @brief BMP转PNG
     * @param bmpFilename BMP文件名
     * @param pngFilename PNG文件名
     * @return 是否成功
     */
    static bool bmpToPng(const std::string& bmpFilename, const std::string& pngFilename);
    
    /**
     * @brief 批量PNG转BMP
     * @param inputDir 输入目录
     * @param outputDir 输出目录
     * @return 成功转换的文件数
     */
    static int batchPngToBmp(const std::string& inputDir, const std::string& outputDir);
    
    /**
     * @brief 批量BMP转PNG
     * @param inputDir 输入目录
     * @param outputDir 输出目录
     * @return 成功转换的文件数
     */
    static int batchBmpToPng(const std::string& inputDir, const std::string& outputDir);
};

} // namespace image
} // namespace eagls