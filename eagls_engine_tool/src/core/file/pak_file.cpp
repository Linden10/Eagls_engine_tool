#include "core/file/pak_file.h"
#include "core/file/file_utils.h"
#include "core/encryption/eagls_encryption.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstring>

namespace eagls {
namespace file {

// PAK文件格式常量
constexpr size_t NAME_SIZE = 0x18;  // 文件名大小
constexpr size_t ENTRY_SIZE = 0x28; // 条目大小
constexpr size_t INDEX_SIZE = 0x61a84; // 索引大小
constexpr size_t DATA_OFFSET = 0x174b; // 数据偏移

// 索引加密密钥
const std::string INDEX_KEY = "1qaz2wsx3edc4rfv5tgb6yhn7ujm8ik,9ol.0p;/-@:^[]";

PakFile::PakFile() : m_isOpen(false) {
}

PakFile::~PakFile() {
    close();
}

bool PakFile::open(const std::string& pakFilename) {
    // 关闭已打开的文件
    close();
    
    // 检查PAK文件是否存在
    if (!FileUtils::fileExists(pakFilename)) {
        std::cerr << "Error: PAK file does not exist: " << pakFilename << std::endl;
        return false;
    }
    
    // 构造索引文件名
    std::string idxFilename = pakFilename;
    size_t extPos = idxFilename.rfind('.');
    if (extPos != std::string::npos) {
        idxFilename = idxFilename.substr(0, extPos);
    }
    idxFilename += ".idx";
    
    // 检查索引文件是否存在
    if (!FileUtils::fileExists(idxFilename)) {
        std::cerr << "Error: Index file does not exist: " << idxFilename << std::endl;
        return false;
    }
    
    // 读取索引文件
    if (!readIndex(idxFilename)) {
        std::cerr << "Error: Failed to read index file: " << idxFilename << std::endl;
        return false;
    }
    
    m_pakFilename = pakFilename;
    m_isOpen = true;
    
    return true;
}

void PakFile::close() {
    m_entries.clear();
    m_pakFilename.clear();
    m_isOpen = false;
}

std::vector<std::string> PakFile::getFileList() const {
    std::vector<std::string> files;
    
    for (const auto& entry : m_entries) {
        files.push_back(entry.first);
    }
    
    return files;
}

bool PakFile::extractFile(const std::string& filename, const std::string& outputPath, bool decrypt) {
    if (!m_isOpen) {
        std::cerr << "Error: PAK file is not open" << std::endl;
        return false;
    }
    
    // 查找文件条目
    auto it = m_entries.find(filename);
    if (it == m_entries.end()) {
        std::cerr << "Error: File not found in PAK: " << filename << std::endl;
        return false;
    }
    
    const PakEntry& entry = it->second;
    
    // 打开PAK文件
    std::ifstream pakFile(m_pakFilename, std::ios::binary);
    if (!pakFile) {
        std::cerr << "Error: Cannot open PAK file: " << m_pakFilename << std::endl;
        return false;
    }
    
    // 读取文件数据
    pakFile.seekg(entry.offset);
    std::vector<uint8_t> data(entry.size);
    if (!pakFile.read(reinterpret_cast<char*>(data.data()), entry.size)) {
        std::cerr << "Error: Failed to read file data from PAK: " << filename << std::endl;
        return false;
    }
    
    // 如果需要解密
    if (decrypt) {
        // 根据文件类型选择解密方法
        if (filename.find(".dat") != std::string::npos) {
            // DAT文件使用EAGLS加密
            encryption::EaglsEncryption enc;
            data = enc.decrypt(data);
        } else if (filename.find(".gr") != std::string::npos) {
            // GR文件使用Lehmer加密
            encryption::LehmerEncryption enc;
            data = enc.decrypt(data);
        }
    }
    
    // 构造输出文件路径
    std::string outputFilename = FileUtils::combinePath(outputPath, filename);
    
    // 确保输出目录存在
    std::string outputDir = FileUtils::getFilePath(outputFilename);
    if (!outputDir.empty() && !FileUtils::createDirectory(outputDir)) {
        std::cerr << "Error: Failed to create output directory: " << outputDir << std::endl;
        return false;
    }
    
    // 写入文件
    if (!FileUtils::writeFile(outputFilename, data)) {
        std::cerr << "Error: Failed to write output file: " << outputFilename << std::endl;
        return false;
    }
    
    return true;
}

bool PakFile::extractAllFiles(const std::string& outputPath, bool decrypt) {
    if (!m_isOpen) {
        std::cerr << "Error: PAK file is not open" << std::endl;
        return false;
    }
    
    // 确保输出目录存在
    if (!FileUtils::createDirectory(outputPath)) {
        std::cerr << "Error: Failed to create output directory: " << outputPath << std::endl;
        return false;
    }
    
    bool success = true;
    
    // 提取所有文件
    for (const auto& entry : m_entries) {
        if (!extractFile(entry.first, outputPath, decrypt)) {
            std::cerr << "Error: Failed to extract file: " << entry.first << std::endl;
            success = false;
        }
    }
    
    return success;
}

bool PakFile::create(const std::string& pakFilename, const std::vector<std::string>& files, bool encrypt) {
    // 关闭已打开的文件
    close();
    
    // 检查文件列表是否为空
    if (files.empty()) {
        std::cerr << "Error: No files to pack" << std::endl;
        return false;
    }
    
    // 创建PAK文件
    std::ofstream pakFile(pakFilename, std::ios::binary);
    if (!pakFile) {
        std::cerr << "Error: Cannot create PAK file: " << pakFilename << std::endl;
        return false;
    }
    
    // 创建索引文件名
    std::string idxFilename = pakFilename;
    size_t extPos = idxFilename.rfind('.');
    if (extPos != std::string::npos) {
        idxFilename = idxFilename.substr(0, extPos);
    }
    idxFilename += ".idx";
    
    // 初始化条目映射
    std::map<std::string, PakEntry> entries;
    
    // 写入文件数据
    uint64_t offset = DATA_OFFSET;
    
    for (const auto& filename : files) {
        // 读取文件数据
        std::vector<uint8_t> data = FileUtils::readFile(filename);
        if (data.empty()) {
            std::cerr << "Error: Failed to read file: " << filename << std::endl;
            continue;
        }
        
        // 如果需要加密
        if (encrypt) {
            // 根据文件类型选择加密方法
            if (filename.find(".dat") != std::string::npos) {
                // DAT文件使用EAGLS加密
                encryption::EaglsEncryption enc;
                data = enc.encrypt(data);
            } else if (filename.find(".gr") != std::string::npos) {
                // GR文件使用Lehmer加密
                encryption::LehmerEncryption enc;
                data = enc.encrypt(data);
            }
        }
        
        // 创建条目
        PakEntry entry;
        entry.name = FileUtils::getFileName(filename) + FileUtils::getFileExtension(filename);
        entry.offset = offset;
        entry.size = data.size();
        entry.flags = 0;
        
        // 添加到条目映射
        entries[entry.name] = entry;
        
        // 写入文件数据
        pakFile.write(reinterpret_cast<const char*>(data.data()), data.size());
        
        // 更新偏移
        offset += data.size();
    }
    
    pakFile.close();
    
    // 写入索引文件
    if (!writeIndex(idxFilename, entries)) {
        std::cerr << "Error: Failed to write index file: " << idxFilename << std::endl;
        return false;
    }
    
    m_pakFilename = pakFilename;
    m_entries = entries;
    m_isOpen = true;
    
    return true;
}

bool PakFile::addFile(const std::string& pakFilename, const std::string& filename, bool encrypt) {
    // 如果PAK文件不存在，创建新的PAK文件
    if (!FileUtils::fileExists(pakFilename)) {
        return create(pakFilename, {filename}, encrypt);
    }
    
    // 如果PAK文件已经打开，关闭它
    if (m_isOpen) {
        close();
    }
    
    // 打开PAK文件
    if (!open(pakFilename)) {
        std::cerr << "Error: Failed to open PAK file: " << pakFilename << std::endl;
        return false;
    }
    
    // 读取文件数据
    std::vector<uint8_t> data = FileUtils::readFile(filename);
    if (data.empty()) {
        std::cerr << "Error: Failed to read file: " << filename << std::endl;
        return false;
    }
    
    // 如果需要加密
    if (encrypt) {
        // 根据文件类型选择加密方法
        if (filename.find(".dat") != std::string::npos) {
            // DAT文件使用EAGLS加密
            encryption::EaglsEncryption enc;
            data = enc.encrypt(data);
        } else if (filename.find(".gr") != std::string::npos) {
            // GR文件使用Lehmer加密
            encryption::LehmerEncryption enc;
            data = enc.encrypt(data);
        }
    }
    
    // 创建条目
    PakEntry entry;
    entry.name = FileUtils::getFileName(filename) + FileUtils::getFileExtension(filename);
    
    // 检查文件是否已存在
    auto it = m_entries.find(entry.name);
    if (it != m_entries.end()) {
        std::cerr << "Error: File already exists in PAK: " << entry.name << std::endl;
        return false;
    }
    
    // 获取PAK文件大小
    size_t pakSize = FileUtils::getFileSize(pakFilename);
    
    // 设置条目信息
    entry.offset = pakSize;
    entry.size = data.size();
    entry.flags = 0;
    
    // 添加到条目映射
    m_entries[entry.name] = entry;
    
    // 打开PAK文件进行追加
    std::ofstream pakFile(pakFilename, std::ios::binary | std::ios::app);
    if (!pakFile) {
        std::cerr << "Error: Cannot open PAK file for appending: " << pakFilename << std::endl;
        return false;
    }
    
    // 写入文件数据
    pakFile.write(reinterpret_cast<const char*>(data.data()), data.size());
    pakFile.close();
    
    // 构造索引文件名
    std::string idxFilename = pakFilename;
    size_t extPos = idxFilename.rfind('.');
    if (extPos != std::string::npos) {
        idxFilename = idxFilename.substr(0, extPos);
    }
    idxFilename += ".idx";
    
    // 写入索引文件
    if (!writeIndex(idxFilename, m_entries)) {
        std::cerr << "Error: Failed to write index file: " << idxFilename << std::endl;
        return false;
    }
    
    return true;
}

bool PakFile::readIndex(const std::string& idxFilename) {
    // 读取索引文件
    std::vector<uint8_t> indexData = FileUtils::readFile(idxFilename);
    if (indexData.empty() || indexData.size() != INDEX_SIZE) {
        std::cerr << "Error: Invalid index file size: " << idxFilename << std::endl;
        return false;
    }
    
    // 解密索引
    encryption::CRuntimeRandomGenerator rng;
    rng.srand(*reinterpret_cast<uint32_t*>(&indexData[INDEX_SIZE - 4]));
    
    std::vector<uint8_t> decryptedIndex(INDEX_SIZE - 4);
    for (size_t i = 0; i < INDEX_SIZE - 4; ++i) {
        decryptedIndex[i] = indexData[i] ^ INDEX_KEY[rng.rand() % INDEX_KEY.size()];
    }
    
    // 解析索引
    m_entries.clear();
    
    for (size_t i = 0; i < (INDEX_SIZE - 4) / ENTRY_SIZE; ++i) {
        size_t offset = i * ENTRY_SIZE;
        
        // 检查是否到达索引末尾
        if (decryptedIndex[offset] == 0) {
            break;
        }
        
        // 读取文件名
        char name[NAME_SIZE + 1] = {0};
        std::memcpy(name, &decryptedIndex[offset], NAME_SIZE);
        
        // 读取条目信息
        PakEntry entry;
        entry.name = name;
        entry.offset = *reinterpret_cast<uint64_t*>(&decryptedIndex[offset + NAME_SIZE]);
        entry.size = *reinterpret_cast<uint32_t*>(&decryptedIndex[offset + NAME_SIZE + 8]);
        entry.flags = *reinterpret_cast<uint32_t*>(&decryptedIndex[offset + NAME_SIZE + 12]);
        
        // 添加到条目映射
        m_entries[entry.name] = entry;
    }
    
    return true;
}

bool PakFile::writeIndex(const std::string& idxFilename, const std::map<std::string, PakEntry>& entries) {
    // 创建索引数据
    std::vector<uint8_t> indexData(INDEX_SIZE, 0);
    
    // 写入条目
    size_t entryCount = 0;
    for (const auto& entry : entries) {
        if (entryCount >= (INDEX_SIZE - 4) / ENTRY_SIZE) {
            std::cerr << "Warning: Too many entries, some will be omitted" << std::endl;
            break;
        }
        
        size_t offset = entryCount * ENTRY_SIZE;
        
        // 写入文件名
        std::memcpy(&indexData[offset], entry.first.c_str(), std::min(entry.first.size(), NAME_SIZE));
        
        // 写入条目信息
        *reinterpret_cast<uint64_t*>(&indexData[offset + NAME_SIZE]) = entry.second.offset;
        *reinterpret_cast<uint32_t*>(&indexData[offset + NAME_SIZE + 8]) = entry.second.size;
        *reinterpret_cast<uint32_t*>(&indexData[offset + NAME_SIZE + 12]) = entry.second.flags;
        
        entryCount++;
    }
    
    // 设置索引尾部标记
    indexData[INDEX_SIZE - 4] = 0x60;
    
    // 加密索引
    encryption::CRuntimeRandomGenerator rng;
    rng.srand(*reinterpret_cast<uint32_t*>(&indexData[INDEX_SIZE - 4]));
    
    std::vector<uint8_t> encryptedIndex(INDEX_SIZE);
    for (size_t i = 0; i < INDEX_SIZE - 4; ++i) {
        encryptedIndex[i] = indexData[i] ^ INDEX_KEY[rng.rand() % INDEX_KEY.size()];
    }
    
    // 复制尾部4字节
    std::memcpy(&encryptedIndex[INDEX_SIZE - 4], &indexData[INDEX_SIZE - 4], 4);
    
    // 写入索引文件
    return FileUtils::writeFile(idxFilename, encryptedIndex);
}

} // namespace file
} // namespace eagls