#pragma once

#include <string>
#include <vector>
#include <map>
#include <cstdint>

// DLL导出宏定义
#ifdef _WIN32
    #ifdef EAGLS_FILE_EXPORTS
        #define EAGLS_FILE_API __declspec(dllexport)
    #else
        #define EAGLS_FILE_API __declspec(dllimport)
    #endif
#else
    #define EAGLS_FILE_API
#endif

namespace eagls {
namespace file {

/**
 * @brief DAT文件条目
 */
struct EAGLS_FILE_API DatEntry {
    std::string name;    // 段名
    uint32_t offset;     // 段偏移
    uint32_t size;       // 段大小
};

/**
 * @brief DAT文件处理类
 */
class EAGLS_FILE_API DatFile {
public:
    /**
     * @brief 构造函数
     */
    DatFile();
    
    /**
     * @brief 析构函数
     */
    ~DatFile();
    
    /**
     * @brief 打开DAT文件
     * @param filename DAT文件名
     * @param decrypt 是否解密
     * @return 是否成功
     */
    bool open(const std::string& filename, bool decrypt = true);
    
    /**
     * @brief 关闭DAT文件
     */
    void close();
    
    /**
     * @brief 获取段列表
     * @return 段列表
     */
    std::vector<std::string> getSectionList() const;
    
    /**
     * @brief 获取段数据
     * @param sectionName 段名
     * @return 段数据
     */
    std::vector<uint8_t> getSectionData(const std::string& sectionName) const;
    
    /**
     * @brief 提取所有段
     * @param outputPath 输出路径
     * @return 是否成功
     */
    bool extractAllSections(const std::string& outputPath) const;
    
    /**
     * @brief 提取文本
     * @param outputFilename 输出文件名
     * @return 是否成功
     */
    bool extractText(const std::string& outputFilename) const;
    
    /**
     * @brief 替换文本
     * @param textFilename 文本文件名
     * @param outputFilename 输出文件名
     * @return 是否成功
     */
    bool replaceText(const std::string& textFilename, const std::string& outputFilename);
    
    /**
     * @brief 创建DAT文件
     * @param filename DAT文件名
     * @param sections 段数据
     * @param encrypt 是否加密
     * @return 是否成功
     */
    bool create(const std::string& filename, const std::map<std::string, std::vector<uint8_t>>& sections, bool encrypt = true);
    
    /**
     * @brief 获取原始数据
     * @return 原始数据
     */
    const std::vector<uint8_t>& getRawData() const;

private:
    std::vector<uint8_t> m_data;                  // 文件数据
    std::map<std::string, DatEntry> m_sections;   // 段映射
    bool m_isOpen;                                // 是否已打开
    
    /**
     * @brief 解析段表
     * @return 是否成功
     */
    bool parseSectionTable();
    
    /**
     * @brief 检查字符串是否为纯ASCII
     * @param str 字符串
     * @return 是否为纯ASCII
     */
    bool isPureAscii(const std::string& str) const;
    
    /**
     * @brief 更新段表
     */
    void updateSectionTable();
};

} // namespace file
} // namespace eagls