#pragma once
#include <iostream>
#include <vector>

class FloatPoint {
private:
    int exponent_len;
    int mantissa_len;
    int max_exponent;
    int min_exponent;
    int len_code;
    int shift_exponent;
    int bytes_out;
    int rounding;
    bool h_or_f;
    bool denormalized;
    int exp;
    int integer;
    std::vector<int> mant;
    std::vector<int> exp_code;
    int type;
    int sign;
public:
    FloatPoint();
    FloatPoint(std::vector<int> code, int rounding, bool h_or_f);
    friend std::ostream& operator<<(std::ostream& os, const FloatPoint& num);
    friend FloatPoint operator+(const FloatPoint& num1, const FloatPoint& num2);
    friend FloatPoint operator-(const FloatPoint& num1, const FloatPoint& num2);
    friend FloatPoint operator*(const FloatPoint& num1, const FloatPoint& num2);
    friend FloatPoint operator/(const FloatPoint& num1, const FloatPoint& num2);
    friend std::string float_to_hex(const FloatPoint& num);   
    FloatPoint to_plus_inf() const;
    FloatPoint to_minus_inf() const;
    FloatPoint to_inf_with_sign(int sign) const;
    FloatPoint to_nan() const;
    FloatPoint to_null() const;
    FloatPoint to_null_with_sign(int sign) const; 
    FloatPoint to_opposite() const;
    FloatPoint increase() const;
    FloatPoint decrease() const;
    FloatPoint rounding_result(int sign, int exponent, const std::vector<int>& code) const;
    FloatPoint to_ans(int sign, int exp, const std::vector<int>& code) const;
    FloatPoint res_too_small(int other_sign) const;
    FloatPoint biggest_positive() const;
    FloatPoint biggest_negative() const;
    FloatPoint biggest_with_sign(int sign) const;
    FloatPoint round_too_small(const FloatPoint& other) const;  
    bool is_null() const;
};