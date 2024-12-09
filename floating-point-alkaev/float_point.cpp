#include <vector>
#include <cassert>
#include <cmath>
#include <iostream>
#include <string>
#include "float_point.h"
#include "help_function.h"
#include "constant.h"


FloatPoint::FloatPoint(){}


FloatPoint::FloatPoint(std::vector<int> code, int rounding, bool h_or_f) : rounding(rounding), h_or_f(h_or_f) {
        if (!h_or_f) {
        exponent_len = 5;
        mantissa_len = 10;
        max_exponent = 16;
        min_exponent = -14;
        len_code = 16;
        shift_exponent = 15;
        bytes_out = 3;
    } else {
        exponent_len = 8;
        mantissa_len = 23;
        max_exponent = 128;
        min_exponent = -126;
        len_code = 32;
        shift_exponent = 127;
        bytes_out = 6;
    }

    if (int(code.size()) > len_code) {
    code = std::vector<int>(code.end() - len_code, code.end());
}

if (int(code.size()) < len_code) {
    std::vector<int> code_copy = code;
    code.assign(len_code - code_copy.size(), 0); // Добавляем (len_code - code_copy.size()) нулей в начало
    code.insert(code.end(), code_copy.begin(), code_copy.end()); // Добавляем оставшиеся элементы
}

    sign = code[0];
    exp_code = std::vector<int>(code.begin() + 1, code.begin() + exponent_len + 1 );

    int s = 0;
    for (int i = 1; i <= exponent_len; ++i){
        if (code[i] == 1) s++;
    }
    if (s == 0) {
        exp = min_exponent;
        denormalized = true;
    } 
    else {
        denormalized = false;
        exp = 0;
        for (int i = exponent_len; i > 0; --i) {
            exp += (code[i] << (exponent_len - i));
        }
        exp -= shift_exponent;
    }

    mant = std::vector<int>(code.begin() + 1 + exponent_len, code.end());

    type = Types_USUAL;

    if (exp == max_exponent) {
        s = 0;
        for (int i = 0; i < int(mant.size()); ++i) {
            if (mant[i] == 1) s++;
        }
        if (s == 0){
            type = (sign == 0) ? Types_PLUS_INF : Types_MINUS_INF;
        } else {
            type = Types_NaN;
        }
    }

    integer = 0;
    for (int i = 0; i < mantissa_len; ++i) {
        integer += mant[i] << (mantissa_len - 1 - i);
    }
    if (!denormalized) {
        integer += (1 << mantissa_len);
    }
}

