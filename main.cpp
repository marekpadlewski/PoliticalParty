#include <iostream>
#include "json.hpp"
#include <pqxx/pqxx>
#include "InputReader.h"

using json = nlohmann::json;

int main() {

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

    inputReader.readCommands();



    return 0;
}