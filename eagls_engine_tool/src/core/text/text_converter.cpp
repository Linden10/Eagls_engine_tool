#include "core/text/text_converter.h"
#include "core/file/file_utils.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <regex>
#include <codecvt>
#include <locale>

namespace fs = std::filesystem;

namespace eagls {
namespace text {

TextConverter::TextConverter() {
}

TextConverter::~TextConverter() {
}

bool TextConverter::binaryToText(const std::string& inputFilename, const std::string& outputFilename, 
                                const std::string& encoding, bool removeNulls) {
    // 读取二进制文件
    std::vector<uint8_t> data = file::FileUtils::readFile(inputFilename);
    if (data.empty()) {
        std::cerr << "Error: Failed to read binary file: " << inputFilename << std::endl;
        return false;
    }
    
    // 将二进制数据转换为文本
    std::string text;
    
    // 根据编码选择转换方法
    if (encoding == "GBK" || encoding == "gbk") {
        // GBK编码
        // 注意：这里使用简单的方法，实际应该使用专业的编码转换库
        text = std::string(reinterpret_cast<const char*>(data.data()), data.size());
        
        // 移除无法解码的字符
        for (size_t i = 0; i < text.size(); ++i) {
            if (static_cast<unsigned char>(text[i]) > 127) {
                // 检查是否是GBK的第一个字节
                if (i + 1 < text.size()) {
                    // GBK编码的第二个字节
                    ++i;
                } else {
                    // 不完整的GBK字符，移除
                    text[i] = '?';
                }
            }
        }
    } else if (encoding == "UTF-8" || encoding == "utf-8" || encoding == "utf8") {
        // UTF-8编码
        text = std::string(reinterpret_cast<const char*>(data.data()), data.size());
    } else if (encoding == "UTF-16" || encoding == "utf-16" || encoding == "utf16") {
        // UTF-16编码
        std::wstring_convert<std::codecvt_utf16<wchar_t>> converter;
        std::wstring wtext = converter.from_bytes(
            reinterpret_cast<const char*>(data.data()), 
            reinterpret_cast<const char*>(data.data() + data.size())
        );
        text = std::string(wtext.begin(), wtext.end());
    } else {
        // 默认使用ASCII编码
        text = std::string(reinterpret_cast<const char*>(data.data()), data.size());
    }
    
    // 移除空字符
    if (removeNulls) {
        text.erase(std::remove(text.begin(), text.end(), '\0'), text.end());
    }
    
    // 写入文本文件
    std::ofstream outFile(outputFilename);
    if (!outFile) {
        std::cerr << "Error: Failed to open output file: " << outputFilename << std::endl;
        return false;
    }
    
    outFile << text;
    outFile.close();
    
    return true;
}

bool TextConverter::textToBinary(const std::string& inputFilename, const std::string& outputFilename, 
                                const std::string& encoding) {
    // 读取文本文件
    std::ifstream inFile(inputFilename);
    if (!inFile) {
        std::cerr << "Error: Failed to open text file: " << inputFilename << std::endl;
        return false;
    }
    
    std::string text((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
    inFile.close();
    
    // 将文本转换为二进制数据
    std::vector<uint8_t> data;
    
    // 根据编码选择转换方法
    if (encoding == "GBK" || encoding == "gbk") {
        // GBK编码
        // 注意：这里使用简单的方法，实际应该使用专业的编码转换库
        data.assign(text.begin(), text.end());
    } else if (encoding == "UTF-8" || encoding == "utf-8" || encoding == "utf8") {
        // UTF-8编码
        data.assign(text.begin(), text.end());
    } else if (encoding == "UTF-16" || encoding == "utf-16" || encoding == "utf16") {
        // UTF-16编码
        std::wstring wtext(text.begin(), text.end());
        std::wstring_convert<std::codecvt_utf16<wchar_t>> converter;
        std::string bytes = converter.to_bytes(wtext);
        data.assign(bytes.begin(), bytes.end());
    } else {
        // 默认使用ASCII编码
        data.assign(text.begin(), text.end());
    }
    
    // 写入二进制文件
    return file::FileUtils::writeFile(outputFilename, data);
}

int TextConverter::batchBinaryToText(const std::string& inputDir, const std::string& outputDir, 
                                    const std::string& filePattern, const std::string& encoding, 
                                    bool removeNulls) {
    // 确保输出目录存在
    if (!file::FileUtils::createDirectory(outputDir)) {
        std::cerr << "Error: Failed to create output directory: " << outputDir << std::endl;
        return 0;
    }
    
    int count = 0;
    
    try {
        // 遍历输入目录
        for (const auto& entry : fs::directory_iterator(inputDir)) {
            if (!entry.is_regular_file()) {
                continue;
            }
            
            std::string filename = entry.path().filename().string();
            
            // 检查文件是否匹配模式
            if (filePattern != "*" && filePattern != "*.*") {
                // 简单的通配符匹配
                std::string pattern = filePattern;
                std::replace(pattern.begin(), pattern.end(), '*', '.');
                std::replace(pattern.begin(), pattern.end(), '?', '.');
                std::regex regex(pattern);
                
                if (!std::regex_match(filename, regex)) {
                    continue;
                }
            }
            
            // 构造输出文件名
            std::string outputFilename = file::FileUtils::combinePath(outputDir, filename);
            
            // 转换文件
            if (binaryToText(entry.path().string(), outputFilename, encoding, removeNulls)) {
                count++;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    
    return count;
}

int TextConverter::batchTextToBinary(const std::string& inputDir, const std::string& outputDir, 
                                    const std::string& filePattern, const std::string& encoding) {
    // 确保输出目录存在
    if (!file::FileUtils::createDirectory(outputDir)) {
        std::cerr << "Error: Failed to create output directory: " << outputDir << std::endl;
        return 0;
    }
    
    int count = 0;
    
    try {
        // 遍历输入目录
        for (const auto& entry : fs::directory_iterator(inputDir)) {
            if (!entry.is_regular_file()) {
                continue;
            }
            
            std::string filename = entry.path().filename().string();
            
            // 检查文件是否匹配模式
            if (filePattern != "*" && filePattern != "*.*") {
                // 简单的通配符匹配
                std::string pattern = filePattern;
                std::replace(pattern.begin(), pattern.end(), '*', '.');
                std::replace(pattern.begin(), pattern.end(), '?', '.');
                std::regex regex(pattern);
                
                if (!std::regex_match(filename, regex)) {
                    continue;
                }
            }
            
            // 构造输出文件名
            std::string outputFilename = file::FileUtils::combinePath(outputDir, filename);
            
            // 转换文件
            if (textToBinary(entry.path().string(), outputFilename, encoding)) {
                count++;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    
    return count;
}

} // namespace text
} // namespace eagls