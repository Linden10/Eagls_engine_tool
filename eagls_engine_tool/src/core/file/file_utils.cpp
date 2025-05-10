#include "core/file/file_utils.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <system_error>

namespace fs = std::filesystem;

namespace eagls {
namespace file {

std::vector<uint8_t> FileUtils::readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error: Cannot open file for reading: " << filename << std::endl;
        return {};
    }
    
    // 获取文件大小
    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    // 读取文件内容
    std::vector<uint8_t> buffer(size);
    if (file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        return buffer;
    }
    
    std::cerr << "Error: Failed to read file: " << filename << std::endl;
    return {};
}

bool FileUtils::writeFile(const std::string& filename, const std::vector<uint8_t>& data) {
    // 确保目录存在
    std::string dir = getFilePath(filename);
    if (!dir.empty() && !createDirectory(dir)) {
        std::cerr << "Error: Failed to create directory: " << dir << std::endl;
        return false;
    }
    
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error: Cannot open file for writing: " << filename << std::endl;
        return false;
    }
    
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    return file.good();
}

size_t FileUtils::getFileSize(const std::string& filename) {
    std::error_code ec;
    auto size = fs::file_size(filename, ec);
    if (ec) {
        std::cerr << "Error: Failed to get file size: " << filename << " - " << ec.message() << std::endl;
        return 0;
    }
    return size;
}

bool FileUtils::fileExists(const std::string& filename) {
    std::error_code ec;
    bool exists = fs::exists(filename, ec);
    if (ec) {
        std::cerr << "Error: Failed to check file existence: " << filename << " - " << ec.message() << std::endl;
        return false;
    }
    return exists;
}

bool FileUtils::createDirectory(const std::string& path) {
    if (path.empty()) {
        return true;  // 空路径视为成功
    }
    
    std::error_code ec;
    bool success = fs::create_directories(path, ec);
    if (ec && !fs::exists(path, ec)) {
        std::cerr << "Error: Failed to create directory: " << path << " - " << ec.message() << std::endl;
        return false;
    }
    return true;
}

std::vector<std::string> FileUtils::getFileList(const std::string& path, bool recursive) {
    std::vector<std::string> files;
    
    if (!fileExists(path)) {
        std::cerr << "Error: Path does not exist: " << path << std::endl;
        return files;
    }
    
    std::error_code ec;
    if (recursive) {
        for (const auto& entry : fs::recursive_directory_iterator(path, ec)) {
            if (ec) {
                std::cerr << "Error during directory iteration: " << ec.message() << std::endl;
                break;
            }
            
            if (entry.is_regular_file()) {
                files.push_back(entry.path().string());
            }
        }
    } else {
        for (const auto& entry : fs::directory_iterator(path, ec)) {
            if (ec) {
                std::cerr << "Error during directory iteration: " << ec.message() << std::endl;
                break;
            }
            
            if (entry.is_regular_file()) {
                files.push_back(entry.path().string());
            }
        }
    }
    
    return files;
}

std::string FileUtils::getFileExtension(const std::string& filename) {
    fs::path path(filename);
    return path.extension().string();
}

std::string FileUtils::getFileName(const std::string& filename) {
    fs::path path(filename);
    return path.stem().string();
}

std::string FileUtils::getFilePath(const std::string& filename) {
    fs::path path(filename);
    return path.parent_path().string();
}

std::string FileUtils::combinePath(const std::string& path1, const std::string& path2) {
    fs::path p1(path1);
    fs::path p2(path2);
    return (p1 / p2).string();
}

} // namespace file
} // namespace eagls