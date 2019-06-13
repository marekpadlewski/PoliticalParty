#include "InputReader.h"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

void InputReader::readInit() {

    std::string line;

    while (std::getline(std::cin, line)){

        json jobj = json::parse(line);

        if (!jobj["leader"].is_null()){

            dbfun.leader(jobj["leader"]);
        }

        else if (!jobj["open"].is_null()){

            dbfun.openInit(jobj["open"]);
        }
    }
}

void InputReader::readApp() {

    std::string line;

    while (std::getline(std::cin, line)){

        json jobj = json::parse(line);

        if (!jobj["protest"].is_null()){
            dbfun.createAction(jobj["protest"], "protest");
        }

        else if (!jobj["support"].is_null()){
            dbfun.createAction(jobj["support"], "support");
        }

        else if (!jobj["upvote"].is_null()){
            dbfun.vote(jobj["upvote"], "upvote");
        }

        else if (!jobj["downvote"].is_null()){
            dbfun.vote(jobj["downvote"], "downvote");
        }

        else if (!jobj["actions"].is_null()){
            dbfun.actions(jobj["actions"]);
        }

        else if (!jobj["projects"].is_null()){
            dbfun.projects(jobj["projects"]);
        }

        else if (!jobj["votes"].is_null()){
            dbfun.votes(jobj["votes"]);
        }

        else if (!jobj["trolls"].is_null()){
            dbfun.trolls(jobj["trolls"]);
        }
        else if (!jobj["open"].is_null()){

            dbfun.openApp(jobj["open"]);
        }
    }
}

InputReader::InputReader() = default;