std::ostream& operator<<(std::ostream& os, const FloatPoint& obj) {
    // Обработка типов NaN, infinity, и нуля
    if (obj.type == Types_NaN) {
        os << "nan";
        return os;
    }
    if (obj.type == Types_PLUS_INF) {
        os << "inf";
        return os;
    }
    if (obj.type == Types_MINUS_INF) {
        os << "-inf";
        return os;
    }
    if (obj.is_null()) {
        os << (obj.sign == 1 ? "-" : "");
        os << (obj.h_or_f ? "0x0.000000p+0" : "0x0.000p+0");
        return os;
    }

    // Преобразование мантиссы в hex
    std::string mant2hex;
    std::vector<int> code = obj.mant;
    int exp = obj.exp;

    if (obj.denormalized) {
        exp = obj.min_exponent - 1;
        size_t idx = 0;
        while (idx < code.size() && code[idx] == 0) {
            idx++;
        }
        exp -= static_cast<int>(idx);
        code.erase(code.begin(), code.begin() + idx + 1);
    }
    code.resize(4 * obj.bytes_out, 0); // Добавляем нули для кратности 4

    for (int i = 0; i < obj.bytes_out; ++i) {
        int value = (8 * code[4 * i] + 4 * code[4 * i + 1] + 2 * code[4 * i + 2] + code[4 * i + 3]);
        mant2hex += hex_string[value];
    }

    os << (obj.sign == 1 ? "-" : "");
    os << "0x1." << mant2hex;
    os << "p" << (exp < 0 ? "-" : "+") << std::abs(exp);

    return os;
}

    FloatPoint FloatPoint::to_plus_inf() const {
        std::vector<int> code_plus_inf(len_code, 0);
        for (int i = 1; i <= exponent_len; ++i) {
            code_plus_inf[i] = 1;
        }
        return FloatPoint(code_plus_inf, Rounding_TOWARD_ZERO, h_or_f);
    }

    FloatPoint FloatPoint::to_minus_inf() const {
        std::vector<int> code_minus_inf(len_code, 0);
        for (int i = 0; i <= exponent_len; ++i) {
            code_minus_inf[i] = 1;
        }
        return FloatPoint(code_minus_inf, Rounding_TOWARD_ZERO, h_or_f);
    }

    FloatPoint FloatPoint::to_inf_with_sign(int sign) const {
        return (sign == 0) ? to_plus_inf() : to_minus_inf();
    }

    FloatPoint FloatPoint::to_nan() const {
        return FloatPoint(std::vector<int>(len_code, 1), Rounding_TOWARD_ZERO, h_or_f);
    }

    FloatPoint FloatPoint::to_null() const {
        return FloatPoint(std::vector<int>(len_code, 0), Rounding_TOWARD_ZERO, h_or_f);
    }

    FloatPoint FloatPoint::to_null_with_sign(int sign) const {
        std::vector<int> code(len_code, 0);
        code[0] = sign;
        return FloatPoint(code, Rounding_TOWARD_ZERO, h_or_f);
    }

    bool FloatPoint::is_null() const {
        int s = 0;
        for (int i = 0; i < int(mant.size()); ++i){
            if (mant[i] == 1) s++;
        }
        return (s == 0) && denormalized;
    }

    FloatPoint FloatPoint::to_opposite() const {
        std::vector<int> new_code;
        new_code.push_back(1 - sign); // Инвертируем знак
        auto exp_bin = integer_to_bin(exp + shift_exponent - denormalized, exponent_len);
        new_code.insert(new_code.end(), exp_bin.begin(), exp_bin.end());
        new_code.insert(new_code.end(), mant.begin(), mant.end());
        return FloatPoint(new_code, rounding, h_or_f);
    }

FloatPoint FloatPoint::increase() const {
    if (is_null()) {
        std::vector<int> ans(len_code, 0);
        ans[len_code - 1] = 1; // Последний бит устанавливается в 1
        return FloatPoint(ans, rounding, h_or_f);
    }

    if (sign == 1) {
        // Если число отрицательное, то увеличиваем противоположное число
        return to_opposite().decrease().to_opposite();
    }

    if (denormalized) {
        if (integer + 1 != (1 << mantissa_len)) { // Все еще будет денормализованным
            std::vector<int> new_code(1 + exponent_len, 0); // Знак и экспонента остаются 0
            auto mant_bin = integer_to_bin(integer + 1, mantissa_len);
            new_code.insert(new_code.end(), mant_bin.begin(), mant_bin.end());
            return FloatPoint(new_code, rounding, h_or_f);
        } else { 
            std::vector<int> new_code(len_code, 0);
            new_code[exponent_len] = 1; // Устанавливаем минимальную экспоненту
            return FloatPoint(new_code, rounding, h_or_f);
        }
    }

    int s = 0;
    for (int i = 0; i < int(mant.size()); ++i){
        if (mant[i] == 0) {
            s++;
            break;
        }
    }

    if (s == 0) {
        if (exp + 1 == max_exponent) {
            return to_plus_inf();
        } else {
            // Увеличиваем экспоненту на 1, мантисса становится нулевой
            std::vector<int> new_code;
            new_code.push_back(0); 
            auto exp_bin = integer_to_bin(exp + shift_exponent + 1, exponent_len);
            new_code.insert(new_code.end(), exp_bin.begin(), exp_bin.end());
            new_code.insert(new_code.end(), mantissa_len, 0); 
            return FloatPoint(new_code, rounding, h_or_f);
        }
    } else {
        // Увеличиваем мантиссу на 1, экспонента не меняется
        std::vector<int> new_code;
        new_code.push_back(0);
        auto exp_bin = integer_to_bin(exp + shift_exponent, exponent_len);
        new_code.insert(new_code.end(), exp_bin.begin(), exp_bin.end());
        auto mant_bin = integer_to_bin(integer + 1, mantissa_len);
        new_code.insert(new_code.end(), mant_bin.begin(), mant_bin.end());
        return FloatPoint(new_code, rounding, h_or_f);
    }
}

