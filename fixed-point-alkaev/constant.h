#pragma once

inline constexpr int Rounding_TOWARD_ZERO = 0;
inline constexpr int Rounding_NEAREST_EVEN = 1;
inline constexpr int Rounding_TOWARD_INFINITY = 2;
inline constexpr int Rounding_TOWARD_NEG_INFINITY = 3;

inline std::unordered_map<char, std::vector<int>> chars16_to_2 = {
    {'0', {0, 0, 0, 0}}, {'1', {0, 0, 0, 1}}, {'2', {0, 0, 1, 0}}, {'3', {0, 0, 1, 1}},
    {'4', {0, 1, 0, 0}}, {'5', {0, 1, 0, 1}}, {'6', {0, 1, 1, 0}}, {'7', {0, 1, 1, 1}},
    {'8', {1, 0, 0, 0}}, {'9', {1, 0, 0, 1}}, {'A', {1, 0, 1, 0}}, {'B', {1, 0, 1, 1}},
    {'C', {1, 1, 0, 0}}, {'D', {1, 1, 0, 1}}, {'E', {1, 1, 1, 0}}, {'F', {1, 1, 1, 1}}
};