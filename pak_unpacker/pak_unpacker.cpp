#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdint>
#include <filesystem>

const char* IndexKey = "1qaz2wsx3edc4rfv5tgb6yhn7ujm8ik,9ol.0p;/-@:^[]";
const char* EaglsKey = "EAGLS_SYSTEM";

// 文件描述结构体
struct FileDesc {
    char filename[0x18];  // 文件名，固定长度为0x18字节
    uint64_t offset;      // 文件在pak中的偏移量
    uint32_t size;        // 文件大小
    uint32_t reserved;    // 保留字段
};

class CRuntimeRandomGenerator {
public:
    void srand(int32_t seed) {
        m_seed = seed;
    }

    int16_t rand() {
        m_seed = m_seed * 214013 + 2531011;
        return (int16_t)(m_seed >> 16) & 0x7FFF;
    }

private:
    uint32_t m_seed = 0;
};

class LehmerRandomGenerator {
public:
    LehmerRandomGenerator() : m_seed(0) {}

    void srand(int32_t seed) {
        m_seed = seed ^ 123459876;
    }

    int32_t rand() {
        m_seed = (48271 * (m_seed % 44488) - 3399 * (m_seed / 44488));
        if (m_seed < 0) {
            m_seed += 2147483647;
        }
        // 使用static_cast明确转换，避免警告
        return static_cast<int32_t>(m_seed * 4.656612875245797e-10 * 256);
    }

private:
    int32_t m_seed;
};

// 解密idx文件
void DecryptIndex(std::vector<uint8_t>& data) {
    CRuntimeRandomGenerator rng;
    // 使用idx文件末尾的4字节作为种子
    uint32_t seed = *reinterpret_cast<const uint32_t*>(data.data() + data.size() - 4);
    rng.srand(seed);
    size_t len_IndexKey = strlen(IndexKey);

    // 解密idx文件内容（除了最后4字节的种子）
    for (size_t i = 0; i < data.size() - 4; ++i) {
        data[i] ^= IndexKey[rng.rand() % len_IndexKey];
    }
}

// 解密CG文件
void DecryptCg(std::vector<uint8_t>& data) {
    LehmerRandomGenerator rng;
    // 使用文件最后一个字节作为种子
    rng.srand(data.back());
    size_t limit = data.size() - 1;
    if (limit > 0x174b)
        limit = 0x174b;

    // 解密文件内容
    for (size_t i = 0; i < limit; ++i) {
        data[i] ^= EaglsKey[rng.rand() % 12];
    }
}

// 解密DAT文件
void DecryptDat(std::vector<uint8_t>& data) {
    CRuntimeRandomGenerator rng;
    int text_offset = 3600;
    // 使用static_cast明确转换size_t到int，避免警告
    int text_length = static_cast<int>(data.size()) - text_offset - 2;

    // 使用文件最后一个字节作为种子
    rng.srand((int8_t)data[data.size() - 1]);

    // 解密文件内容，每隔2字节解密一次
    for (int i = 0; i < text_length; i += 2) {
        data[text_offset + i] ^= EaglsKey[rng.rand() % 12];
    }
}

// 根据文件扩展名选择合适的解密方法
void DecryptFile(std::vector<uint8_t>& data, const std::string& filename) {
    std::string extension = filename.substr(filename.find_last_of(".") + 1);

    if (extension == "dat") {
        DecryptDat(data);
        std::cout << "已解密DAT文件: " << filename << std::endl;
    } else if (extension == "gr") {
        DecryptCg(data);
        std::cout << "已解密GR文件: " << filename << std::endl;
    } else {
        std::cout << "未知文件类型，不进行解密: " << filename << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "用法: " << argv[0] << " <pak文件路径> <输出目录> [解密=1]" << std::endl;
        return 1;
    }

    std::string pak_path = argv[1];
    std::string output_dir = argv[2];
    bool decrypt = true;

    if (argc > 3) {
        decrypt = (std::string(argv[3]) == "1");
    }

    // 确保输出目录存在
    std::filesystem::create_directories(output_dir);

    // 构造idx文件路径
    std::string idx_path = pak_path.substr(0, pak_path.length() - 3) + "idx";

    // 读取idx文件
    std::ifstream idx_file(idx_path, std::ios::binary);
    if (!idx_file.is_open()) {
        std::cerr << "无法打开idx文件: " << idx_path << std::endl;
        return 1;
    }

    // 读取idx文件内容
    std::vector<uint8_t> idx_data((std::istreambuf_iterator<char>(idx_file)), std::istreambuf_iterator<char>());
    idx_file.close();

    // 解密idx文件
    DecryptIndex(idx_data);
    std::cout << "已解密idx文件" << std::endl;

    // 读取pak文件
    std::ifstream pak_file(pak_path, std::ios::binary);
    if (!pak_file.is_open()) {
        std::cerr << "无法打开pak文件: " << pak_path << std::endl;
        return 1;
    }

    // 解析idx文件中的文件信息
    size_t file_count = 0;
    for (size_t i = 0; i < idx_data.size() - 4; i += sizeof(FileDesc)) {
        FileDesc* file_desc = reinterpret_cast<FileDesc*>(idx_data.data() + i);

        // 检查文件名是否为空，如果是则跳过
        if (file_desc->filename[0] == 0) {
            continue;
        }

        std::string filename(file_desc->filename);
        // 移除文件名中的空字符
        filename = filename.c_str();

        if (filename.empty()) {
            continue;
        }

        std::cout << "发现文件: " << filename << ", 偏移量: " << file_desc->offset << ", 大小: " << file_desc->size << std::endl;

        // 从pak文件中提取文件内容
        std::vector<uint8_t> file_data(file_desc->size);
        pak_file.seekg(file_desc->offset - 0x174b);  // 调整偏移量
        pak_file.read(reinterpret_cast<char*>(file_data.data()), file_desc->size);

        // 如果需要解密，则解密文件
        if (decrypt) {
            DecryptFile(file_data, filename);
        }

        // 保存文件
        std::string output_path = output_dir + "/" + filename;
        std::ofstream output_file(output_path, std::ios::binary);
        if (!output_file.is_open()) {
            std::cerr << "无法创建输出文件: " << output_path << std::endl;
            continue;
        }

        output_file.write(reinterpret_cast<const char*>(file_data.data()), file_data.size());
        output_file.close();

        std::cout << "已保存文件: " << output_path << std::endl;
        file_count++;
    }

    pak_file.close();
    std::cout << "解包完成，共提取了 " << file_count << " 个文件" << std::endl;

    return 0;
}
