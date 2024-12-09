#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include "constant.h"

std::vector<int> hex_to_binary_code(const std::string& code16) {
    std::vector<int> code2((code16.length() - 2) * 4, 0); 

    for (size_t i = 2; i < code16.length(); ++i) {
        auto binary_bits = chars16_to_2[code16[i]];
        for (int j = 0; j < 4; ++j) {
            code2[(i - 2) * 4 + j] = binary_bits[j];
        }
    }
    return code2;
}

bool is_valid(const std::string& s) {
    
    if (s.size() < 3 || (s.substr(0, 2) != "0x" && s.substr(0, 2) != "0X")) {
        return false;
    }
    
   
    for (size_t i = 2; i < s.size(); ++i) {
        char c = s[i];
        if (!isxdigit(c)) {  // Функция isxdigit проверяет, является ли символ шестнадцатеричным
            return false;
        }
    }
    return true;
}

int need_bits(int x) {
    int need = 0;
    while ((1 << need) <= x) {
        need++;
    }
    return need;
}

std::vector<int> integer_to_bin(int x, int bits) {
    std::vector<int> result(bits);
    for (int i = 0; i < bits; ++i) {
        result[i] = (x & (1 << (bits - 1 - i))) ? 1 : 0;
    }
    return result;
}

bool none_of_exp(std::vector<int>::iterator begin, std::vector<int>::iterator end) {
    for (auto it = begin; it != end; ++it) {
        if (*it == 1) {
            return false;
        }
    }
    return true;
}

int bin_to_integer(const std::vector<int>& code) {
    int t = 0;
    for (int i = 0; i < int(code.size()); ++i) {
        t += (1 << i) * code[code.size() - 1 - i];
    }
    return t;
}


