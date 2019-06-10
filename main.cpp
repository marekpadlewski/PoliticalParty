#include <iostream>
#include "json.hpp"
#include <pqxx/pqxx>
#include "InputReader.h"

using json = nlohmann::json;

int main(int argc, char * argv[]) {

/*

     // a JSON text
    auto text = R"(
     { "leader": { "timestamp": 1557475000, "password": "abcde", "member": 1}}
     )";

    // parse and serialize JSON
    json j_complete = json::parse(text);

    //std::cout << j_complete["leader"];

    try
    {
        pqxx::connection C("dbname=dbtest1");
        std::cout << "Connected to " << C.dbname() << std::endl;
        pqxx::work W(C);

        pqxx::result R = W.exec("SELECT kod_przed, nazwa FROM przedmiot ORDER BY 2");

        std::cout << "Found " << R.size() << " lessons:" << std::endl;
        for (auto row: R)
            std::cout << row[0].c_str() << " " << row[1].c_str() << std::endl;



        //std::cout << "Doubling all employees' salaries..." << std::endl;
        W.exec("UPDATE przedmiot SET nazwa = 'Algebra' WHERE kod_przed = 1");

        std::cout << "Making changes definite: ";
        W.commit();
        std::cout << "OK." << std::endl;


    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    */

    InputReader inputReader;

    inputReader.dbfun = DatabaseFunctions();


    //INIT user
    if (argc == 2){
        std::string arg = argv[1];

        if (arg == "--init"){
            std::cout << "INIT" << std::endl;
            inputReader.readInit();
        }


        else{
            std::cerr << "Wrong arguments!";
            return -1;
        }
    }

    //APP user
    else{
        std::cout << "APP" << std::endl;

        inputReader.readApp();
    }





    return 0;
}