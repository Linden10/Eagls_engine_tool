#include "core/encryption/lehmer.h"

namespace eagls {
namespace encryption {

LehmerRandomGenerator::LehmerRandomGenerator() : m_seed(0) {
}

void LehmerRandomGenerator::srand(uint32_t seed) {
    m_seed = seed ^ 123459876;
}

uint32_t LehmerRandomGenerator::rand() {
    // 计算 m_seed = (48271 * (m_seed % 44488) - 3399 * (m_seed / 44488))
    int32_t high = m_seed / 44488;
    int32_t low = m_seed % 44488;
    
    m_seed = 48271 * low - 3399 * high;
    
    // 如果结果为负，加上模数
    if (m_seed < 0) {
        m_seed += 2147483647;
    }
    
    // 确保结果在32位范围内
    m_seed &= 0xFFFFFFFF;
    
    // 返回0-255范围的随机数
    return static_cast<uint32_t>(m_seed * 4.656612875245797e-10 * 256) & 0xFFFFFFFF;
}

CRuntimeRandomGenerator::CRuntimeRandomGenerator() : m_seed(0) {
}

void CRuntimeRandomGenerator::srand(uint32_t seed) {
    m_seed = seed;
}

uint32_t CRuntimeRandomGenerator::rand() {
    // 标准C运行时随机数生成算法
    m_seed = (m_seed * 214013 + 2531011) & 0xFFFFFFFF;
    return (m_seed >> 16) & 0x7FFF;
}

} // namespace encryption
} // namespace eagls