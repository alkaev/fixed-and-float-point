#pragma once
#include <iostream>
#include <vector>

class FixedPoint {
public:
    int a;
    int b;
    std::vector<int> code;
    int rounding;
    long long integer;

    FixedPoint(int a, int b, std::vector<int> code, int rounding);
    FixedPoint(const FixedPoint& other);
    friend FixedPoint operator+(const FixedPoint& num1, const FixedPoint& num2);
    friend std::ostream& operator<<(std::ostream& os, const FixedPoint& num);
    friend FixedPoint operator-(const FixedPoint& num1, const FixedPoint& num2);
    friend FixedPoint operator*(const FixedPoint& num1, const FixedPoint& num2);
    friend FixedPoint operator/(const FixedPoint& num1, const FixedPoint& num2);
};