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
 * @brief BMP/GR图像转换器
 */
class EAGLS_IMAGE_API BmpGrConverter {
public:
    /**
     * @brief 构造函数
     */
    BmpGrConverter();
    
    /**
     * @brief 析构函数
     */
    ~BmpGrConverter();
    
    /**
     * @brief BMP转GR
     * @param bmpFilename BMP文件名
     * @param grFilename GR文件名
     * @return 是否成功
     */
    bool bmpToGr(const std::string& bmpFilename, const std::string& grFilename);
    
    /**
     * @brief GR转BMP
     * @param grFilename GR文件名
     * @param bmpFilename BMP文件名
     * @return 是否成功
     */
    bool grToBmp(const std::string& grFilename, const std::string& bmpFilename);
    
    /**
     * @brief 批量BMP转GR
     * @param inputDir 输入目录
     * @param outputDir 输出目录
     * @return 成功转换的文件数
     */
    int batchBmpToGr(const std::string& inputDir, const std::string& outputDir);
    
    /**
     * @brief 批量GR转BMP
     * @param inputDir 输入目录
     * @param outputDir 输出目录
     * @return 成功转换的文件数
     */
    int batchGrToBmp(const std::string& inputDir, const std::string& outputDir);

private:
    /**
     * @brief 读取BMP文件
     * @param filename 文件名
     * @param width 宽度
     * @param height 高度
     * @param bpp 每像素位数
     * @return 图像数据
     */
    std::vector<uint8_t> readBmp(const std::string& filename, int& width, int& height, int& bpp);
    
    /**
     * @brief 写入BMP文件
     * @param filename 文件名
     * @param data 图像数据
     * @param width 宽度
     * @param height 高度
     * @param bpp 每像素位数
     * @return 是否成功
     */
    bool writeBmp(const std::string& filename, const std::vector<uint8_t>& data, int width, int height, int bpp);
    
    /**
     * @brief 读取GR文件
     * @param filename 文件名
     * @return 图像数据
     */
    std::vector<uint8_t> readGr(const std::string& filename);
    
    /**
     * @brief 写入GR文件
     * @param filename 文件名
     * @param data 图像数据
     * @return 是否成功
     */
    bool writeGr(const std::string& filename, const std::vector<uint8_t>& data);
};

} // namespace image
} // namespace eagls