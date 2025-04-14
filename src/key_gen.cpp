#include "../include/idea.hpp"

int main()
{
    auto bin_key = generate16byteBinaryKey();
    std::cout << bytesToHexString(bin_key) << '\n';
    createBinKeyFile(bin_key);
    std::cout << "Key successfully generated!\n";
    system("pause");
}