#include "core/text/encoding_converter.h"
#include "core/file/file_utils.h"
#include <fstream>
#include <iostream>
#include <codecvt>
#include <locale>

namespace eagls {
namespace text {

EncodingConverter::EncodingConverter() {
}

EncodingConverter::~EncodingConverter() {
}

std::string EncodingConverter::convert(const std::string& input, const std::string& fromEncoding, const std::string& toEncoding) {
    // 这里只是一个简单的实现
    
    // 如果源编码和目标编码相同，直接返回
    if (fromEncoding == toEncoding) {
        return input;
    }
    
    // 这里只实现了UTF-8和UTF-16之间的转换
    if (fromEncoding == "UTF-8" && toEncoding == "UTF-16") {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        std::wstring wstr = converter.from_bytes(input);
        
        // 将wstring转换为string
        std::string result(reinterpret_cast<const char*>(wstr.data()), wstr.size() * sizeof(wchar_t));
        return result;
    } else if (fromEncoding == "UTF-16" && toEncoding == "UTF-8") {
        std::wstring wstr(reinterpret_cast<const wchar_t*>(input.data()), input.size() / sizeof(wchar_t));
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        return converter.to_bytes(wstr);
    }
    
    // 其他编码转换暂不支持
    std::cerr << "Warning: Encoding conversion from " << fromEncoding << " to " << toEncoding << " is not supported" << std::endl;
    return input;
}

bool EncodingConverter::convertFile(const std::string& inputFilename, const std::string& outputFilename, const std::string& fromEncoding, const std::string& toEncoding) {
    // 读取输入文件
    std::vector<uint8_t> data = file::FileUtils::readFile(inputFilename);
    if (data.empty()) {
        std::cerr << "Error: Failed to read file: " << inputFilename << std::endl;
        return false;
    }
    
    // 转换编码
    std::string input(reinterpret_cast<const char*>(data.data()), data.size());
    std::string output = convert(input, fromEncoding, toEncoding);
    
    // 写入输出文件
    std::vector<uint8_t> outputData(output.begin(), output.end());
    if (!file::FileUtils::writeFile(outputFilename, outputData)) {
        std::cerr << "Error: Failed to write output file: " << outputFilename << std::endl;
        return false;
    }
    
    return true;
}

int EncodingConverter::batchConvertFiles(const std::string& inputDir, const std::string& outputDir, const std::string& fromEncoding, const std::string& toEncoding) {
    // 确保输出目录存在
    if (!file::FileUtils::createDirectory(outputDir)) {
        std::cerr << "Error: Failed to create output directory: " << outputDir << std::endl;
        return 0;
    }
    
    int count = 0;
    
    // 获取输入目录中的所有文件
    std::vector<std::string> files = file::FileUtils::getFileList(inputDir);
    
    // 处理每个文件
    for (const auto& file : files) {
        std::string outputFilename = file::FileUtils::combinePath(outputDir, file::FileUtils::getFileName(file) + file::FileUtils::getFileExtension(file));
        
        if (convertFile(file, outputFilename, fromEncoding, toEncoding)) {
            count++;
        }
    }
    
    return count;
}

std::string EncodingConverter::detectFileEncoding(const std::string& filename) {
    // 读取文件
    std::vector<uint8_t> data = file::FileUtils::readFile(filename);
    if (data.empty()) {
        std::cerr << "Error: Failed to read file: " << filename << std::endl;
        return "";
    }
    
    return detectStringEncoding(std::string(reinterpret_cast<const char*>(data.data()), data.size()));
}

std::string EncodingConverter::detectStringEncoding(const std::string& input) {
    // 这里只是一个简单的实现
    
    // 检查BOM
    if (input.size() >= 3 && input[0] == '\xEF' && input[1] == '\xBB' && input[2] == '\xBF') {
        return "UTF-8";
    }
    
    if (input.size() >= 2) {
        if (input[0] == '\xFF' && input[1] == '\xFE') {
            return "UTF-16LE";
        }
        if (input[0] == '\xFE' && input[1] == '\xFF') {
            return "UTF-16BE";
        }
    }
    
    // 检查是否为UTF-8
    bool isUtf8 = true;
    for (size_t i = 0; i < input.size(); ++i) {
        unsigned char c = input[i];
        if (c >= 0x80) {
            // 检查UTF-8多字节序列
            if ((c & 0xE0) == 0xC0) {
                // 2字节序列
                if (i + 1 >= input.size() || (input[i + 1] & 0xC0) != 0x80) {
                    isUtf8 = false;
                    break;
                }
                i += 1;
            } else if ((c & 0xF0) == 0xE0) {
                // 3字节序列
                if (i + 2 >= input.size() || (input[i + 1] & 0xC0) != 0x80 || (input[i + 2] & 0xC0) != 0x80) {
                    isUtf8 = false;
                    break;
                }
                i += 2;
            } else if ((c & 0xF8) == 0xF0) {
                // 4字节序列
                if (i + 3 >= input.size() || (input[i + 1] & 0xC0) != 0x80 || (input[i + 2] & 0xC0) != 0x80 || (input[i + 3] & 0xC0) != 0x80) {
                    isUtf8 = false;
                    break;
                }
                i += 3;
            } else {
                isUtf8 = false;
                break;
            }
        }
    }
    
    if (isUtf8) {
        return "UTF-8";
    }
    
    // 检查是否为GBK
    bool isGbk = true;
    for (size_t i = 0; i < input.size(); ++i) {
        unsigned char c = input[i];
        if (c >= 0x80) {
            // GBK编码的第一个字节范围是0x81-0xFE
            if (c >= 0x81 && c <= 0xFE) {
                // GBK编码的第二个字节范围是0x40-0xFE（除了0x7F）
                if (i + 1 >= input.size() || input[i + 1] < 0x40 || input[i + 1] == 0x7F || input[i + 1] > 0xFE) {
                    isGbk = false;
                    break;
                }
                i += 1;
            } else {
                isGbk = false;
                break;
            }
        }
    }
    
    if (isGbk) {
        return "GBK";
    }
    
    // 默认返回ASCII
    return "ASCII";
}

} // namespace text
} // namespace eagls