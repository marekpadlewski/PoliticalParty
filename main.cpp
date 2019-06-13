#include <iostream>
#include "json.hpp"
#include <pqxx/pqxx>
#include "InputReader.h"

using json = nlohmann::json;

int main(int argc, char * argv[]) {

    InputReader inputReader;

    inputReader.dbfun = DatabaseFunctions();


    //INIT user
    if (argc == 2){
        std::string arg = argv[1];

        if (arg == "--init"){
            inputReader.readInit();
        }


        else{
            std::cerr << "Wrong arguments!";
            return -1;
        }
    }

    //APP user
    else{
        inputReader.readApp();
    }





    return 0;
}