#pragma once

#include <iostream>
#include "json.hpp"
#include <pqxx/pqxx>

class DatabaseFunctions {

public:
    DatabaseFunctions();

    void open(nlohmann::json jobj);
    void leader(nlohmann::json jobj);
    void createAction(nlohmann::json jobj, std::string type);
    void vote(nlohmann::json jobj, std::string type);
    void actions(nlohmann::json jobj);
    void projects(nlohmann::json jobj);
    void votes(nlohmann::json jobj);
    void trolls(nlohmann::json jobj);
    bool isFrozen(long lastTS, long currTS);

private:
    nlohmann::json confirmation;
    nlohmann::json negation;

};


