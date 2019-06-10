#include "InputReader.h"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

void InputReader::readInit() {

    std::string line;

    while (std::getline(std::cin, line)){

        json jobj = json::parse(line);



        if (jobj["leader"] != nullptr){

            dbfun.leader(jobj["leader"]);
        }

        else if (jobj["open"] != nullptr){

            dbfun.open(jobj["open"]);
        }


    }

}

void InputReader::readApp() {

    std::string line;

    while (std::getline(std::cin, line)){

        json jobj = json::parse(line);

        if (jobj["protest"] != nullptr){
            dbfun.createAction(jobj["protest"], "protest");
        }

        if (jobj["support"] != nullptr){
            dbfun.createAction(jobj["support"], "support");
        }
    }

}

InputReader::InputReader() = default;
