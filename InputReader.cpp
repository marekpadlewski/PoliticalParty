#include "InputReader.h"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

void InputReader::readCommands() {

    std::string line;

    while (std::getline(std::cin, line)){

        json args = json::parse(line);

        std::cout << args << std::endl;
        std::cout << args["leader"] << std::endl;
    }

}

InputReader::InputReader() = default;
