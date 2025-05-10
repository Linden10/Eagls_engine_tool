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
 * @brief PAK文件条目
 */
struct EAGLS_FILE_API PakEntry {
    std::string name;    // 文件名
    uint64_t offset;     // 文件偏移
    uint32_t size;       // 文件大小
    uint32_t flags;      // 文件标志
};

/**
 * @brief PAK文件处理类
 */
class EAGLS_FILE_API PakFile {
public:
    /**
     * @brief 构造函数
     */
    PakFile();
    
    /**
     * @brief 析构函数
     */
    ~PakFile();
    
    /**
     * @brief 打开PAK文件
     * @param pakFilename PAK文件名
     * @return 是否成功
     */
    bool open(const std::string& pakFilename);
    
    /**
     * @brief 关闭PAK文件
     */
    void close();
    
    /**
     * @brief 获取文件列表
     * @return 文件列表
     */
    std::vector<std::string> getFileList() const;
    
    /**
     * @brief 提取文件
     * @param filename 要提取的文件名
     * @param outputPath 输出路径
     * @param decrypt 是否解密
     * @return 是否成功
     */
    bool extractFile(const std::string& filename, const std::string& outputPath, bool decrypt = true);
    
    /**
     * @brief 提取所有文件
     * @param outputPath 输出路径
     * @param decrypt 是否解密
     * @return 是否成功
     */
    bool extractAllFiles(const std::string& outputPath, bool decrypt = true);
    
    /**
     * @brief 创建PAK文件
     * @param pakFilename PAK文件名
     * @param files 要添加的文件列表
     * @param encrypt 是否加密
     * @return 是否成功
     */
    bool create(const std::string& pakFilename, const std::vector<std::string>& files, bool encrypt = true);
    
    /**
     * @brief 添加文件
     * @param pakFilename PAK文件名
     * @param filename 要添加的文件名
     * @param encrypt 是否加密
     * @return 是否成功
     */
    bool addFile(const std::string& pakFilename, const std::string& filename, bool encrypt = true);

private:
    std::string m_pakFilename;                  // PAK文件名
    std::map<std::string, PakEntry> m_entries;  // 文件条目
    bool m_isOpen;                              // 是否已打开
    
    /**
     * @brief 读取索引文件
     * @param idxFilename 索引文件名
     * @return 是否成功
     */
    bool readIndex(const std::string& idxFilename);
    
    /**
     * @brief 写入索引文件
     * @param idxFilename 索引文件名
     * @param entries 文件条目
     * @return 是否成功
     */
    bool writeIndex(const std::string& idxFilename, const std::map<std::string, PakEntry>& entries);
};

} // namespace file
} // namespace eagls