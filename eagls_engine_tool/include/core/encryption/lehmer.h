#pragma once

#include <cstdint>

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
 * @brief Lehmer随机数生成器
 * 
 * 基于Python脚本中的LehmerRandomGenerator重写的C++版本
 */
class EAGLS_ENCRYPTION_API LehmerRandomGenerator {
public:
    /**
     * @brief 构造函数
     */
    LehmerRandomGenerator();
    
    /**
     * @brief 设置随机数种子
     * @param seed 随机数种子
     */
    void srand(uint32_t seed);
    
    /**
     * @brief 生成随机数
     * @return 生成的随机数
     */
    uint32_t rand();

private:
    uint32_t m_seed;  // 随机数种子
};

/**
 * @brief C运行时随机数生成器
 * 
 * 基于Python脚本中的CRuntimeRandomGenerator重写的C++版本
 */
class EAGLS_ENCRYPTION_API CRuntimeRandomGenerator {
public:
    /**
     * @brief 构造函数
     */
    CRuntimeRandomGenerator();
    
    /**
     * @brief 设置随机数种子
     * @param seed 随机数种子
     */
    void srand(uint32_t seed);
    
    /**
     * @brief 生成随机数
     * @return 生成的随机数
     */
    uint32_t rand();

private:
    uint32_t m_seed;  // 随机数种子
};

} // namespace encryption
} // namespace eagls