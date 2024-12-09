#pragma once
#include <string>
#include <vector>


std::vector<int> hex_to_binary_code(const std::string& code16);
bool is_valid(const std::string& s);
int need_bits(int x);
std::vector<int> integer_to_bin(int x, int bits);
bool none_of_exp(std::vector<int>::iterator begin, std::vector<int>::iterator end);
int bin_to_integer(const std::vector<int>& code);