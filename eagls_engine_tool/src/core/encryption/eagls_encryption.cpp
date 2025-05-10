#include "core/encryption/eagls_encryption.h"
#include <fstream>
#include <iostream>

namespace eagls {
namespace encryption {

EaglsEncryption::EaglsEncryption() : m_key("EAGLS_SYSTEM") {
}

std::vector<uint8_t> EaglsEncryption::encrypt(const std::vector<uint8_t>& data) {
    // 加密和解密使用相同的算法，只是应用的位置不同
    return decrypt(data);
}

std::vector<uint8_t> EaglsEncryption::decrypt(const std::vector<uint8_t>& data) {
    // 如果数据太小，无法解密
    if (data.size() <= 3602) {
        return data;
    }
    
    // 复制数据，因为我们需要修改它
    std::vector<uint8_t> result = data;
    
    // 文本偏移量和长度
    const size_t text_offset = 3600;
    const size_t text_length = data.size() - text_offset - 2;
    
    // 设置随机数种子
    m_rng.srand(data.back());
    
    // 解密文本部分
    for (size_t i = 0; i < text_length; i += 2) {
        result[text_offset + i] ^= m_key[m_rng.rand() % m_key.size()];
    }
    
    return result;
}

bool EaglsEncryption::encryptFile(const std::string& inputFilename, const std::string& outputFilename) {
    // 读取输入文件
    std::ifstream inFile(inputFilename, std::ios::binary);
    if (!inFile) {
        std::cerr << "Error: Cannot open input file: " << inputFilename << std::endl;
        return false;
    }
    
    // 读取文件内容
    std::vector<uint8_t> inputData((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
    inFile.close();
    
    // 加密数据
    std::vector<uint8_t> outputData = encrypt(inputData);
    
    // 写入输出文件
    std::ofstream outFile(outputFilename, std::ios::binary);
    if (!outFile) {
        std::cerr << "Error: Cannot open output file: " << outputFilename << std::endl;
        return false;
    }
    
    outFile.write(reinterpret_cast<const char*>(outputData.data()), outputData.size());
    outFile.close();
    
    return true;
}

bool EaglsEncryption::decryptFile(const std::string& inputFilename, const std::string& outputFilename) {
    // 读取输入文件
    std::ifstream inFile(inputFilename, std::ios::binary);
    if (!inFile) {
        std::cerr << "Error: Cannot open input file: " << inputFilename << std::endl;
        return false;
    }
    
    // 读取文件内容
    std::vector<uint8_t> inputData((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
    inFile.close();
    
    // 解密数据
    std::vector<uint8_t> outputData = decrypt(inputData);
    
    // 写入输出文件
    std::ofstream outFile(outputFilename, std::ios::binary);
    if (!outFile) {
        std::cerr << "Error: Cannot open output file: " << outputFilename << std::endl;
        return false;
    }
    
    outFile.write(reinterpret_cast<const char*>(outputData.data()), outputData.size());
    outFile.close();
    
    return true;
}

LehmerEncryption::LehmerEncryption() : m_key("EAGLS_SYSTEM") {
}

std::vector<uint8_t> LehmerEncryption::encrypt(const std::vector<uint8_t>& data) {
    // 如果数据为空，直接返回
    if (data.empty()) {
        return data;
    }
    
    // 复制数据，因为我们需要修改它
    std::vector<uint8_t> result = data;
    
    // 设置随机数种子
    m_rng.srand(data.back());
    
    // 加密限制
    const size_t limit = std::min(data.size() - 1, static_cast<size_t>(0x174b));
    
    // 加密数据
    for (size_t i = 0; i < limit; ++i) {
        result[i] ^= m_key[m_rng.rand() % m_key.size()];
    }
    
    return result;
}

std::vector<uint8_t> LehmerEncryption::decrypt(const std::vector<uint8_t>& data) {
    // 加密和解密使用相同的算法
    return encrypt(data);
}

} // namespace encryption
} // namespace eagls