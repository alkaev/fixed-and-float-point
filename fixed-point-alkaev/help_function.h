#pragma once

#include <unordered_map>
#include <vector>
#include <string>

std::vector<int> hex_to_binary_code(const std::string& code16);
std::vector<int> int_to_code(long long x, int bits);
bool is_valid(const std::string& s);
bool has_not_0(const std::string& s);
long long floor_div(long long a, long long b);
std::pair<std::string, int> increase(std::string& after_dot, int& before_dot);
std::pair<std::string, int> decrease(std::string& after_dot, int& before_dot);