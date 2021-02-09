#include "tcr_input.h"

#include <fstream>
#include <iostream>

int main(int argc, char** argv) {
    if(argc == 0)   printf("Please insert your script file in the command line args you mf");

    std::ifstream input(argv[0]);
    auto result = tcr::parse([&]() -> std::optional<std::string> {
        std::string line;
        if(std::getline(input, line))   return line;    else return {};
    });

    for(auto msg : result.messages) {
        std::cout << msg.author.name << " (r=" << msg.author.nameColor[0] << ", g=" << msg.author.nameColor[1] << ", b=" << msg.author.nameColor[2] << ", a=" << msg.author.nameColor[3] << ", icons=";
        
        if(msg.author.icons.empty()) {
            std::cout << "empty";
        } else {
            std::cout << "[" << msg.author.icons[0];
            for(size_t i = 1; i < msg.author.icons.size(); i++)    std::cout << ", " << msg.author.icons[i];
            std::cout << "]";
        }

        std::cout << ") " << msg.content << std::endl;
    }
}