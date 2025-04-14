#ifndef IDEA_HPP
#define IDEA_HPP

#include <iostream>
#include <string>
#include <cstdint>
#include <tuple>
#include <fstream>
#include <vector>
#include <array>
#include <bitset>
#include <random>
#include <sstream>
#include <iomanip>

std::vector<uint64_t> string_to_blocksUINT64(const std::string &str);
std::string blocksUINT64_to_string(const std::vector<uint64_t> &blocks);

std::vector<uint64_t> readDataFromFileUINT64(const std::string &fileName);
void writeDataToFileUINT64(const std::string &fileName, const std::vector<uint64_t> &blocks);

std::string bytesToHexString(const std::array<unsigned char, 16> &bytes);

std::string read16BytesAsHexString(const std::string &filename);
std::array<unsigned char, 16> generate16byteBinaryKey();
void createBinKeyFile(const std::array<unsigned char, 16> &bytes);

std::pair<uint64_t, uint64_t> getKeyFromFile(const std::string &fileName);

class IDEA
{
private:
    std::vector<std::vector<uint16_t>> _keys;

    std::vector<uint64_t> string_to_blocks(const std::string &str);
    std::string blocks_to_string(const std::vector<uint64_t> &blocks);

    uint16_t mul_mod(uint16_t a, uint16_t b);
    uint16_t add_mod(uint16_t a, uint16_t b);
    uint16_t add_inv(uint16_t key);
    int32_t mul_inv(int32_t key);

    std::tuple<uint16_t, uint16_t, uint16_t, uint16_t>
    round(uint16_t p1, uint16_t p2, uint16_t p3, uint16_t p4, const std::vector<uint16_t> &keys);

    uint64_t encrypt_block(uint64_t plain_block);
    uint64_t decrypt_block(uint64_t encrypted_block);

public:
    IDEA(uint64_t key_high, uint64_t key_low);
    void gen_keys(uint64_t key_high, uint64_t key_low);

    void encrypt(const std::string &sourceFile, const std::string &destFile);
    void decrypt(const std::string &sourceFile, const std::string &destFile);
};

#endif IDEA_HPP