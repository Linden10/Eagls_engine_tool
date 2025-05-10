#include "core/text/text_replacer.h"
#include "core/file/file_utils.h"
#include <fstream>
#include <iostream>
#include <sstream>

namespace eagls {
namespace text {

TextReplacer::TextReplacer() : m_encoding("GBK") {
}

TextReplacer::~TextReplacer() {
}

bool TextReplacer::replaceText(const std::string& filename, const std::string& textFilename, const std::string& outputFilename) {
    // 读取原始文件
    std::vector<uint8_t> data = file::FileUtils::readFile(filename);
    if (data.empty()) {
        std::cerr << "Error: Failed to read file: " << filename << std::endl;
        return false;
    }
    
    // 读取文本文件
    std::ifstream textFile(textFilename);
    if (!textFile) {
        std::cerr << "Error: Failed to open text file: " << textFilename << std::endl;
        return false;
    }
    
    // 读取替换文本
    std::map<std::string, std::string> replacements;
    std::string line;
    std::string original;
    int lineCount = 0;
    
    while (std::getline(textFile, line)) {
        lineCount++;
        
        if (lineCount % 3 == 1) {
            // 原始文本
            original = line;
        } else if (lineCount % 3 == 2) {
            // 替换文本
            replacements[original] = line;
        }
    }
    
    textFile.close();
    
    // 替换文本
    std::string text(reinterpret_cast<const char*>(data.data()), data.size());
    
    for (const auto& replacement : replacements) {
        // 查找并替换所有匹配项
        size_t pos = 0;
        while ((pos = text.find(replacement.first, pos)) != std::string::npos) {
            text.replace(pos, replacement.first.length(), replacement.second);
            pos += replacement.second.length();
        }
    }
    
    // 写入输出文件
    std::vector<uint8_t> outputData(text.begin(), text.end());
    if (!file::FileUtils::writeFile(outputFilename, outputData)) {
        std::cerr << "Error: Failed to write output file: " << outputFilename << std::endl;
        return false;
    }
    
    return true;
}

int TextReplacer::batchReplaceText(const std::string& inputDir, const std::string& textDir, const std::string& outputDir) {
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
        std::string filename = file::FileUtils::getFileName(file);
        std::string textFilename = file::FileUtils::combinePath(textDir, filename + ".txt");
        std::string outputFilename = file::FileUtils::combinePath(outputDir, file::FileUtils::getFileName(file) + file::FileUtils::getFileExtension(file));
        
        // 检查文本文件是否存在
        if (!file::FileUtils::fileExists(textFilename)) {
            std::cerr << "Warning: Text file not found: " << textFilename << std::endl;
            continue;
        }
        
        if (replaceText(file, textFilename, outputFilename)) {
            count++;
        }
    }
    
    return count;
}

void TextReplacer::setEncoding(const std::string& encoding) {
    m_encoding = encoding;
}

std::string TextReplacer::getEncoding() const {
    return m_encoding;
}

} // namespace text
} // namespace eagls