FloatPoint FloatPoint::decrease() const {
    if (is_null()) {
        std::vector<int> ans(len_code, 0);
        ans[0] = 1;         
        ans[len_code - 1] = 1;    
        return FloatPoint(ans, rounding, h_or_f);
    }

    if (sign == 1) {
        // Если число отрицательное, уменьшаем противоположное
        return to_opposite().increase().to_opposite();
    }

    if (denormalized) {
        // Уменьшаем денормализованное число
        std::vector<int> new_code(1 + exponent_len, 0); // Знак и экспонента остаются 0
        auto mant_bin = integer_to_bin(integer - 1, mantissa_len);
        new_code.insert(new_code.end(), mant_bin.begin(), mant_bin.end());
        return FloatPoint(new_code, rounding, h_or_f);
    }

    // Обработка нормализованных чисел
    std::vector<int> new_code;
    new_code.push_back(0); 
    int s = 0;
    for (int i = 0; i < int(mant.size()); ++i){
        if (mant[i] == 1) s++;
        break;
    }

    if (s != 0) {
        // Мантисса ненулевая
        auto exp_bin = integer_to_bin(exp + shift_exponent, exponent_len);
        new_code.insert(new_code.end(), exp_bin.begin(), exp_bin.end());
        auto mant_bin = integer_to_bin(integer - 1, mantissa_len);
        new_code.insert(new_code.end(), mant_bin.begin(), mant_bin.end());
    } else {
        // Мантисса равна нулю
        if (exp == min_exponent) {
            auto exp_bin = integer_to_bin(0, exponent_len);
            new_code.insert(new_code.end(), exp_bin.begin(), exp_bin.end());
        } else {
            // Уменьшаем экспоненту на 1
            auto exp_bin = integer_to_bin(exp + shift_exponent - 1, exponent_len);
            new_code.insert(new_code.end(), exp_bin.begin(), exp_bin.end());
        }
        new_code.insert(new_code.end(), mantissa_len, 1); // Мантисса из единиц
    }

    return FloatPoint(new_code, rounding, h_or_f);
}

FloatPoint FloatPoint::rounding_result(int sign, int exponent, const std::vector<int>& code) const {

    // Формируем код для округленного числа
    std::vector<int> ans_code;
    ans_code.push_back(sign); 
    auto exp_bin = integer_to_bin(exponent + shift_exponent, exponent_len);
    ans_code.insert(ans_code.end(), exp_bin.begin(), exp_bin.end()); 
    ans_code.insert(ans_code.end(), code.begin(), code.begin() + mantissa_len); 

    FloatPoint ans(ans_code, rounding, h_or_f);

    if (rounding == Rounding_TOWARD_ZERO) {
        return ans;
    }

    int s = 0;
        for (int i = mantissa_len; i < int(code.size()); ++i){
            if (code[i] != 0) s++;
            break;
        }

    if (rounding == Rounding_TOWARD_NEG_INFINITY) {
        
        if (sign == 1 && (s != 0)) {
            return ans.decrease();
        }
        return ans;
    }

    if (rounding == Rounding_TOWARD_INFINITY) {
        if ((s != 0) && sign == 0) {
            return ans.increase();
        }
        return ans;
    }

    if (int(code.size()) == mantissa_len || code[mantissa_len] == 0) {
        return ans; 
    } else {
        int s1 = 0;
        for (int i = mantissa_len + 1; i < int(code.size()); ++i){
            if (code[i] == 1) s1++;
            break;
        }

        if ((s1 == 0) && ans_code.back() == 0) {
            return ans; 
        } else {
            if (ans_code[0] == 1) {
                return ans.decrease();
            } else {
                return ans.increase();
            }
        }
    }
}

