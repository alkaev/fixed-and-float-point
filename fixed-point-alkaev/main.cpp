#include <iostream>
#include <string>
#include <cctype>
#include <sstream>
#include <cmath>
#include <stdexcept>
#include <vector>
#include "help_function.h"
#include "fixed_point.h"


void solve(const std::vector<std::string>& inputs) {
    if (inputs.size() != 3 && inputs.size() != 5) {
        std::cerr << "3 or 5 arguments expected, but " << inputs.size() << " given" << std::endl;
        return;
    }

    if (inputs.size() == 3) {
        std::string hex_value = inputs[2];
        if (!is_valid(hex_value)) {
            std::cerr << hex_value << " is not valid hex input" << std::endl;
            return;
        }

        hex_value = inputs[2];
        for (auto& c : hex_value) {
            c = toupper(c);
        }
   
        std::size_t dot_pos = inputs[0].find('.');
        if (dot_pos == std::string::npos) return; 
        std::string A_str = inputs[0].substr(0, dot_pos);
        std::string B_str = inputs[0].substr(dot_pos + 1);
        int A = stoi(A_str);
        int B = stoi(B_str);
        int rounding = stoi(inputs[1]);
        std::cout << FixedPoint(A, B, hex_to_binary_code(hex_value), rounding) << std::endl;
        
    } else if (inputs.size() == 5) {
        if (inputs[3] != "+" && inputs[3] != "-" && inputs[3] != "*" && inputs[3] != "/") {
        std::cerr << "Incorrect input: Operation should be +, -, *, or /, but got " << inputs[3] << std::endl;
        return;
    }

    if (!is_valid(inputs[2])) {
        std::cerr << inputs[2] << " is not valid hex input 2" << std::endl;
        return;
    }
    if (!is_valid(inputs[4])) {
        std::cerr << inputs[4] << " is not valid hex input" << std::endl;
        return;
    }

    std::string input2 = inputs[2];
    std::string input4 = inputs[4];
    for (auto& c : input2) c = toupper(c);
    for (auto& c : input4) c = toupper(c);

    size_t dot_pos = inputs[0].find('.');
    if (dot_pos == std::string::npos) {
        std::cerr << "Invalid format for fixed-point" << std::endl;
        return;
    }

    int A = stoi(inputs[0].substr(0, dot_pos));
    int B = stoi(inputs[0].substr(dot_pos + 1));

    int rounding = stoi(inputs[1]);
    FixedPoint num1(A, B, hex_to_binary_code(input2), rounding);
    FixedPoint num2(A, B, hex_to_binary_code(input4), rounding);

   
    if (inputs[3] == "+") {
        std::cout << num1 + num2;
    } else if (inputs[3] == "-") {
        std::cout << num1 - num2;
    } else if (inputs[3] == "*") {
       std::cout << num1 * num2;
    } else if (inputs[3] == "/") {
      std::cout << num1 / num2;
    }
    
}
}

std::vector<std::string> splitString(const std::string& input) {
    std::vector<std::string> words;       
    std::istringstream stream(input);     
    std::string word;

    while (stream >> word) {              
        words.push_back(word);           
    }

    return words;                         
}


int main(int argc, char* argv[]) {
    

    std::vector<std::string> inputs(argv + 1, argv + argc); 
    solve(inputs);

    return 0;
}