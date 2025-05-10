#include "core/file/dat_file.h"
#include "core/file/file_utils.h"
#include "core/encryption/eagls_encryption.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstring>
#include <regex>
#include <sstream>
#include <iomanip>

namespace eagls {
namespace file {

// DAT文件格式常量
constexpr size_t SECTION_TABLE_OFFSET = 0;    // 段表偏移
constexpr size_t SECTION_TABLE_SIZE = 0xE10;  // 段表大小
constexpr size_t TEXT_OFFSET = 0xE10;         // 文本偏移
constexpr size_t SECTION_NAME_SIZE = 0x20;    // 段名大小
constexpr size_t SECTION_ENTRY_SIZE = 0x24;   // 段条目大小

DatFile::DatFile() : m_isOpen(false) {
}

DatFile::~DatFile() {
    close();
}

bool DatFile::open(const std::string& filename, bool decrypt) {
    // 关闭已打开的文件
    close();
    
    // 读取文件数据
    m_data = FileUtils::readFile(filename);
    if (m_data.empty()) {
        std::cerr << "Error: Failed to read DAT file: " << filename << std::endl;
        return false;
    }
    
    // 如果需要解密
    if (decrypt) {
        encryption::EaglsEncryption enc;
        m_data = enc.decrypt(m_data);
    }
    
    // 解析段表
    if (!parseSectionTable()) {
        std::cerr << "Error: Failed to parse section table" << std::endl;
        return false;
    }
    
    m_isOpen = true;
    return true;
}

void DatFile::close() {
    m_data.clear();
    m_sections.clear();
    m_isOpen = false;
}

std::vector<std::string> DatFile::getSectionList() const {
    std::vector<std::string> sections;
    
    for (const auto& section : m_sections) {
        sections.push_back(section.first);
    }
    
    return sections;
}

std::vector<uint8_t> DatFile::getSectionData(const std::string& sectionName) const {
    if (!m_isOpen) {
        std::cerr << "Error: DAT file is not open" << std::endl;
        return {};
    }
    
    // 查找段
    auto it = m_sections.find(sectionName);
    if (it == m_sections.end()) {
        std::cerr << "Error: Section not found: " << sectionName << std::endl;
        return {};
    }
    
    const DatEntry& entry = it->second;
    
    // 检查段范围
    if (entry.offset + entry.size > m_data.size()) {
        std::cerr << "Error: Section data out of range: " << sectionName << std::endl;
        return {};
    }
    
    // 提取段数据
    return std::vector<uint8_t>(m_data.begin() + entry.offset, m_data.begin() + entry.offset + entry.size);
}

bool DatFile::extractAllSections(const std::string& outputPath) const {
    if (!m_isOpen) {
        std::cerr << "Error: DAT file is not open" << std::endl;
        return false;
    }
    
    // 确保输出目录存在
    if (!FileUtils::createDirectory(outputPath)) {
        std::cerr << "Error: Failed to create output directory: " << outputPath << std::endl;
        return false;
    }
    
    bool success = true;
    
    // 提取所有段
    for (const auto& section : m_sections) {
        std::string outputFilename = FileUtils::combinePath(outputPath, section.first + ".bin");
        
        // 提取段数据
        std::vector<uint8_t> data = getSectionData(section.first);
        if (data.empty()) {
            std::cerr << "Error: Failed to extract section: " << section.first << std::endl;
            success = false;
            continue;
        }
        
        // 写入文件
        if (!FileUtils::writeFile(outputFilename, data)) {
            std::cerr << "Error: Failed to write section file: " << outputFilename << std::endl;
            success = false;
        }
    }
    
    return success;
}

bool DatFile::extractText(const std::string& outputFilename) const {
    if (!m_isOpen) {
        std::cerr << "Error: DAT file is not open" << std::endl;
        return false;
    }
    
    std::ofstream outFile(outputFilename, std::ios::binary);
    if (!outFile) {
        std::cerr << "Error: Cannot open output file: " << outputFilename << std::endl;
        return false;
    }
    
    // 文本提取正则表达式
    std::regex asciiPattern(R"(^[a-zA-Z0-9_\.%@(),:=\\]+$)");
    
    // 遍历所有段
    for (const auto& section : m_sections) {
        std::vector<uint8_t> data = getSectionData(section.first);
        if (data.empty()) {
            continue;
        }
        
        size_t i = 0;
        while (i < data.size()) {
            int j = -1;
            
            // 查找引号包围的文本
            if (data[i] == '"') {
                j = i + 1;
                while (j < data.size() && data[j] != '"') {
                    j++;
                }
            }
            // 查找注释
            else if (data[i] == '#') {
                j = i + 1;
                while (j + 2 < data.size() && !(data[j] == '\n' || (data[j] == '\r' && data[j + 1] == '\n'))) {
                    j++;
                }
            }
            
            // 如果找到文本
            if (j != -1 && j - i <= 1000) {
                if (j < data.size()) {
                    // 提取文本
                    std::string text(reinterpret_cast<const char*>(&data[i + 1]), j - (i + 1));
                    
                    // 检查文本是否为纯ASCII
                    if (!text.empty() && !std::regex_match(text, asciiPattern)) {
                        // 使用Base64编码
                        std::stringstream ss;
                        for (size_t k = 0; k < text.size(); ++k) {
                            ss << std::setw(2) << std::setfill('0') << std::hex << (unsigned int)(unsigned char)text[k];
                        }
                        std::string encoded = ss.str();
                        
                        // 写入文本
                        outFile << encoded << std::endl;
                        outFile << encoded << std::endl;
                        outFile << std::endl;
                    }
                }
                i = j + 1;
            } else {
                i++;
            }
        }
    }
    
    outFile.close();
    return true;
}

bool DatFile::replaceText(const std::string& textFilename, const std::string& outputFilename) {
    if (!m_isOpen) {
        std::cerr << "Error: DAT file is not open" << std::endl;
        return false;
    }
    
    // 读取文本文件
    std::ifstream textFile(textFilename);
    if (!textFile) {
        std::cerr << "Error: Cannot open text file: " << textFilename << std::endl;
        return false;
    }
    
    // 读取所有文本行
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(textFile, line)) {
        lines.push_back(line);
    }
    textFile.close();
    