FloatPoint FloatPoint::to_ans(int sign, int exp, const std::vector<int>& code) const {
    if (int(code.size()) >= mantissa_len + 1) {
        return rounding_result(sign, exp, code);
    } else {
        std::vector<int> new_code;
        new_code.push_back(sign);
        auto exp_bin = integer_to_bin(exp + shift_exponent, exponent_len);
        new_code.insert(new_code.end(), exp_bin.begin(), exp_bin.end()); 
        new_code.insert(new_code.end(), code.begin(), code.begin() + mantissa_len); 
        return FloatPoint(new_code, rounding, h_or_f);
    }
}

FloatPoint FloatPoint::res_too_small(int other_sign) const {
    if (rounding == Rounding_NEAREST_EVEN) {
        return *this;
    } else if (rounding == Rounding_TOWARD_ZERO) {
        if (sign == other_sign) {
            return *this;
        } else {
            if (sign == 0) {
                return decrease(); 
            } else {
                return increase(); 
            }
        }
    } else if (rounding == Rounding_TOWARD_INFINITY) {
        if (sign == 0) {
            if (other_sign == 0) {
                return increase(); 
            } else {
                return *this; 
            }
        } else {
            if (other_sign == 0) {
                return increase();
            } else {
                return *this; 
            }
        }
    } else { 
        if (sign == 0) {
            
            if (other_sign == 0) {
                return *this; 
            } else {
                return decrease(); 
            }
        } else {
            if (other_sign == 0) {
                return decrease(); 
            } else {
                return decrease(); 
            }
        }
    }
}

FloatPoint FloatPoint::biggest_positive() const {
    if (rounding == Rounding_TOWARD_INFINITY) {
        // Если округление к бесконечности, возвращаем +inf
        return to_plus_inf();
    }
    // Создаем код для наибольшего положительного числа
    std::vector<int> code(len_code, 1);
    code[0] = 0;                 
    code[exponent_len] = 0;           
    return FloatPoint(code, rounding, len_code == 32);
}

FloatPoint FloatPoint::biggest_negative() const {
    if (rounding == Rounding_TOWARD_NEG_INFINITY) {
        // Если округление к минус-бесконечности, возвращаем -inf
        return to_minus_inf();
    }
    // Создаем код для наибольшего отрицательного числа
    std::vector<int> code(len_code, 1); 
    code[exponent_len] = 0;             
    return FloatPoint(code, rounding, len_code == 32);
}

FloatPoint FloatPoint::biggest_with_sign(int sign) const {
    if (sign == 0) {
        return biggest_positive();
    }
    return biggest_negative();
}

