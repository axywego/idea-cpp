# C++ IDEA Encryption/Decryption
C++ implementation of the IDEA block cipher algorithm

## File structure
 - include:
   - idea.hpp - header file for idea.cpp.
 - src:
   - decrypt.cpp - file with decryption impelemtation
   - encrypt.cpp - file with encryption impelemtation
   - encrypt.cpp - file with key generator impelemtation

## Build instructions
### Using make
```bash
 make 
```
### GCC

```bash
    g++ src/key_gen.cpp include/idea.hpp src/idea.cpp -o key_gen.exe
    g++ src/encrypt.cpp include/idea.hpp src/idea.cpp -o encrypt.exe
    g++ src/decrypt.cpp include/idea.hpp src/idea.cpp -o decrypt.exe
```

## Building result 
 - **key_gen.exe** - Generates a 128-bit key saves it to the key.bin.
 - **encrypt.exe** - Encrypts the file with the key from key.bin and saves the encryption to another file.
 - **decrypt.exe** - Decrypts a file using a key from key.bin and saves the result to another file.
