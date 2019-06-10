
#include "DatabaseFunctions.h"

DatabaseFunctions::DatabaseFunctions() {
    //creating json output objects
    confirmation["status"] = "OK";
    negation["status"] = "ERROR";
}

void DatabaseFunctions::open(nlohmann::json jobj) {

    std::cout << "OPEN" << std::endl;

}

void DatabaseFunctions::leader(nlohmann::json jobj) {

    int id = jobj["member"];
    long ts = jobj["timestamp"];
    std::string pass = jobj["password"];

    pqxx::connection C("dbname=dbtest1");
    pqxx::work W(C);

    //check if memberid is taken
    pqxx::result R = W.exec("SELECT id FROM takenid WHERE id = " + std::to_string(id));

    //id is not taken
    if (R.empty()){


        W.exec("INSERT INTO takenid VALUES (" + std::to_string(id) + ")");


        W.exec("INSERT INTO members (id, password, timestamp) VALUES (" + std::to_string(id) + ", '" + pass + "', " + std::to_string(ts) + ")");

        W.commit();

        std::cout << confirmation << std::endl;
    }

    //id is taken
    else{
        std::cout << negation << std::endl;
    }
}

void DatabaseFunctions::createAction(nlohmann::json jobj, std::string type) {
    bool flag = true;

    int memid = jobj["member"];
    long ts = jobj["timestamp"];
    std::string mempass = jobj["password"];
    int actionid = jobj["action"];
    int projectid = jobj["project"];

    pqxx::connection C("dbname=dbtest1");
    pqxx::work W(C);

    pqxx::result R;

    R = W.exec("SELECT id, password FROM members WHERE id = " + std::to_string(memid));
    //if such member doesnt exist
    if (R.empty()){

        //check if memberid is taken
        R = W.exec("SELECT id FROM takenid WHERE id = " + std::to_string(memid));

        //is not taken
        if (R.empty()){
            //we can create new member
            W.exec("INSERT INTO takenid VALUES (" + std::to_string(memid) + ")");
            W.exec("INSERT INTO members (id, password, timestamp) "
                   "VALUES (" + std::to_string(memid) + ", '" + mempass + "', " + std::to_string(ts) + ")");
        }

        else{
            //memberid is taken
            flag = false;
            std::cout << "ER 1" << std::endl;
        }
    }

    //such member already exist
    else{
        std::string currMemPass = R[0][1].as<std::string>();


        //check if password is not correct
        if (currMemPass != mempass){
            flag = false;
            std::cout << "ER 2" << std::endl;
        }

        //password is correct
        else{
            //check if member is frozen
            R = W.exec("SELECT timestamp FROM members WHERE id = " + std::to_string(memid));

            auto lastTS = R[0][0].as<long>();

            if (isFrozen(lastTS, ts)){
                flag = false;
                std::cout << "ER 3" << std::endl;
            }

            else {
                //update timestamp of member
                W.exec("UPDATE members SET timestamp = " + std::to_string(ts) + " WHERE id = " + std::to_string(memid));
            }

        }
    }


    //check if project already exist when theres no authority
    if (jobj["authority"] == nullptr && flag){
        R = W.exec("SELECT id, authorityid FROM projects WHERE id = " + std::to_string(projectid));
        //if doesnt exist
        if (R.empty()){
            flag = false;
            std::cout << "ER 4" << std::endl;
        }

        //project already exist
        else{
            //add new action
            auto authid = R[0][1].as<int>();

            //check if actionid is not taken
            R = W.exec("SELECT id FROM takenid WHERE id = " + std::to_string(actionid));

            if (R.empty()){
                W.exec("INSERT INTO actions "
                           "VALUES (" + std::to_string(actionid) + ", " + std::to_string(projectid)  + ", " + std::to_string(memid) + ", " + std::to_string(authid) + ", '" + type + "', " + std::to_string(ts) + ", 0, 0)");

                W.exec("INSERT INTO takenid VALUES (" + std::to_string(actionid) + ")");
            }

            else{
                flag = false;
                std::cout << "ER 5" << std::endl;
            }

;
        }

    }

    else if (flag){
        int authorityid = jobj["authority"];

        //check if actionid, project and authorityid is not taken
        R = W.exec("SELECT id FROM takenid WHERE id = " + std::to_string(actionid) + " OR id = " + std::to_string(projectid) + " OR id = " + std::to_string(authorityid));

        // ids not taken
        if (R.empty()){

            W.exec("INSERT INTO projects "
                       "VALUES (" + std::to_string(projectid) + ", " + std::to_string(authorityid) + ")");


            W.exec("INSERT INTO actions "
                       "VALUES (" + std::to_string(actionid) + ", " + std::to_string(projectid)  + ", " + std::to_string(memid) + ", " + std::to_string(authorityid) + ", '" + type + "', " + std::to_string(ts) + ", 0, 0)");

            W.exec("INSERT INTO takenid VALUES (" + std::to_string(projectid) + ")");
            W.exec("INSERT INTO takenid VALUES (" + std::to_string(actionid) + ")");
            W.exec("INSERT INTO takenid VALUES (" + std::to_string(authorityid) + ")");

        }

        else{
            flag = false;
            std::cout << "ER 6" << std::endl;
        }



    }


    //all is correct
    if (flag){
        W.commit();

        std::cout << confirmation << std::endl;
    }

    else{
        std::cout << negation << std::endl;
    }
}

void DatabaseFunctions::vote(nlohmann::json jobj, std::string type) {

}


void DatabaseFunctions::actions(nlohmann::json jobj) {

}

void DatabaseFunctions::projects(nlohmann::json jobj) {

}

void DatabaseFunctions::votes(nlohmann::json jobj) {

}

void DatabaseFunctions::trolls(nlohmann::json jobj) {

}

bool DatabaseFunctions::isFrozen(long lastTS, long currTS) {

    return currTS - lastTS > 31556908800;

}



