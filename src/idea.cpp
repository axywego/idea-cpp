#include "../include/idea.hpp"

std::vector<uint64_t> string_to_blocksUINT64(const std::string &str)
{
    std::vector<uint64_t> blocks;
    uint64_t current_block = 0;
    int byte_count = 0;

    for (const char &c : str)
    {
        current_block = (current_block << 8) | static_cast<uint8_t>(c);
        byte_count++;

        if (byte_count == 8)
        {
            blocks.push_back(current_block);
            current_block = 0;
            byte_count = 0;
        }
    }

    // Добавляем последний неполный блок
    if (byte_count > 0)
    {
        // Дополняем нулями справа
        current_block <<= (8 * (8 - byte_count));
        blocks.push_back(current_block);
    }

    return blocks;
}

std::string blocksUINT64_to_string(const std::vector<uint64_t> &blocks)
{
    std::string result;

    for (uint64_t block : blocks)
    {
        for (int i = 7; i >= 0; --i)
        {
            uint8_t byte = (block >> (i * 8)) & 0xFF;
            if (byte != 0)
            { // Пропускаем нулевые байты дополнения
                result += static_cast<char>(byte);
            }
        }
    }

    return result;
}

std::vector<uint64_t> readDataFromFileUINT64(const std::string &fileName)
{
    std::ifstream f(fileName);
    std::string lines;
    if (f.is_open())
    {
        std::string line;
        while (std::getline(f, line))
        {
            lines += line;
        }
    }
    f.close();
    return string_to_blocksUINT64(lines);
}

void writeDataToFileUINT64(const std::string &fileName, const std::vector<uint64_t> &blocks)
{
    std::ofstream f(fileName);
    auto data = blocksUINT64_to_string(blocks);
    while (data.length() % 8 != 0)
    {
        data += ' ';
    }
    if (f.is_open())
    {
        f.write(data.c_str(), data.length());
    }
    f.close();
}

std::array<unsigned char, 16> generate16byteBinaryKey()
{
    std::array<unsigned char, 16> bytes{};

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint16_t> dist(0, 255); // Случайный байт (0-255)

    for (auto &byte : bytes)
    {
        byte = static_cast<unsigned char>(dist(gen));
    }

    return bytes;
}

std::string bytesToHexString(const std::array<unsigned char, 16> &bytes)
{
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (const auto &byte : bytes)
    {
        oss << std::setw(2) << static_cast<int>(byte);
    }
    return oss.str();
}

void createBinKeyFile(const std::array<unsigned char, 16> &bytes)
{
    std::ofstream outFile("key.bin", std::ios::binary);
    if (!outFile)
    {
        throw std::runtime_error("Cannot open file for writing");
    }

    outFile.write(reinterpret_cast<const char *>(bytes.data()), bytes.size());

    // Явная проверка, что записалось ровно 16 байт
    if (!outFile || outFile.tellp() != 16)
    {
        throw std::runtime_error("Failed to write exactly 16 bytes");
    }
    outFile.close();
}

