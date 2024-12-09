#include "fixed_point.h"
#include "help_function.h"
#include "constant.h"
#include <iostream>
#include <cmath> 
#include <string> 

FixedPoint::FixedPoint(int a, int b, std::vector<int> code, int rounding) : a(a), b(b), rounding(rounding), integer(0) {

        if (int(code.size()) <= a + b) {
            // Заполняем `code` нулями до длины a + b
            this->code = std::vector<int>(a + b - code.size(), 0);
            this->code.insert(this->code.end(), code.begin(), code.end());
        } else {
            // Если code длиннее a + b, берём последние a + b бит
            this->code = std::vector<int>(code.end() - (a + b), code.end());
        }
        /*for (int i = 0 ; i < int(this->code.size()); i++)
            std::cout << this->code[i];
*/
        // Рассчитываем целое значение `integer` из битов в code
        int p = 1;
        for (int i = int(this->code.size())-1; i > 0; --i) {
            integer += p * this->code[i];
            p <<= 1;
            //std::cout << integer << " "; 
        }

        integer -= p * this->code[0];
        
        //std::cout << integer << std::endl;
    }

FixedPoint::FixedPoint(const FixedPoint& other)
    : a(other.a), b(other.b), code(other.code), rounding(other.rounding), integer(other.integer) {}


FixedPoint operator+(const FixedPoint& num1, const FixedPoint& num2) {
    //std::cout << num1.integer << " " << num2.integer << std::endl;
    return FixedPoint(num1.a, num1.b, int_to_code(num1.integer + num2.integer, num1.a + num1.b), num1.rounding);
}

FixedPoint operator-(const FixedPoint& num1, const FixedPoint& num2) {
    //std::cout << num1.integer << " " << num2.integer << std::endl;
    return FixedPoint(num1.a, num1.b, int_to_code(num1.integer - num2.integer, num1.a + num1.b), num1.rounding);
}

FixedPoint operator*(const FixedPoint& num1, const FixedPoint& num2) {
    int64_t new_int = static_cast<int64_t>(num1.integer) * num2.integer;

    if (num1.rounding == Rounding_TOWARD_ZERO) {
        if (new_int >= 0) {
            new_int >>= num1.b;
        } else {
            new_int = -((std::abs(new_int)) >> num1.b);
        }
    } else if (num1.rounding == Rounding_TOWARD_INFINITY) {
       // new_int = -((-new_int) / (1 << num1.b));
         new_int = -floor_div((-new_int), (1 << num1.b));
    } else if (num1.rounding == Rounding_TOWARD_NEG_INFINITY) {
        new_int = floor_div((new_int), (1 << num1.b));
    } else {
        bool inv = false;
        if (new_int < 0) {
            inv = true;
            new_int = -new_int;
        }
        if ((new_int % (1 << num1.b)) * 2 > (1 << num1.b)) {
            new_int >>= num1.b;
            new_int += 1;
        } else if ((new_int % (1 << num1.b)) * 2 == (1 << num1.b)) {
            new_int >>= num1.b;
            new_int += (new_int % 2);
        } else {
            new_int >>= num1.b;
        }
        if (inv) {
            new_int = -new_int;
        }
    }

    new_int %= (1 << (num1.a + num1.b));
    return FixedPoint(num1.a, num1.b, int_to_code(new_int, num1.a + num1.b), num1.rounding);
}

FixedPoint operator/(const FixedPoint& num1, const FixedPoint& num2){

    //std::cout << num2.integer << "  " ;
    if (num2.integer == 0) {
        std::cerr << "div_by_0" << std::endl;
        return FixedPoint(num1.a, num1.b, int_to_code(0, num1.a + num1.b), num1.rounding);
    }
    if (num1.integer == 0) {
        return FixedPoint(num1.a, num1.b, int_to_code(0, num1.a + num1.b), num1.rounding);
    }

    int64_t new_int;
    int64_t abs_num1_int = std::abs(num1.integer);
    int64_t abs_num2_int = std::abs(num2.integer);

    if (num1.rounding == Rounding_TOWARD_ZERO) {
        if (num1.code[0] == num2.code[0]) {
            new_int = floor_div((abs_num1_int << num1.b), abs_num2_int);
        } else {
            new_int = -floor_div((abs_num1_int << num1.b), abs_num2_int);
        }
    } else if (num1.rounding == Rounding_NEAREST_EVEN) {
        new_int = floor_div((abs_num1_int << num1.b), abs_num2_int);
        if (2 * ((abs_num1_int << num1.b) % abs_num2_int) > abs_num2_int) {
            new_int += 1;
        } else if (2 * ((abs_num1_int << num1.b) % abs_num2_int) == abs_num2_int && new_int % 2 == 1) {
            new_int += 1;
        }
        if (num1.code[0] != num2.code[0]) {
            new_int = -new_int;
        }
    } else if (num1.rounding == Rounding_TOWARD_INFINITY) {
        if (num1.code[0] != num2.code[0]) {
            new_int = -floor_div((abs_num1_int << num1.b), abs_num2_int);
        } else {
            new_int = floor_div((abs_num1_int << num1.b), abs_num2_int);
            if (abs_num1_int % abs_num2_int > 0) {
                new_int += 1;
            }
        }
    } else { // Rounding_TOWARD_NEG_INFINITY
        if (num1.code[0] == num2.code[0]) {
            new_int = floor_div((abs_num1_int << num1.b), abs_num2_int);
        } else {
            new_int = -floor_div((abs_num1_int << num1.b), abs_num2_int);
            if (abs_num1_int % abs_num2_int > 0) {
                new_int -= 1;
            }
        }
    }

    return FixedPoint(num1.a, num1.b, int_to_code(new_int, num1.a + num1.b), num1.rounding);
}

