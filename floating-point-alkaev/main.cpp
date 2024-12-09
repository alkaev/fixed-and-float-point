#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cassert>
#include "float_point.h" 
#include "help_function.h"
#include "constant.h"

void solve(const std::vector<std::string>& inputs) {
    if (inputs.size() != 3 && inputs.size() != 5) {
        std::cerr << "3 or 5 arguments expected, but " << inputs.size() << " given" << std::endl;
        return;
    }

    if (inputs.size() == 3) {
        std::string hex_value = inputs[2];
        for (auto& c : hex_value) {
            c = toupper(c);
        }
        if (!is_valid(hex_value)) {
            std::cerr << hex_value << " is not valid hex input" << std::endl;
            return;
        }

        if (inputs[0] != "h" && inputs[0] != "f"){
            std::cerr << hex_value << " is not valid input" << std::endl;
            return;
        }


        if (inputs[0] == "h" || inputs[0] == "f") {
            auto code = hex_to_binary_code(hex_value); 

            /*
            for (auto el : code)
                std::cout << el;
            std::cout << " ";
            */

            int rounding = std::stoi(inputs[1]);
            
            std::cout << FloatPoint(code, rounding, inputs[0] == "f") << " ";
            std::string ans = float_to_hex(FloatPoint(code, rounding, inputs[0] == "f"));       
            std::cout << "0x" << ans;
        } 
        
    } else if (inputs.size() == 5) {
        if (inputs[3] != "+" && inputs[3] != "-" && inputs[3] != "*" && inputs[3] != "/") {
            std::cerr << "Incorrect input: Operation should be +,-,* or /, but got " << inputs[3] << std::endl;
            return;
        }

        std::string input2 = inputs[2];
        std::string input4 = inputs[4];
        for (auto& c : input2) c = toupper(c);
        for (auto& c : input4) c = toupper(c);

        if (!is_valid(input2)) {
            std::cerr << input2 << " is not valid hex input" << std::endl;
            return;
        }
        if (!is_valid(input4)) {
            std::cerr << input4 << " is not valid hex input" << std::endl;
            return;
        }

        
        int rounding = std::stoi(inputs[1]);
        FloatPoint num1 = FloatPoint(hex_to_binary_code(input2), rounding, inputs[0] == "f");
        FloatPoint num2 = FloatPoint(hex_to_binary_code(input4), rounding, inputs[0] == "f");
        
        FloatPoint result;
        if (inputs[3] == "+") {
           result = num1 + num2;
        } else if (inputs[3] == "-") {
           result = num1 - num2;
        } else if (inputs[3] == "*") {
           result = num1 * num2;
        } else if (inputs[3] == "/") {
           result = num1 / num2;
        }
        std::cout << result << " 0x" << float_to_hex(result);
    }
}


int main(int argc, char* argv[]) {

    std::vector<std::string> inputs(argv + 1, argv + argc); 
    solve(inputs);

    return 0;
}