    // 检查文本行数是否为3的倍数
    if (lines.size() % 3 != 0) {
        std::cerr << "Error: Invalid text file format, line count must be a multiple of 3" << std::endl;
        return false;
    }
    
    // 创建文本替换映射
    std::map<std::string, std::string> replacements;
    for (size_t i = 0; i < lines.size(); i += 3) {
        if (i + 1 < lines.size()) {
            // 原始文本和替换文本
            std::string original = lines[i];
            std::string replacement = lines[i + 1];
            
            // 解码十六进制文本
            std::string decodedOriginal;
            for (size_t j = 0; j < original.length(); j += 2) {
                std::string byteString = original.substr(j, 2);
                char byte = (char)std::stoi(byteString, nullptr, 16);
                decodedOriginal.push_back(byte);
            }
            
            std::string decodedReplacement;
            for (size_t j = 0; j < replacement.length(); j += 2) {
                std::string byteString = replacement.substr(j, 2);
                char byte = (char)std::stoi(byteString, nullptr, 16);
                decodedReplacement.push_back(byte);
            }
            
            // 添加到替换映射
            replacements[decodedOriginal] = decodedReplacement;
        }
    }
    
    // 复制原始数据
    std::vector<uint8_t> newData = m_data;
    
    // 遍历所有段
    for (const auto& section : m_sections) {
        const DatEntry& entry = section.second;
        
        size_t i = entry.offset;
        while (i < entry.offset + entry.size) {
            int j = -1;
            
            // 查找引号包围的文本
            if (i < newData.size() && newData[i] == '"') {
                j = i + 1;
                while (j < newData.size() && newData[j] != '"') {
                    j++;
                }
            }
            // 查找注释
            else if (i < newData.size() && newData[i] == '#') {
                j = i + 1;
                while (j + 2 < newData.size() && !(newData[j] == '\n' || (newData[j] == '\r' && newData[j + 1] == '\n'))) {
                    j++;
                }
            }
            
            // 如果找到文本
            if (j != -1 && j - i <= 1000) {
                if (j < newData.size()) {
                    // 提取文本
                    std::string text(reinterpret_cast<const char*>(&newData[i + 1]), j - (i + 1));
                    
                    // 检查是否有替换
                    auto it = replacements.find(text);
                    if (it != replacements.end()) {
                        // 替换文本
                        const std::string& replacement = it->second;
                        
                        // 检查替换文本长度
                        if (replacement.length() <= text.length()) {
                            // 直接替换
                            std::copy(replacement.begin(), replacement.end(), newData.begin() + i + 1);
                            
                            // 如果替换文本较短，用空字符填充
                            if (replacement.length() < text.length()) {
                                std::fill(newData.begin() + i + 1 + replacement.length(), 
                                         newData.begin() + i + 1 + text.length(), 
                                         0);
                            }
                        } else {
                            std::cerr << "Warning: Replacement text is longer than original, skipping: " << text << std::endl;
                        }
                    }
                }
                i = j + 1;
            } else {
                i++;
            }
        }
    }
    
