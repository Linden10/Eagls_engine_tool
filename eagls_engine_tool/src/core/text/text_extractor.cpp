#include "core/text/text_extractor.h"
#include "core/file/file_utils.h"
#include <fstream>
#include <iostream>
#include <regex>

namespace eagls {
namespace text {

TextExtractor::TextExtractor() : m_encoding("GBK") {
}

TextExtractor::~TextExtractor() {
}

bool TextExtractor::extractText(const std::string& filename, const std::string& outputFilename) {
    // 读取文件
    std::vector<uint8_t> data = file::FileUtils::readFile(filename);
    if (data.empty()) {
        std::cerr << "Error: Failed to read file: " << filename << std::endl;
        return false;
    }
    
    // 打开输出文件
    std::ofstream outFile(outputFilename);
    if (!outFile) {
        std::cerr << "Error: Failed to open output file: " << outputFilename << std::endl;
        return false;
    }
    
    // 提取文本
    std::string text(reinterpret_cast<const char*>(data.data()), data.size());
    
    // 提取非ASCII文本
    std::string result;
    size_t i = 0;
    while (i < text.size()) {
        // 查找引号包围的文本
        if (text[i] == '"') {
            size_t start = i + 1;
            size_t end = text.find('"', start);
            if (end != std::string::npos) {
                std::string str = text.substr(start, end - start);
                if (!str.empty() && !isPureAscii(str)) {
                    result += str + "\n";
                }
                i = end + 1;
                continue;
            }
        }
        
        // 查找注释
        if (text[i] == '#') {
            size_t start = i + 1;
            size_t end = text.find('\n', start);
            if (end != std::string::npos) {
                std::string str = text.substr(start, end - start);
                if (!str.empty() && !isPureAscii(str)) {
                    result += str + "\n";
                }
                i = end + 1;
                continue;
            }
        }
        
        i++;
    }
    
    // 写入输出文件
    outFile << result;
    outFile.close();
    
    return true;
}

int TextExtractor::batchExtractText(const std::string& inputDir, const std::string& outputDir) {
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
        std::string filename = file::FileUtils::getFileName(file) + ".txt";
        std::string outputFilename = file::FileUtils::combinePath(outputDir, filename);
        
        if (extractText(file, outputFilename)) {
            count++;
        }
    }
    
    return count;
}

void TextExtractor::setEncoding(const std::string& encoding) {
    m_encoding = encoding;
}

std::string TextExtractor::getEncoding() const {
    return m_encoding;
}

bool TextExtractor::isPureAscii(const std::string& str) {
    std::regex pattern(R"(^[a-zA-Z0-9_\.%@(),:=\\]+$)");
    return std::regex_match(str, pattern);
}

} // namespace text
} // namespace eagls