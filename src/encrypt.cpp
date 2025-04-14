#include "../include/idea.hpp"

int main()
{
    std::string source, dest, key_source;
    std::cout << "Enter source key name:\n>>> ";
    std::getline(std::cin, key_source);
    std::cout << "Enter source file name:\n>>> ";
    std::getline(std::cin, source);
    std::cout << "Enter destination file name:\n>>> ";
    std::getline(std::cin, dest);

    auto key_str = read16BytesAsHexString(key_source);

    std::string part1_str = key_str.substr(0, 16);
    std::string part2_str = key_str.substr(16, 16);

    uint64_t key_part1 = stoull(part1_str, nullptr, 16);
    uint64_t key_part2 = stoull(part2_str, nullptr, 16);

    IDEA idea(key_part1, key_part2);
    idea.encrypt(source, dest);
    std::cout << "File successfully encrypted!\n";
    system("pause");
}