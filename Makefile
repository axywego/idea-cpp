# Компилятор
CXX = g++

# Флаги компиляции
CXXFLAGS = -std=c++17 -Iinclude

# Исходные файлы
COMMON_SRC = src/idea.cpp
KEY_GEN_SRC = src/key_gen.cpp
ENCRYPT_SRC = src/encrypt.cpp
DECRYPT_SRC = src/decrypt.cpp

# Целевые исполняемые файлы
TARGETS = key_gen.exe encrypt.exe decrypt.exe

# Правило для сборки всех целей
all: $(TARGETS)

# Правила для каждого исполняемого файла
key_gen.exe: $(KEY_GEN_SRC) $(COMMON_SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@

encrypt.exe: $(ENCRYPT_SRC) $(COMMON_SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@

decrypt.exe: $(DECRYPT_SRC) $(COMMON_SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Правило для очистки
clean:
	rm -f $(TARGETS)

# Удаление временных файлов (например, .o)
.PHONY: all clean