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

long long floor_div(long long a, long long b) {
    int result = a / b;
    // Если результат округлился к нулю, но a и b имеют разные знаки, уменьшаем результат на 1
    if ((a < 0) != (b < 0)) {
        result -= 1;
    }
    return result;
}


std::vector<int> int_to_code(long long x, int bits) {
    std::vector<int> code(bits, 0);
    x %= (1 << bits); 

    if (x >= (1 << (bits - 1))) {
        x -= (1 << bits); 
    }

    if (x < 0) {
        code[0] = 1;
        x += (1 << (bits - 1));
    }

    for (int i = bits - 1; i > 0; --i) {
        code[i] = x % 2;
        x >>= 1;
    }
    return code;
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

bool has_not_0(const std::string& s)  {
    return s.find_first_not_of('0') != std::string::npos;
}

std::pair<std::string, int> increase(std::string& after_dot, int& before_dot) {
    if (after_dot != "999") {
        after_dot = std::to_string(std::stoi(after_dot) + 1);
        after_dot = std::string(3 - after_dot.length(), '0') + after_dot;
    } else {
        after_dot = "000";
        before_dot += 1;
    }
    return {after_dot, before_dot};
}


std::pair<std::string, int> decrease(std::string& after_dot, int& before_dot) {
    if (after_dot != "999") {
        after_dot = std::to_string(std::stoi(after_dot) + 1);
        after_dot = std::string(3 - after_dot.length(), '0') + after_dot;
    } else {
        after_dot = "000";
        before_dot -= 1;
    }
    return {after_dot, before_dot};
}
