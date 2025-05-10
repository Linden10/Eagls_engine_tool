#pragma once

#include <string>
#include <vector>
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
 * @brief 文件工具类
 */
class EAGLS_FILE_API FileUtils {
public:
    /**
     * @brief 读取文件
     * @param filename 文件名
     * @return 文件内容
     */
    static std::vector<uint8_t> readFile(const std::string& filename);
    
    /**
     * @brief 写入文件
     * @param filename 文件名
     * @param data 文件内容
     * @return 是否成功
     */
    static bool writeFile(const std::string& filename, const std::vector<uint8_t>& data);
    
    /**
     * @brief 获取文件大小
     * @param filename 文件名
     * @return 文件大小
     */
    static size_t getFileSize(const std::string& filename);
    
    /**
     * @brief 检查文件是否存在
     * @param filename 文件名
     * @return 是否存在
     */
    static bool fileExists(const std::string& filename);
    
    /**
     * @brief 创建目录
     * @param path 目录路径
     * @return 是否成功
     */
    static bool createDirectory(const std::string& path);
    
    /**
     * @brief 获取目录中的文件列表
     * @param path 目录路径
     * @param recursive 是否递归
     * @return 文件列表
     */
    static std::vector<std::string> getFileList(const std::string& path, bool recursive = false);
    
    /**
     * @brief 获取文件扩展名
     * @param filename 文件名
     * @return 扩展名
     */
    static std::string getFileExtension(const std::string& filename);
    
    /**
     * @brief 获取文件名（不含路径和扩展名）
     * @param filename 文件名
     * @return 文件名
     */
    static std::string getFileName(const std::string& filename);
    
    /**
     * @brief 获取文件路径
     * @param filename 文件名
     * @return 文件路径
     */
    static std::string getFilePath(const std::string& filename);
    
    /**
     * @brief 合并路径
     * @param path1 路径1
     * @param path2 路径2
     * @return 合并后的路径
     */
    static std::string combinePath(const std::string& path1, const std::string& path2);
};

} // namespace file
} // namespace eagls