FloatPoint operator+(const FloatPoint& num1, const FloatPoint& num2) {
    if (num1.type == Types_NaN ) { 
        return num1;
    }
    if (num2.type == Types_NaN) {
        return num2;
    }
    
    if ((num1.type == Types_MINUS_INF && num2.type == Types_PLUS_INF) || 
        (num2.type == Types_MINUS_INF && num1.type == Types_PLUS_INF)) { 
        return num1.to_nan();
    }
    if (num1.type == Types_MINUS_INF || num2.type == Types_MINUS_INF) { 
        return num1.to_minus_inf();
    }
    if (num1.type == Types_PLUS_INF || num2.type == Types_PLUS_INF) {
        return num1.to_plus_inf();
    }
    if (num1.is_null() && num2.is_null()) { 
        if (num1.sign == 1 && num2.sign == 1) {
            return num1.to_null_with_sign(1);
        } else if ((num1.sign == 1 || num2.sign == 1) && num1.rounding == Rounding_TOWARD_NEG_INFINITY) {
            return num1.to_null_with_sign(1);
        }
        return num1.to_null();
    }
    if (num1.sign != num2.sign) {
        if (num1.integer == num2.integer && num1.exp == num2.exp) {
            return num1.to_null_with_sign(num1.rounding == Rounding_TOWARD_NEG_INFINITY ? 1 : 0);
        }
    }

    if (num1.denormalized && num2.denormalized) {
        int num1_int = num1.integer * (num1.sign == 1 ? -1 : 1);
        int num2_int = num2.integer * (num2.sign == 1 ? -1 : 1);
        int new_num = num1_int + num2_int;
        std::vector<int> new_code(num1.len_code, 0);
        new_code[0] = (new_num >= 0) ? 0 : 1;
        new_num = std::abs(new_num);
        if (new_num < (1 << num1.mantissa_len)) { 
            for (int i = 0; i < num1.mantissa_len; ++i) {
                new_code[num1.len_code - 1 - i] = (new_num & (1 << i)) ? 1 : 0;
            }
        } else { 
            for (int i = 0; i <= num1.mantissa_len; ++i) {
                new_code[num1.len_code - 1 - i] = (new_num & (1 << i)) ? 1 : 0;
            }
        }
        return FloatPoint(new_code, num1.rounding, num1.h_or_f);
    }

    if (!num1.denormalized && num2.denormalized) {
        if (num2.is_null()) {
            return num1;
        }
        int num1_exp = num1.exp;
        int num2_exp = num1.min_exponent;
        int num1_int = num1.integer;
        int num2_int = num2.integer;
        int div_exp = num1_exp - num2_exp;
        if (div_exp >= num1.mantissa_len + 2) {
            return num1.res_too_small(num2.sign);
        } else {
            if (num1.sign != num2.sign) {
                num2_int *= -1;
            }
            num1_int <<= (num1_exp - num1.min_exponent);
            int need = need_bits(num1_int);
            num1_int += num2_int;

            if (num1_int >= (1 << need)) { 
                num1_exp++;
                if (num1_exp == num1.max_exponent) {
                    return num1.biggest_with_sign(num1.sign);
                }
                auto code_of_num1_int = integer_to_bin(num1_int, need);
                return num1.to_ans(num1.sign, num1_exp, code_of_num1_int);
            } else if ((1 << need) > num1_int && num1_int >= (1 << (need - 1))) { 
                auto code_of_num1_int = integer_to_bin(num1_int, need - 1);
                return num1.to_ans(num1.sign, num1_exp, code_of_num1_int);
            } else { 
                if (num1_exp > num1.min_exponent) {
                    num1_exp--;
                    auto code_of_num1_int = integer_to_bin(num1_int, need - 2);
                    return num1.to_ans(num1.sign, num1_exp, code_of_num1_int);
                } else {
                    std::vector<int> new_code(num1.len_code, 0);
                    new_code[0] = num1.sign;
                    auto zeros = integer_to_bin(0, num1.exponent_len);
                    new_code.insert(new_code.end(), zeros.begin(), zeros.end());
                    auto int_code = integer_to_bin(num1_int, need - 1);
                    new_code.insert(new_code.end(), int_code.begin(), int_code.end());
                    return FloatPoint(new_code, num1.rounding, num1.h_or_f);
                }
            }
        }
    }

    if (num1.denormalized && !num2.denormalized) {
        return num2 + num1;
    }

    if (num1.exp < num2.exp || (num1.exp == num2.exp && num1.integer < num2.integer)) {
        return num2 + num1;
    }

    int num1_int = num1.integer;
    int num2_int = num2.integer;
    int num1_exp = num1.exp;
    int num2_exp = num2.exp;
    if (num1_exp - num2_exp > num1.mantissa_len + 2) { 
        return num1.res_too_small(num2.sign);
    }

    num1_int <<= (num1_exp - num2_exp);
    int need = need_bits(num1_int);
    if (num1.sign != num2.sign) {
        num2_int *= -1;
    }
    num1_int += num2_int;
    if (num1_int >= (1 << need)) {
        need++;
        num1_exp++;
    }
    if (num1_int == 0) {
        return FloatPoint(std::vector<int>(num1.len_code, 0), num1.rounding, num1.h_or_f);
    }
    if (num1_exp == num1.max_exponent) { 
        return num1.biggest_with_sign(num1.sign);
    }

    int idx = 0;
    auto code_of_num1_int = integer_to_bin(num1_int, need);
    while (code_of_num1_int[idx] == 0) {
        idx++;
        num1_exp--;
    }
    if (num1_exp >= num1.min_exponent) { 
        idx++;
        code_of_num1_int.insert(code_of_num1_int.end(), num1.mantissa_len, 0);
        return num1.to_ans(num1.sign, num1_exp, {code_of_num1_int.begin() + idx, code_of_num1_int.end()});
    } else { 
        int x = -(num1_exp - num1.min_exponent);
        assert(x >= 1);
        std::vector<int> new_code;
        new_code.push_back(num1.sign);
        auto zeros = integer_to_bin(0, num1.exponent_len);
        new_code.insert(new_code.end(), zeros.begin(), zeros.end());
        code_of_num1_int.insert(code_of_num1_int.end(), num1.mantissa_len, 0);
        new_code.insert(new_code.end(), std::vector<int>(std::min(x - 1, num1.mantissa_len), 0).begin(), std::vector<int>(std::min(x - 1, num1.mantissa_len), 0).end());
        new_code.insert(new_code.end(), code_of_num1_int.begin() + idx, code_of_num1_int.begin() + idx + num1.mantissa_len - std::min(x - 1, num1.mantissa_len));
        return FloatPoint(new_code, num1.rounding, num1.h_or_f);
    }
}

