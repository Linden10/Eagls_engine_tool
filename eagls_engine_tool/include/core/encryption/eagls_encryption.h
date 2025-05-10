#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include "lehmer.h"

// DLL导出宏定义
#ifdef _WIN32
    #ifdef EAGLS_ENCRYPTION_EXPORTS
        #define EAGLS_ENCRYPTION_API __declspec(dllexport)
    #else
        #define EAGLS_ENCRYPTION_API __declspec(dllimport)
    #endif
#else
    #define EAGLS_ENCRYPTION_API
#endif

namespace eagls {
namespace encryption {

/**
 * @brief EAGLS加密实现
 *
 * 基于Python脚本中的EaglsEncryption重写的C++版本
 */
class EAGLS_ENCRYPTION_API EaglsEncryption {
public:
    /**
     * @brief 构造函数
     */
    EaglsEncryption();

    /**
     * @brief 加密数据
     * @param data 要加密的数据
     * @return 加密后的数据
     */
    std::vector<uint8_t> encrypt(const std::vector<uint8_t>& data);

    /**
     * @brief 解密数据
     * @param data 要解密的数据
     * @return 解密后的数据
     */
    std::vector<uint8_t> decrypt(const std::vector<uint8_t>& data);

    /**
     * @brief 加密文件
     * @param inputFilename 输入文件名
     * @param outputFilename 输出文件名
     * @return 是否成功
     */
    bool encryptFile(const std::string& inputFilename, const std::string& outputFilename);

    /**
     * @brief 解密文件
     * @param inputFilename 输入文件名
     * @param outputFilename 输出文件名
     * @return 是否成功
     */
    bool decryptFile(const std::string& inputFilename, const std::string& outputFilename);

private:
    CRuntimeRandomGenerator m_rng;  // 随机数生成器

#ifdef _MSC_VER
    #pragma warning(push)
    #pragma warning(disable: 4251)  // 禁用C4251警告
#endif
    std::string m_key;              // 加密密钥
#ifdef _MSC_VER
    #pragma warning(pop)
#endif
};

/**
 * @brief Lehmer加密实现
 *
 * 基于Python脚本中的Lehmer_encode函数重写的C++版本
 */
class EAGLS_ENCRYPTION_API LehmerEncryption {
public:
    /**
     * @brief 构造函数
     */
    LehmerEncryption();

    /**
     * @brief 加密数据
     * @param data 要加密的数据
     * @return 加密后的数据
     */
    std::vector<uint8_t> encrypt(const std::vector<uint8_t>& data);

    /**
     * @brief 解密数据
     * @param data 要解密的数据
     * @return 解密后的数据
     */
    std::vector<uint8_t> decrypt(const std::vector<uint8_t>& data);

private:
    LehmerRandomGenerator m_rng;  // 随机数生成器

#ifdef _MSC_VER
    #pragma warning(push)
    #pragma warning(disable: 4251)  // 禁用C4251警告
#endif
    std::string m_key;            // 加密密钥
#ifdef _MSC_VER
    #pragma warning(pop)
#endif
};

} // namespace encryption
} // namespace eagls