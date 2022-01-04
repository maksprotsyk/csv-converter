#include <iostream>

#include "parsing.h"


int main()
{
    std::string input_name;
    std::string output_name;
    std::string separator;

    std::cout << "Enter the input filename: ";
    std::cin >> input_name;
    std::cout << "Enter the output filename: ";
    std::cin >> output_name;
    while (true) {
        std::cout << "Enter the separator (for example: ';'): ";
        std::cin >> separator;
        if (separator.size() == 1) {
            break;
        }
        std::cout << "Error: separator size should be exactly 1" << std::endl;
    }

    return convert_csv(input_name, output_name, separator);

}
 