std::ostream& operator<<(std::ostream& os, const FixedPoint& num) {
    
        int first_part = 0;  
        for (int i = num.a - 1; i > 0; --i) {
            first_part += (num.code[i]) * (1 << (num.a - 1 - i));
        }
        first_part -= (num.code[0]) * (1 << (num.a - 1));
        
        long long second_part = 0;
        for (int i = num.a; i < num.a + num.b; ++i) {
            second_part += num.code[i]  * (1 << (num.a + num.b - i - 1));
        }
        
        second_part *= pow(5, num.b);

        if (num.code[0] == 1) {
            if (second_part > 0) {
                second_part = pow(10, num.b) - second_part;
                first_part += 1;
            }
        }

        std::string rounded_second_part =  std::to_string(second_part);
        int first_nulls = num.b - rounded_second_part.length() ;
        std::string str_second_part = std::to_string(second_part);

        if (first_nulls >= 3) {
            rounded_second_part = "000";
        } else {
            rounded_second_part = std::string(first_nulls, '0') + rounded_second_part.substr(0, 3 - first_nulls);
        }
        
        

      if (num.rounding == Rounding_TOWARD_ZERO) {
    if (num.code[0] == 1 && first_part == 0 && rounded_second_part != "000") {
        
        
        os << '-' << first_part << '.' << rounded_second_part 
           << std::string(3 - rounded_second_part.length(), '0');
        return os;
    }
    
    os << first_part << '.' << rounded_second_part 
       << std::string(3 - rounded_second_part.length(), '0');
       return os;
}
        if (num.rounding == Rounding_TOWARD_INFINITY) {
    if (num.code[0] == 1) {  
        if (first_part == 0 && rounded_second_part != "000") {
            os << '-' << first_part << '.' << rounded_second_part 
               << std::string(3 - rounded_second_part.length(), '0');
            return os;
        }
        os << first_part << '.' << rounded_second_part 
           << std::string(3 - rounded_second_part.length(), '0');
        return os;
    } else {
       
        if (int(std::to_string(second_part).length()) > 3 - first_nulls && 
            has_not_0(std::to_string(second_part).substr(3 - first_nulls))) {
            std::tie(rounded_second_part, first_part) = increase(rounded_second_part, first_part);
        }
        os << first_part << '.' << rounded_second_part 
           << std::string(3 - rounded_second_part.length(), '0');
        return os;
    }
}
        
        else if (num.rounding == Rounding_TOWARD_NEG_INFINITY) {
        if (num.code[0] == 1) {
            if (int(str_second_part.length()) > 3 - first_nulls && has_not_0(str_second_part.substr(3 - first_nulls))) {
                decrease(rounded_second_part, first_part);
            }
            if (first_part == 0) {  
                os << "-" + std::to_string(first_part) + "." + rounded_second_part + std::string(3 - rounded_second_part.length(), '0');
                return os;
            }
            os << std::to_string(first_part) + "." + rounded_second_part + std::string(3 - rounded_second_part.length(), '0');
            return os;
        } else {
            os << std::to_string(first_part) + "." + rounded_second_part + std::string(3 - rounded_second_part.length(), '0');
            return os;
        }
    } else {  
        if (num.code[0] == 1) {
            if (first_nulls <= 3 && int(str_second_part.length()) > 3 - first_nulls) {
                if (str_second_part[3 - first_nulls] == '5') {
                    if (has_not_0(str_second_part.substr(4 - first_nulls))) {
                        decrease(rounded_second_part, first_part);
                    } else if (std::stoi(rounded_second_part) % 2 == 1) {
                        decrease(rounded_second_part, first_part);
                    }
                } else if (str_second_part[3 - first_nulls] > '5') {
                    decrease(rounded_second_part, first_part);
                }
            }
            if (first_part == 0 && rounded_second_part != "000") {
                os << "-" + std::to_string(first_part) + "." + rounded_second_part + std::string(3 - rounded_second_part.length(), '0');
                return os;
            }
            os << std::to_string(first_part) + "." + rounded_second_part + std::string(3 - rounded_second_part.length(), '0');
            return os;
        } else {
            if (first_nulls <= 3 && int(str_second_part.length()) > 3 - first_nulls) {
                
                if (str_second_part[3 - first_nulls] == '5') {
                    if (has_not_0(str_second_part.substr(4 - first_nulls))) {
                        increase(rounded_second_part, first_part);
                    } else if (std::stoi(rounded_second_part) % 2 == 1) {
                        increase(rounded_second_part, first_part);
                    }
                } else if (str_second_part[3 - first_nulls] > '5') {
                    increase(rounded_second_part, first_part);
                }
            }
            os << std::to_string(first_part) + "." + rounded_second_part + std::string(3 - rounded_second_part.length(), '0');
            return os;
        }
    }
    return os;
}



