#include "InputReader.h"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

void InputReader::readInit() {

    std::string line;

    while (std::getline(std::cin, line)){

        json jobj = json::parse(line);

        if (!jobj["leader"].is_null()){
            pqxx::connection C("dbname=student host=localhost user=init password=qwerty");
            dbfun.leader(jobj["leader"], C);
        }

        else if (!jobj["open"].is_null()){

            dbfun.openInit(jobj["open"]);
        }
    }
}

void InputReader::readApp() {

    std::string line;

    pqxx::connection C("dbname=student host=localhost user=app password=qwerty");

    while (std::getline(std::cin, line)){

        json jobj = json::parse(line);

        if (!jobj["protest"].is_null()){
            dbfun.createAction(jobj["protest"], "protest", C);
        }

        else if (!jobj["support"].is_null()){
            dbfun.createAction(jobj["support"], "support", C);
        }

        else if (!jobj["upvote"].is_null()){
            dbfun.vote(jobj["upvote"], "upvote", C);
        }

        else if (!jobj["downvote"].is_null()){
            dbfun.vote(jobj["downvote"], "downvote", C);
        }

        else if (!jobj["actions"].is_null()){
            dbfun.actions(jobj["actions"], C);
        }

        else if (!jobj["projects"].is_null()){
            dbfun.projects(jobj["projects"], C);
        }

        else if (!jobj["votes"].is_null()){
            dbfun.votes(jobj["votes"], C);
        }

        else if (!jobj["trolls"].is_null()){
            dbfun.trolls(jobj["trolls"], C);
        }
        else if (!jobj["open"].is_null()){

            dbfun.openApp(jobj["open"]);
        }
    }
}

InputReader::InputReader() = default;