    // 更新段表
    m_data = newData;
    updateSectionTable();
    
    // 加密数据
    encryption::EaglsEncryption enc;
    std::vector<uint8_t> encryptedData = enc.encrypt(m_data);
    
    // 写入输出文件
    return FileUtils::writeFile(outputFilename, encryptedData);
}

bool DatFile::create(const std::string& filename, const std::map<std::string, std::vector<uint8_t>>& sections, bool encrypt) {
    // 关闭已打开的文件
    close();
    
    // 检查段数
    if (sections.empty() || sections.size() > 100) {  // 假设最多支持100个段
        std::cerr << "Error: Invalid number of sections: " << sections.size() << std::endl;
        return false;
    }
    
    // 创建段表
    std::vector<uint8_t> sectionTable(SECTION_TABLE_SIZE, 0);
    
    // 计算文本部分的大小
    size_t textSize = 0;
    for (const auto& section : sections) {
        textSize += section.second.size();
    }
    
    // 创建文件数据
    m_data.resize(TEXT_OFFSET + textSize);
    std::fill(m_data.begin(), m_data.end(), 0);
    
    // 写入段数据
    size_t offset = TEXT_OFFSET;
    size_t sectionIndex = 0;
    
    for (const auto& section : sections) {
        // 检查段名长度
        if (section.first.length() >= SECTION_NAME_SIZE) {
            std::cerr << "Error: Section name too long: " << section.first << std::endl;
            return false;
        }
        
        // 写入段表条目
        size_t tableOffset = sectionIndex * SECTION_ENTRY_SIZE;
        
        // 写入段名
        std::copy(section.first.begin(), section.first.end(), sectionTable.begin() + tableOffset);
        
        // 写入段偏移
        uint32_t sectionOffset = static_cast<uint32_t>(offset - TEXT_OFFSET);
        std::memcpy(&sectionTable[tableOffset + SECTION_NAME_SIZE], &sectionOffset, sizeof(uint32_t));
        
        // 写入段数据
        std::copy(section.second.begin(), section.second.end(), m_data.begin() + offset);
        offset += section.second.size();
        
        // 创建段条目
        DatEntry entry;
        entry.name = section.first;
        entry.offset = offset - section.second.size();
        entry.size = static_cast<uint32_t>(section.second.size());
        
        // 添加到段映射
        m_sections[section.first] = entry;
        
        sectionIndex++;
    }
    
    // 复制段表到文件数据
    std::copy(sectionTable.begin(), sectionTable.end(), m_data.begin() + SECTION_TABLE_OFFSET);
    
    // 如果需要加密
    std::vector<uint8_t> outputData = m_data;
    if (encrypt) {
        encryption::EaglsEncryption enc;
        outputData = enc.encrypt(m_data);
    }
    
    // 写入文件
    if (!FileUtils::writeFile(filename, outputData)) {
        std::cerr << "Error: Failed to write DAT file: " << filename << std::endl;
        return false;
    }
    
    m_isOpen = true;
    return true;
}

const std::vector<uint8_t>& DatFile::getRawData() const {
    return m_data;
}

bool DatFile::parseSectionTable() {
    m_sections.clear();
    
    // 检查文件大小
    if (m_data.size() <= SECTION_TABLE_SIZE) {
        std::cerr << "Error: DAT file too small" << std::endl;
        return false;
    }
    
    // 解析段表
    for (size_t i = 0; i < 100; ++i) {  // 假设最多100个段
        size_t offset = SECTION_TABLE_OFFSET + i * SECTION_ENTRY_SIZE;
        
        // 检查是否到达段表末尾
        if (offset + SECTION_ENTRY_SIZE > SECTION_TABLE_SIZE || m_data[offset] == 0) {
            break;
        }
        
        // 读取段名
        char name[SECTION_NAME_SIZE + 1] = {0};
        std::memcpy(name, &m_data[offset], SECTION_NAME_SIZE);
        
        // 读取段偏移
        uint32_t sectionOffset = *reinterpret_cast<uint32_t*>(&m_data[offset + SECTION_NAME_SIZE]);
        
        // 计算段大小（通过查找下一个段的偏移或文件结尾）
        uint32_t sectionSize = 0;
        if (i + 1 < 100) {
            size_t nextOffset = offset + SECTION_ENTRY_SIZE;
            if (nextOffset + SECTION_ENTRY_SIZE <= SECTION_TABLE_SIZE && m_data[nextOffset] != 0) {
                uint32_t nextSectionOffset = *reinterpret_cast<uint32_t*>(&m_data[nextOffset + SECTION_NAME_SIZE]);
                sectionSize = nextSectionOffset - sectionOffset;
            } else {
                sectionSize = static_cast<uint32_t>(m_data.size() - (TEXT_OFFSET + sectionOffset));
            }
        } else {
            sectionSize = static_cast<uint32_t>(m_data.size() - (TEXT_OFFSET + sectionOffset));
        }
        
        // 创建段条目
        DatEntry entry;
        entry.name = name;
        entry.offset = TEXT_OFFSET + sectionOffset;
        entry.size = sectionSize;
        
        // 添加到段映射
        m_sections[entry.name] = entry;
    }
    
    return !m_sections.empty();
}

bool DatFile::isPureAscii(const std::string& str) const {
    std::regex pattern(R"(^[a-zA-Z0-9_\.%@(),:=\\]+$)");
    return std::regex_match(str, pattern);
}

void DatFile::updateSectionTable() {
    // 更新段表
    for (size_t i = 0; i < 100; ++i) {  // 假设最多100个段
        size_t offset = SECTION_TABLE_OFFSET + i * SECTION_ENTRY_SIZE;
        
        // 检查是否到达段表末尾
        if (offset + SECTION_ENTRY_SIZE > SECTION_TABLE_SIZE || m_data[offset] == 0) {
            break;
        }
        
        // 读取段名
        char name[SECTION_NAME_SIZE + 1] = {0};
        std::memcpy(name, &m_data[offset], SECTION_NAME_SIZE);
        
        // 查找段
        auto it = m_sections.find(name);
        if (it != m_sections.end()) {
            // 更新段偏移
            uint32_t sectionOffset = static_cast<uint32_t>(it->second.offset - TEXT_OFFSET);
            std::memcpy(&m_data[offset + SECTION_NAME_SIZE], &sectionOffset, sizeof(uint32_t));
        }
    }
}

} // namespace file
} // namespace eagls