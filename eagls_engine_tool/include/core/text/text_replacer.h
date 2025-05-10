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
 * @brief 文本替换器
 */
class EAGLS_TEXT_API TextReplacer {
public:
    /**
     * @brief 构造函数
     */
    TextReplacer();
    
    /**
     * @brief 析构函数
     */
    ~TextReplacer();
    
    /**
     * @brief 替换文件中的文本
     * @param filename 文件名
     * @param textFilename 文本文件名
     * @param outputFilename 输出文件名
     * @return 是否成功
     */
    bool replaceText(const std::string& filename, const std::string& textFilename, const std::string& outputFilename);
    
    /**
     * @brief 批量替换文本
     * @param inputDir 输入目录
     * @param textDir 文本目录
     * @param outputDir 输出目录
     * @return 成功替换的文件数
     */
    int batchReplaceText(const std::string& inputDir, const std::string& textDir, const std::string& outputDir);
    
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
};

} // namespace text
} // namespace eagls