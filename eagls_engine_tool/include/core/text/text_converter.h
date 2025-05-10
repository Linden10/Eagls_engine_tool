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
 * @brief 文本转换器
 * 
 * 提供各种文本转换功能，包括二进制到文本的转换
 */
class EAGLS_TEXT_API TextConverter {
public:
    /**
     * @brief 构造函数
     */
    TextConverter();
    
    /**
     * @brief 析构函数
     */
    ~TextConverter();
    
    /**
     * @brief 将二进制文件转换为文本文件
     * @param inputFilename 输入文件名
     * @param outputFilename 输出文件名
     * @param encoding 编码（默认为GBK）
     * @param removeNulls 是否移除空字符（默认为true）
     * @return 是否成功
     */
    bool binaryToText(const std::string& inputFilename, const std::string& outputFilename, 
                      const std::string& encoding = "GBK", bool removeNulls = true);
    
    /**
     * @brief 将文本文件转换为二进制文件
     * @param inputFilename 输入文件名
     * @param outputFilename 输出文件名
     * @param encoding 编码（默认为GBK）
     * @return 是否成功
     */
    bool textToBinary(const std::string& inputFilename, const std::string& outputFilename, 
                      const std::string& encoding = "GBK");
    
    /**
     * @brief 批量将二进制文件转换为文本文件
     * @param inputDir 输入目录
     * @param outputDir 输出目录
     * @param filePattern 文件模式（默认为*.dat）
     * @param encoding 编码（默认为GBK）
     * @param removeNulls 是否移除空字符（默认为true）
     * @return 成功转换的文件数
     */
    int batchBinaryToText(const std::string& inputDir, const std::string& outputDir, 
                          const std::string& filePattern = "*.dat", 
                          const std::string& encoding = "GBK", bool removeNulls = true);
    
    /**
     * @brief 批量将文本文件转换为二进制文件
     * @param inputDir 输入目录
     * @param outputDir 输出目录
     * @param filePattern 文件模式（默认为*.txt）
     * @param encoding 编码（默认为GBK）
     * @return 成功转换的文件数
     */
    int batchTextToBinary(const std::string& inputDir, const std::string& outputDir, 
                          const std::string& filePattern = "*.txt", 
                          const std::string& encoding = "GBK");
};

} // namespace text
} // namespace eagls