FloatPoint operator-(const FloatPoint& num1, const FloatPoint& num2) {
     if (num1.type == Types_NaN ) { 
        return num1;
    }
    if (num2.type == Types_NaN) {
        return num2;
    }
    return num1 + num2.to_opposite();  // Сложение с противоположным числом
}

FloatPoint FloatPoint::round_too_small(const FloatPoint& other) const {
    // Если результат слишком мал по модулю
    if (rounding == Rounding_TOWARD_ZERO || 
        rounding == Rounding_NEAREST_EVEN || 
        (rounding == Rounding_TOWARD_INFINITY && sign != other.sign) || 
        (rounding == Rounding_TOWARD_NEG_INFINITY && sign == other.sign)) {
        return to_null_with_sign((sign == other.sign) ? 0 : 1);
    } else {
        std::vector<int> ans(len_code, 0);
        ans[0] = (sign == other.sign) ? 0 : 1; 
        ans[len_code - 1] = 1; // Устанавливаем младший бит
        return FloatPoint(ans, rounding, h_or_f);
    }
}


FloatPoint operator*(const FloatPoint& num1, const FloatPoint& num2) {
    // Специальные случаи
    if (num1.type == Types_NaN) { 
        return num1;
    } 

    if (num1.type == Types_NaN){
        return num2;
    }

    if ((num1.is_null() && (num2.type == Types_MINUS_INF || num2.type == Types_PLUS_INF)) || 
               (num2.is_null() && (num1.type == Types_MINUS_INF || num1.type == Types_PLUS_INF))) {
        return num1.to_nan();
    }

    if (num1.is_null() || num2.is_null()) {
        return num1.to_null_with_sign((num1.sign == num2.sign) ? 0 : 1);
    }
    if (num1.type == Types_MINUS_INF || num1.type == Types_PLUS_INF || 
        num2.type == Types_MINUS_INF || num2.type == Types_PLUS_INF) {
        return num1.to_inf_with_sign((num1.sign == num2.sign) ? 0 : 1);
    }

    // Оба числа денормализованные
    if (num1.denormalized && num2.denormalized) { 
        return num1.round_too_small(num2);
    }

    // Если одно число денормализованное
    if (num2.denormalized) {
        int num1_exp = num1.exp;
        int num1_int = num1.integer;
        int num2_exp = num2.min_exponent;

        int x = 1;
        while (num2.mant[x - 1] == 0) {
            ++x;
        }
        num2_exp -= x;
        int num2_int = num2.integer;
        num1_int *= num2_int;
        num1_exp += num2_exp;

        int need = need_bits(num1_int);

        if (num1_exp >= num1.min_exponent) { 
            auto code_of_num1_int = integer_to_bin(num1_int, need - 1);
            return num1.rounding_result((num1.sign == num2.sign) ? 0 : 1, num1_exp, code_of_num1_int);
        } else if (num1_exp < num1.min_exponent - num1.mantissa_len) { 
            return num1.round_too_small(num2);
        } else { 
            int prev_nulls_in_mant = -(num1_exp - num1.min_exponent) - 1;
            std::vector<int> code_of_num1_int(prev_nulls_in_mant, 0);
            auto bin_representation = integer_to_bin(num1_int, need);
            code_of_num1_int.insert(code_of_num1_int.end(), bin_representation.begin(), bin_representation.end());
            return num1.to_ans((num1.sign == num2.sign) ? 0 : 1, num1.min_exponent - 1, code_of_num1_int);
        }
    } else if (num1.denormalized) {
        return num2 * num1;
    } else { // Оба числа нормализованные
        int num1_exp = num1.exp;
        int num1_int = num1.integer;
        int num2_exp = num2.exp;
        int num2_int = num2.integer;
        num1_int *= num2_int;
        int need = need_bits(num1_int);
        int div_exp = need - 2 * num1.mantissa_len - 1;
        auto code_of_num1_int = integer_to_bin(num1_int, need - 1);
        num1_exp += num2_exp + div_exp;

        if (num1_exp >= num1.max_exponent) { 
            return num1.biggest_with_sign((num1.sign == num2.sign) ? 0 : 1);
        } else if (num1_exp >= num1.min_exponent) { 
            return num1.to_ans((num1.sign == num2.sign) ? 0 : 1, num1_exp, code_of_num1_int);
        } else if (num1_exp < num1.min_exponent - num1.mantissa_len) {
            return num1.round_too_small(num2);
        } else { 
            int prev_nulls_in_mant = -(num1_exp - num1.min_exponent) - 1;
            std::vector<int> code_of_num1_int(prev_nulls_in_mant, 0);
            auto bin_representation = integer_to_bin(num1_int, need_bits(num1_int));
            code_of_num1_int.insert(code_of_num1_int.end(), bin_representation.begin(), bin_representation.end());
            return num1.to_ans((num1.sign == num2.sign) ? 0 : 1, num1.min_exponent - 1, code_of_num1_int);
        }
    }
}

