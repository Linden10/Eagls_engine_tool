#pragma once

#include <vector>
#include <string>
#include <cstdint>

// DLL导出宏定义
#ifdef _WIN32
    #ifdef EAGLS_COMPRESSION_EXPORTS
        #define EAGLS_COMPRESSION_API __declspec(dllexport)
    #else
        #define EAGLS_COMPRESSION_API __declspec(dllimport)
    #endif
#else
    #define EAGLS_COMPRESSION_API
#endif

namespace eagls {
namespace compression {

/**
 * @brief LZSS压缩算法实现
 * 
 * 基于Python脚本中的LZSS算法重写的C++版本
 */
class EAGLS_COMPRESSION_API LZSS {
public:
    /**
     * @brief 构造函数
     * @param preBufSizeBits 前向缓冲区占用的比特位
     */
    explicit LZSS(int preBufSizeBits = 7);
    
    /**
     * @brief 压缩数据
     * @param data 要压缩的数据
     * @return 压缩后的数据
     */
    std::vector<uint8_t> encode(const std::vector<uint8_t>& data);
    
    /**
     * @brief 解压数据
     * @param data 要解压的数据
     * @return 解压后的数据
     */
    std::vector<uint8_t> decode(const std::vector<uint8_t>& data);
    
    /**
     * @brief 压缩文件
     * @param inputFilename 输入文件名
     * @param outputFilename 输出文件名
     * @return 压缩后的文件大小
     */
    size_t encodeFile(const std::string& inputFilename, const std::string& outputFilename);
    
    /**
     * @brief 解压文件
     * @param inputFilename 输入文件名
     * @param outputFilename 输出文件名
     * @return 解压后的文件大小
     */
    size_t decodeFile(const std::string& inputFilename, const std::string& outputFilename);

private:
    int threshold;           // 长度大于等于threshold的匹配串才有必要压缩
    int preBufSizeBits;      // 前向缓冲区占用的比特位
    int windowBufSizeBits;   // 滑动窗口占用的比特位
    int preBufSize;          // 前向缓冲区大小
    int windowBufSize;       // 滑动窗口大小
};

} // namespace compression
} // namespace eagls