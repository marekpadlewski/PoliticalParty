#pragma once

#include <iostream>
#include "json.hpp"
#include <pqxx/pqxx>

class DatabaseFunctions {

public:
    DatabaseFunctions();

    void openInit(nlohmann::json jobj);
    void openApp(nlohmann::json jobj);
    void leader(nlohmann::json jobj, pqxx::connection_base &C);
    void createAction(nlohmann::json jobj, const std::string &type, pqxx::connection_base &C);
    void vote(nlohmann::json jobj, const std::string &type, pqxx::connection_base &C);
    void actions(nlohmann::json jobj, pqxx::connection_base &C);
    void projects(nlohmann::json jobj, pqxx::connection_base &C);
    void votes(nlohmann::json jobj, pqxx::connection_base &C);
    void trolls(nlohmann::json jobj, pqxx::connection_base &C);

private:
    nlohmann::json confirmation;
    nlohmann::json negation;
    std::string db_name;
    bool isFrozen(long lastTS, long currTS);

};