FloatPoint operator/(const FloatPoint& num1, const FloatPoint& num2) {
    // Специальные случаи
    if (num1.type == Types_NaN) {
        return num1;
    }

    if (num2.type == Types_NaN) {
        return num2;
    }

    if ((num1.type == Types_PLUS_INF || num1.type == Types_MINUS_INF) &&
               (num2.type == Types_PLUS_INF || num2.type == Types_MINUS_INF)) {
        return num1.to_nan();
    } else if (num1.is_null() && num2.is_null()) {
        return num1.to_nan();
    } else if (num1.is_null() || num2.type == Types_PLUS_INF || num2.type == Types_MINUS_INF) {
        return num1.to_null_with_sign((num1.sign == num2.sign) ? 0 : 1);
    } else if (num1.type == Types_PLUS_INF || num1.type == Types_MINUS_INF || num2.is_null()) {
        return num1.to_inf_with_sign((num1.sign == num2.sign) ? 0 : 1);
    }

    int ans_sign = (num1.sign == num2.sign) ? 0 : 1;

    // Разбираем делимое
    int num1_exp;
    int num1_int;
    if (!num1.denormalized) {
        num1_exp = num1.exp;
        num1_int = num1.integer;
    } else {
        num1_exp = num1.min_exponent - 1;
        int idx = 0;
        while (num1.mant[idx] == 0) {
            ++idx;
            --num1_exp;
        }
        std::vector<int> code_for_num1_int(num1.mant.begin() + idx, num1.mant.end());
        code_for_num1_int.resize(1 + num1.mantissa_len, 0);
        num1_int = bin_to_integer(code_for_num1_int);
    }

    // Разбираем делитель
    int num2_exp;
    int num2_int;
    if (!num2.denormalized) {
        num2_exp = num2.exp;
        num2_int = num2.integer;
    } else {
        num2_exp = num1.min_exponent - 1;
        int idx = 0;
        while (num2.mant[idx] == 0) {
            ++idx;
            --num2_exp;
        }
        std::vector<int> code_for_num2_int(num2.mant.begin() + idx, num2.mant.end());
        code_for_num2_int.resize(1 + num1.mantissa_len, 0);
        num2_int = bin_to_integer(code_for_num2_int);
    }

    const int need = 1 + num1.mantissa_len;

    if (num1_int >= num2_int) {
        num1_int <<= num1.mantissa_len;
        num1_exp -= num2_exp;
    } else {
        num1_int <<= (num1.mantissa_len + 1);
        num1_exp -= (num2_exp + 1);
    }

    int div = num1_int / num2_int;
    int r = num1_int % num2_int;

    // Округление
    if (num1.rounding == Rounding_TOWARD_INFINITY && num1.sign == 0 && r > 0) {
        div += 1;
    } else if (num1.rounding == Rounding_TOWARD_NEG_INFINITY && num1.sign == 1 && r > 0) {
        div += 1;
    } else if (num1.rounding == Rounding_NEAREST_EVEN && 
               (2 * r > num2_int || (2 * r == num2_int && div % 2 == 1))) {
        div += 1;
    }

    if (div >= (1 << need)) {
        ++num1_exp;
    }

    if (num1_exp >= num1.max_exponent) {
        return num1.biggest_with_sign(ans_sign);
    } else if (num1_exp < num1.min_exponent - num1.mantissa_len) {
        return num1.round_too_small(num2);
    } else if (num1_exp < num1.min_exponent) {
        std::vector<int> code_of_res(-(num1_exp - num1.min_exponent) - 1, 0);
        auto bin_representation = integer_to_bin(div, need - 1);
        code_of_res.insert(code_of_res.end(), bin_representation.begin(), bin_representation.end());
        return num1.to_ans(ans_sign, num1.min_exponent - 1, code_of_res);
    } else {
        return num1.to_ans(ans_sign, num1_exp, integer_to_bin(div, need - 1));
    }
}

std::string float_to_hex(const FloatPoint& num){
    std::vector<int> code;
    code.insert(code.begin(), num.sign);
    code.insert(code.end(), num.exp_code.begin(), num.exp_code.end());
    code.insert(code.end(), num.mant.begin(), num.mant.end());
    std::string ans = "";
    for(int i = 0; i < int(code.size())/4; ++i){
        int idx = 0;
        for (int j = 0; j < 4; ++j){
            idx += pow(2, 3 - j) * code[4 * i + j];
        }
    ans += index_to_hex[idx];
    }
    return ans;
}
