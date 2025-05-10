#pragma once

#include <string>
#include <vector>

// DLL导出宏定义
#ifdef _WIN32
    #ifdef EAGLS_TEXT_EXPORTS
        #define EAGLS_TEXT_API __declspec(dllexport)
    #else
        #define EAGLS_TEXT_API __declspec(dllimport)
    #endif
#else
    #define EAGLS_TEXT_API
#endif

namespace eagls {
namespace text {

/**
 * @brief 编码转换器
 */
class EAGLS_TEXT_API EncodingConverter {
public:
    /**
     * @brief 构造函数
     */
    EncodingConverter();
    
    /**
     * @brief 析构函数
     */
    ~EncodingConverter();
    
    /**
     * @brief 转换编码
     * @param input 输入字符串
     * @param fromEncoding 源编码
     * @param toEncoding 目标编码
     * @return 转换后的字符串
     */
    std::string convert(const std::string& input, const std::string& fromEncoding, const std::string& toEncoding);
    
    /**
     * @brief 转换文件编码
     * @param inputFilename 输入文件名
     * @param outputFilename 输出文件名
     * @param fromEncoding 源编码
     * @param toEncoding 目标编码
     * @return 是否成功
     */
    bool convertFile(const std::string& inputFilename, const std::string& outputFilename, const std::string& fromEncoding, const std::string& toEncoding);
    
    /**
     * @brief 批量转换文件编码
     * @param inputDir 输入目录
     * @param outputDir 输出目录
     * @param fromEncoding 源编码
     * @param toEncoding 目标编码
     * @return 成功转换的文件数
     */
    int batchConvertFiles(const std::string& inputDir, const std::string& outputDir, const std::string& fromEncoding, const std::string& toEncoding);
    
    /**
     * @brief 检测文件编码
     * @param filename 文件名
     * @return 检测到的编码
     */
    std::string detectFileEncoding(const std::string& filename);
    
    /**
     * @brief 检测字符串编码
     * @param input 输入字符串
     * @return 检测到的编码
     */
    std::string detectStringEncoding(const std::string& input);
};

} // namespace text
} // namespace eagls