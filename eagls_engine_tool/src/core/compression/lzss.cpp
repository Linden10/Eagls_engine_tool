#include "core/compression/lzss.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstring>

namespace eagls {
namespace compression {

LZSS::LZSS(int preBufSizeBits) 
    : threshold(2), 
      preBufSizeBits(preBufSizeBits),
      windowBufSizeBits(16 - preBufSizeBits) {
    
    // 通过占用的比特位计算缓冲区大小 2^(preBufSizeBits) - 1 + 2
    preBufSize = (1 << preBufSizeBits) - 1 + threshold;
    
    // 通过占用的比特位计算滑动窗口大小
    windowBufSize = (1 << windowBufSizeBits) - 1 + threshold;
}

std::vector<uint8_t> LZSS::encode(const std::vector<uint8_t>& data) {
    std::vector<uint8_t> result;
    
    // 如果输入数据为空，直接返回空结果
    if (data.empty()) {
        return result;
    }
    
    std::vector<uint8_t> windowBuf;  // 滑动窗口
    std::vector<uint8_t> preBuf;     // 前向缓冲区
    
    // 初始化前向缓冲区
    size_t dataPos = 0;
    while (preBuf.size() < preBufSize && dataPos < data.size()) {
        preBuf.push_back(data[dataPos++]);
    }
    
    uint8_t signbits = 0;    // 标记字节
    int itemnum = 0;         // 项目计数
    std::vector<uint8_t> restorebuff;  // 临时缓冲区
    
    // 前向缓冲区有数据时继续压缩
    while (!preBuf.empty()) {
        // 在滑动窗口中寻找最长匹配串
        size_t matchIndex = 0;
        size_t matchLength = 0;
        
        // 从最小匹配长度开始查找
        for (size_t i = threshold; i <= preBuf.size(); ++i) {
            // 在滑动窗口中查找匹配
            bool found = false;
            for (size_t j = 0; j + i <= windowBuf.size(); ++j) {
                if (std::equal(preBuf.begin(), preBuf.begin() + i, windowBuf.begin() + j)) {
                    matchIndex = j;
                    matchLength = i;
                    found = true;
                    break;
                }
            }
            
            // 如果没找到更长的匹配，就退出循环
            if (!found) {
                break;
            }
        }
        
        // 如果找到了足够长的匹配串
        if (matchLength >= threshold) {
            // 计算编码值
            uint16_t code = static_cast<uint16_t>((matchIndex << preBufSizeBits) | (matchLength - threshold));
            
            // 添加到临时缓冲区
            restorebuff.push_back(code & 0xFF);
            restorebuff.push_back((code >> 8) & 0xFF);
            
            // 不设置标志位，表示这是压缩数据
        } else {
            // 没有找到匹配，直接输出原始数据
            matchLength = 1;
            restorebuff.push_back(preBuf[0]);
            
            // 设置标志位，表示这是原始数据
            signbits |= (1 << (7 - itemnum));
        }
        
        // 项目计数增加
        itemnum++;
        
        // 如果已经处理了8个项目，写入一组数据
        if (itemnum >= 8) {
            result.push_back(signbits);
            result.insert(result.end(), restorebuff.begin(), restorebuff.end());
            
            // 重置计数和缓冲区
            itemnum = 0;
            signbits = 0;
            restorebuff.clear();
        }
        
        // 更新滑动窗口和前向缓冲区
        // 将匹配的数据移入滑动窗口
        windowBuf.insert(windowBuf.end(), preBuf.begin(), preBuf.begin() + matchLength);
        
        // 如果滑动窗口超出大小限制，移除前面的数据
        if (windowBuf.size() > windowBufSize) {
            windowBuf.erase(windowBuf.begin(), windowBuf.begin() + (windowBuf.size() - windowBufSize));
        }
        
        // 从前向缓冲区移除已处理的数据
        preBuf.erase(preBuf.begin(), preBuf.begin() + matchLength);
        
        // 从输入数据中读取新数据填充前向缓冲区
        while (preBuf.size() < preBufSize && dataPos < data.size()) {
            preBuf.push_back(data[dataPos++]);
        }
    }
    
    // 处理最后不足8个项目的数据
    if (itemnum > 0) {
        result.push_back(signbits);
        result.insert(result.end(), restorebuff.begin(), restorebuff.end());
    }
    
    return result;
}

std::vector<uint8_t> LZSS::decode(const std::vector<uint8_t>& data) {
    std::vector<uint8_t> result;
    
    // 如果输入数据为空，直接返回空结果
    if (data.empty()) {
        return result;
    }
    
    std::vector<uint8_t> windowBuf;  // 滑动窗口
    
    size_t dataPos = 0;
    
    // 解压数据
    while (dataPos < data.size()) {
        // 读取标记字节
        uint8_t signbits = data[dataPos++];
        
        // 处理8个项目或直到数据结束
        for (int i = 0; i < 8 && dataPos < data.size(); ++i) {
            // 检查标记位
            if (signbits & (1 << (7 - i))) {
                // 原始数据
                uint8_t byte = data[dataPos++];
                result.push_back(byte);
                windowBuf.push_back(byte);
            } else {
                // 压缩数据
                if (dataPos + 1 >= data.size()) {
                    break;  // 数据不足
                }
                
                uint8_t lo = data[dataPos++];
                uint8_t hi = data[dataPos++];
                
                // 解析匹配位置和长度
                uint16_t code = (hi << 8) | lo;
                uint16_t offset = code >> preBufSizeBits;
                uint16_t length = (code & ((1 << preBufSizeBits) - 1)) + threshold;
                
                // 检查偏移是否有效
                if (offset >= windowBuf.size()) {
                    // 无效偏移，可能是数据损坏
                    continue;
                }
                
                // 复制匹配的数据
                for (uint16_t j = 0; j < length; ++j) {
                    uint8_t byte = windowBuf[offset + j];
                    result.push_back(byte);
                    windowBuf.push_back(byte);
                }
            }
            
            // 限制滑动窗口大小
            if (windowBuf.size() > windowBufSize) {
                windowBuf.erase(windowBuf.begin(), windowBuf.begin() + (windowBuf.size() - windowBufSize));
            }
        }
    }
    
    return result;
}

size_t LZSS::encodeFile(const std::string& inputFilename, const std::string& outputFilename) {
    // 读取输入文件
    std::ifstream inFile(inputFilename, std::ios::binary);
    if (!inFile) {
        std::cerr << "Error: Cannot open input file: " << inputFilename << std::endl;
        return 0;
    }
    
    // 读取文件内容
    std::vector<uint8_t> inputData((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
    inFile.close();
    
    // 压缩数据
    std::vector<uint8_t> outputData = encode(inputData);
    
    // 写入输出文件
    std::ofstream outFile(outputFilename, std::ios::binary);
    if (!outFile) {
        std::cerr << "Error: Cannot open output file: " << outputFilename << std::endl;
        return 0;
    }
    
    outFile.write(reinterpret_cast<const char*>(outputData.data()), outputData.size());
    outFile.close();
    
    return outputData.size();
}

size_t LZSS::decodeFile(const std::string& inputFilename, const std::string& outputFilename) {
    // 读取输入文件
    std::ifstream inFile(inputFilename, std::ios::binary);
    if (!inFile) {
        std::cerr << "Error: Cannot open input file: " << inputFilename << std::endl;
        return 0;
    }
    
    // 读取文件内容
    std::vector<uint8_t> inputData((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
    inFile.close();
    
    // 解压数据
    std::vector<uint8_t> outputData = decode(inputData);
    
    // 写入输出文件
    std::ofstream outFile(outputFilename, std::ios::binary);
    if (!outFile) {
        std::cerr << "Error: Cannot open output file: " << outputFilename << std::endl;
        return 0;
    }
    
    outFile.write(reinterpret_cast<const char*>(outputData.data()), outputData.size());
    outFile.close();
    
    return outputData.size();
}

} // namespace compression
} // namespace eagls