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

        else if (jobj["support"] != nullptr){
            dbfun.createAction(jobj["support"], "support");
        }

        else if (jobj["upvote"] != nullptr){
            dbfun.vote(jobj["upvote"], "upvote");
        }

        else if (jobj["downvote"] != nullptr){
            dbfun.vote(jobj["downvote"], "downvote");
        }

        else if (jobj["actions"] != nullptr){
            dbfun.actions(jobj["actions"]);
        }

        else if (jobj["projects"] != nullptr){
            dbfun.projects(jobj["projects"]);
        }

        else if (jobj["votes"] != nullptr){
            dbfun.votes(jobj["votes"]);
        }

        else if (jobj["trolls"] != nullptr){
            dbfun.trolls(jobj["trolls"]);
        }


    }

}

InputReader::InputReader() = default;