std::string read16BytesAsHexString(const std::string &filename)
{
    // 1. Чтение 16 байт из файла
    std::array<unsigned char, 16> bytes;
    std::ifstream file(filename, std::ios::binary);

    if (!file)
    {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    file.read(reinterpret_cast<char *>(bytes.data()), bytes.size());

    if (file.gcount() != bytes.size())
    {
        throw std::runtime_error("File doesn't contain exactly 16 bytes");
    }

    // 2. Преобразование в HEX-строку
    std::ostringstream hexStream;
    hexStream << std::hex << std::setfill('0');

    for (const auto &byte : bytes)
    {
        hexStream << std::setw(2) << static_cast<int>(byte);
    }

    return hexStream.str();
}

uint16_t IDEA::mul_mod(uint16_t a, uint16_t b)
{

    if (a == 0)
        a = 0x10000;
    if (b == 0)
        b = 0x10000;

    uint32_t r = (static_cast<uint32_t>(a) * static_cast<uint32_t>(b)) % 0x10001;

    if (r == 0x10000)
        r = 0;

    return static_cast<uint16_t>(r);
}

uint16_t IDEA::add_mod(uint16_t a, uint16_t b)
{
    return (a + b) % 0x10000;
}

uint16_t IDEA::add_inv(uint16_t key)
{
    uint16_t u = (0x10000 - key) % 0xFFFF;
    return u;
}

int32_t IDEA::mul_inv(int32_t key)
{
    int32_t a = 0x10000 + 1;
    if (key == 0)
        return 0;

    int32_t x = 0, y = 0;
    int32_t x1 = 0, x2 = 1;
    int32_t y1 = 1, y2 = 0;

    while (key > 0)
    {
        int32_t q = a / key;
        int32_t r = a - q * key;
        x = x2 - q * x1;
        y = y2 - q * y1;
        a = key;
        key = r;
        x2 = x1;
        x1 = x;
        y2 = y1;
        y1 = y;
    }

    if (y2 < 0)
        y2 += 0x10001; // Коррекция отрицательных значений
    return y2;
}

std::tuple<uint16_t, uint16_t, uint16_t, uint16_t>
IDEA::round(uint16_t p1, uint16_t p2, uint16_t p3, uint16_t p4, const std::vector<uint16_t> &keys)
{
    uint16_t k1 = keys[0], k2 = keys[1], k3 = keys[2], k4 = keys[3], k5 = keys[4], k6 = keys[5];

    p1 = mul_mod(p1, k1);
    p4 = mul_mod(p4, k4);
    p2 = add_mod(p2, k2);
    p3 = add_mod(p3, k3);

    uint16_t x = p1 ^ p3;
    uint16_t t0 = mul_mod(k5, x);
    x = p2 ^ p4;
    x = add_mod(t0, x);
    uint16_t t1 = mul_mod(k6, x);
    uint16_t t2 = add_mod(t0, t1);

    p1 ^= t1;
    p4 ^= t2;
    uint16_t a = p2 ^ t2;
    p2 = p3 ^ t1;
    p3 = a;

    return {p1, p2, p3, p4};
}

IDEA::IDEA(uint64_t key_high, uint64_t key_low)
{
    gen_keys(key_high, key_low);
}

void IDEA::gen_keys(uint64_t key_high, uint64_t key_low)
{
    std::vector<uint16_t> sub_keys;

    for (int i = 0; i < 52; ++i)
    {
        // Вычисляем, из какой части ключа брать биты (high или low)
        bool use_high_part = (i % 8) < 4; // Первые 4 подключа берутся из key_high, следующие 4 — из key_low
        uint64_t current_part = use_high_part ? key_high : key_low;

        // Позиция битов в текущей части (key_high или key_low)
        int shift = (112 - 16 * (i % 8)) % 64;
        if (shift < 0)
            shift += 64; // Корректировка для отрицательных сдвигов

        uint16_t sub_key = (current_part >> shift) & 0xFFFF;
        sub_keys.push_back(sub_key);

        if (i % 8 == 7)
        {
            // Циклический сдвиг 128 бит влево на 25 позиций
            uint64_t carry_high = key_high >> (64 - 25); // Биты, которые "переедут" из key_high в key_low
            uint64_t carry_low = key_low >> (64 - 25);   // Биты, которые "переедут" из key_low в key_high

            uint64_t new_high = (key_high << 25) | carry_low;
            uint64_t new_low = (key_low << 25) | carry_high;

            key_high = new_high;
            key_low = new_low;
        }
    }

    // Формируем _keys (как в оригинале)
    for (int i = 0; i < 9; ++i)
    {
        std::vector<uint16_t> round_keys(sub_keys.begin() + 6 * i, sub_keys.begin() + 6 * (i + 1));
        _keys.push_back(round_keys);
    }
}

uint64_t IDEA::encrypt_block(uint64_t plain)
{
    uint16_t p1 = (plain >> 48) & 0xFFFF;
    uint16_t p2 = (plain >> 32) & 0xFFFF;
    uint16_t p3 = (plain >> 16) & 0xFFFF;
    uint16_t p4 = plain & 0xFFFF;

    for (int i = 0; i < 8; ++i)
    {
        auto [np1, np2, np3, np4] = round(p1, p2, p3, p4, _keys[i]);
        p1 = np1;
        p2 = np2;
        p3 = np3;
        p4 = np4;
    }

    uint16_t k1 = _keys[8][0], k2 = _keys[8][1], k3 = _keys[8][2], k4 = _keys[8][3];
    uint16_t y1 = mul_mod(p1, k1);
    uint16_t y2 = add_mod(p3, k2);
    uint16_t y3 = add_mod(p2, k3);
    uint16_t y4 = mul_mod(p4, k4);

    return (static_cast<uint64_t>(y1) << 48) | (static_cast<uint64_t>(y2) << 32) |
           (static_cast<uint64_t>(y3) << 16) | y4;
}

uint64_t IDEA::decrypt_block(uint64_t encrypted)
{
    uint16_t p1 = (encrypted >> 48) & 0xFFFF;
    uint16_t p2 = (encrypted >> 32) & 0xFFFF;
    uint16_t p3 = (encrypted >> 16) & 0xFFFF;
    uint16_t p4 = encrypted & 0xFFFF;

    // Round 1
    auto keys = _keys[8];
    int32_t k1 = mul_inv(keys[0]);
    if (k1 < 0)
        k1 += 0x10001;
    uint16_t k2 = add_inv(keys[1]);
    uint16_t k3 = add_inv(keys[2]);
    int32_t k4 = mul_inv(keys[3]);
    if (k4 < 0)
        k4 += 0x10001;
    keys = _keys[7];
    uint16_t k5 = keys[4], k6 = keys[5];
    std::vector<uint16_t> round_keys = {static_cast<uint16_t>(k1), k2, k3, static_cast<uint16_t>(k4), k5, k6};
    std::tie(p1, p2, p3, p4) = round(p1, p2, p3, p4, round_keys);

    // Other rounds
    for (int i = 1; i < 8; ++i)
    {
        keys = _keys[8 - i];
        k1 = mul_inv(keys[0]);
        if (k1 < 0)
            k1 += 0x10001;
        k2 = add_inv(keys[2]);
        k3 = add_inv(keys[1]);
        k4 = mul_inv(keys[3]);
        if (k4 < 0)
            k4 += 0x10001;
        keys = _keys[7 - i];
        k5 = keys[4], k6 = keys[5];
        round_keys = {static_cast<uint16_t>(k1), k2, k3, static_cast<uint16_t>(k4), k5, k6};
        std::tie(p1, p2, p3, p4) = round(p1, p2, p3, p4, round_keys);
    }

    // Final output transformation
    keys = _keys[0];
    k1 = mul_inv(keys[0]);
    if (k1 < 0)
        k1 += 0x10001;
    k2 = add_inv(keys[1]);
    k3 = add_inv(keys[2]);
    k4 = mul_inv(keys[3]);
    if (k4 < 0)
        k4 += 0x10001;
    uint16_t y1 = mul_mod(p1, static_cast<uint16_t>(k1));
    uint16_t y2 = add_mod(p3, k2);
    uint16_t y3 = add_mod(p2, k3);
    uint16_t y4 = mul_mod(p4, static_cast<uint16_t>(k4));

    return (static_cast<uint64_t>(y1) << 48) | (static_cast<uint64_t>(y2) << 32) |
           (static_cast<uint64_t>(y3) << 16) | y4;
}

void IDEA::encrypt(const std::string &sourceFile, const std::string &destFile)
{
    auto ref_blocks = readDataFromFileUINT64(sourceFile);

    decltype(ref_blocks) encrypted_blocks;
    for (const auto &block : ref_blocks)
    {
        auto encrypted_block = encrypt_block(block);
        encrypted_blocks.push_back(encrypted_block);
    }
    writeDataToFileUINT64(destFile, encrypted_blocks);
}

void IDEA::decrypt(const std::string &sourceFile, const std::string &destFile)
{
    auto ref_blocks = readDataFromFileUINT64(sourceFile);
    decltype(ref_blocks) decrypted_blocks;
    for (const auto &block : ref_blocks)
    {
        auto decrypted_block = decrypt_block(block);
        decrypted_blocks.emplace_back(decrypted_block);
    }
    writeDataToFileUINT64(destFile, decrypted_blocks);
}