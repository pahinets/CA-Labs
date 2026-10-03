#include <iostream>
#include <cstdlib>
#include <string>
#include "calculator.h"

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <a> <+|-|*|/> <b>\n";
        return 1;
    }
    double a = std::atof(argv[1]);
    double b = std::atof(argv[3]);
    std::string op = argv[2];
    try {
        if (op == "+") std::cout << add(a, b) << "\n";
        else if (op == "-") std::cout << subtract(a, b) << "\n";
        else if (op == "*") std::cout << multiply(a, b) << "\n";
        else if (op == "/") std::cout << divide(a, b) << "\n";
        else { std::cerr << "Unknown operator\n"; return 1; }
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
