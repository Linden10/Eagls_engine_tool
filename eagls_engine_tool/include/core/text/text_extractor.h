#pragma once

#include <string>
#include <vector>
#include <map>

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
 * @brief 文本提取器
 */
class EAGLS_TEXT_API TextExtractor {
public:
    /**
     * @brief 构造函数
     */
    TextExtractor();
    
    /**
     * @brief 析构函数
     */
    ~TextExtractor();
    
    /**
     * @brief 从文件中提取文本
     * @param filename 文件名
     * @param outputFilename 输出文件名
     * @return 是否成功
     */
    bool extractText(const std::string& filename, const std::string& outputFilename);
    
    /**
     * @brief 批量提取文本
     * @param inputDir 输入目录
     * @param outputDir 输出目录
     * @return 成功提取的文件数
     */
    int batchExtractText(const std::string& inputDir, const std::string& outputDir);
    
    /**
     * @brief 设置编码
     * @param encoding 编码
     */
    void setEncoding(const std::string& encoding);
    
    /**
     * @brief 获取编码
     * @return 编码
     */
    std::string getEncoding() const;

private:
    std::string m_encoding;  // 编码
    
    /**
     * @brief 检查字符串是否为纯ASCII
     * @param str 字符串
     * @return 是否为纯ASCII
     */
    bool isPureAscii(const std::string& str);
};

} // namespace text
} // namespace eagls