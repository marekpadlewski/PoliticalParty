
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


        W.exec("INSERT INTO members (id, password, timestamp, isleader) VALUES (" + std::to_string(id) + ", '" + pass + "', " + std::to_string(ts) + "', " + std::to_string(true) + ")");

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

    int memberid = jobj["member"];
    long ts = jobj["timestamp"];
    std::string mempass = jobj["password"];
    int actionid = jobj["action"];
    int projectid = jobj["project"];

    pqxx::connection C("dbname=dbtest1");
    pqxx::work W(C);

    pqxx::result R;

    R = W.exec("SELECT id, password FROM members WHERE id = " + std::to_string(memberid));
    //if such member doesnt exist
    if (R.empty()){

        //check if memberid is taken
        R = W.exec("SELECT id FROM takenid WHERE id = " + std::to_string(memberid));

        //is not taken
        if (R.empty()){
            //we can create new member
            W.exec("INSERT INTO takenid VALUES (" + std::to_string(memberid) + ")");
            W.exec("INSERT INTO members (id, password, timestamp, isleader) "
                   "VALUES (" + std::to_string(memberid) + ", '" + mempass + "', " + std::to_string(ts) + "', " + std::to_string(false) + ")");
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
            R = W.exec("SELECT timestamp FROM members WHERE id = " + std::to_string(memberid));

            auto lastTS = R[0][0].as<long>();

            if (isFrozen(lastTS, ts)){
                flag = false;
                std::cout << "ER 3" << std::endl;
            }

            else {
                //update timestamp of member
                W.exec("UPDATE members SET timestamp = " + std::to_string(ts) + " WHERE id = " + std::to_string(memberid));
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
                           "VALUES (" + std::to_string(actionid) + ", " + std::to_string(projectid)  + ", " + std::to_string(memberid) + ", " + std::to_string(authid) + ", '" + type + "', " + std::to_string(ts) + ", 0, 0)");

                W.exec("INSERT INTO takenid VALUES (" + std::to_string(actionid) + ")");
            }

            else{
                flag = false;
                std::cout << "ER 5" << std::endl;
            }
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
                       "VALUES (" + std::to_string(actionid) + ", " + std::to_string(projectid)  + ", " + std::to_string(memberid) + ", " + std::to_string(authorityid) + ", '" + type + "', " + std::to_string(ts) + ", 0, 0)");

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
    bool flag = true;

    int memberid = jobj["member"];
    long ts = jobj["timestamp"];
    std::string mempass = jobj["password"];
    int actionid = jobj["action"];

    pqxx::connection C("dbname=dbtest1");
    pqxx::work W(C);

    pqxx::result R;

    R = W.exec("SELECT id, password FROM members WHERE id = " + std::to_string(memberid));
    //if such member doesnt exist
    if (R.empty()){

        //check if memberid is taken
        R = W.exec("SELECT id FROM takenid WHERE id = " + std::to_string(memberid));

        //is not taken
        if (R.empty()){
            //we can create new member
            W.exec("INSERT INTO takenid VALUES (" + std::to_string(memberid) + ")");
            W.exec("INSERT INTO members (id, password, timestamp, isleader) "
                   "VALUES (" + std::to_string(memberid) + ", '" + mempass + "', " + std::to_string(ts) + "', " + std::to_string(false) + ")");
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
            R = W.exec("SELECT timestamp FROM members WHERE id = " + std::to_string(memberid));

            auto lastTS = R[0][0].as<long>();

            if (isFrozen(lastTS, ts)){
                flag = false;
                std::cout << "ER 3" << std::endl;
            }

            else {
                //update timestamp of member
                W.exec("UPDATE members SET timestamp = " + std::to_string(ts) + " WHERE id = " + std::to_string(memberid));
            }
        }
    }

    //check if action already exist
    if (flag){
        R = W.exec("SELECT id FROM actions WHERE id = " + std::to_string(actionid));
        //if doesnt exist
        if (R.empty()){
            flag = false;
            std::cout << "ER 4" << std::endl;
        }

            //action already exist
        else{
            //check if member already voted for this action

            R = W.exec("SELECT * FROM votes WHERE actionid = " + std::to_string(actionid) + " AND memberid = " + std::to_string(memberid));

            if (R.empty()){
                //not voted
                W.exec("INSERT INTO votes VALUES (" + std::to_string(actionid) + ", " + std::to_string(memberid) + ", '" + type + "')");

                if (type == "upvote")
                    W.exec("UPDATE actions SET upvotes = upvotes + 1 WHERE id = " + std::to_string(actionid));
                else if (type == "downvote")
                    W.exec("UPDATE actions SET downvotes = downvotes + 1 WHERE id = " + std::to_string(actionid));
            }

            else{
                //already voted
                flag = false;
                std::cout << "ER 5" << std::endl;
            }
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


void DatabaseFunctions::actions(nlohmann::json jobj) {
    bool flag = true;

    long ts = jobj["timestamp"];
    int memberid = jobj["member"];
    std::string mempass = jobj["password"];

    std::string condition = "WHERE m.id = " + std::to_string(memberid) + " AND m.password = '" + mempass + "' AND m.isleader = true";

    if (jobj["type"] != nullptr){
        std::string type = jobj["type"];
        std::string s1 = " AND type = '" + type + "'";
        condition += s1;
    }

    if (jobj["project"] != nullptr){
        int project = jobj["project"];
        std::string s2 = " AND projectid = " + std::to_string(project);
        condition += s2;
    }

    if (jobj["authority"] != nullptr){
        int authority = jobj["authority"];
        std::string s3 = " AND authorityid = " + std::to_string(authority);
        condition += s3;
    }


    pqxx::connection C("dbname=dbtest1");
    pqxx::work W(C);

    pqxx::result R;

    R =  W.exec("SELECT id FROM members WHERE id = " + std::to_string(memberid) + " AND password = '" + mempass + "' AND isleader = true");

    //if member data is incorrect
    if (R.empty()){
        flag = false;
    }

    else{
        //check if member is frozen
        R = W.exec("SELECT timestamp FROM members WHERE id = " + std::to_string(memberid));

        auto lastTS = R[0][0].as<long>();

        if (isFrozen(lastTS, ts)){
            flag = false;

        }

        else
            W.exec("UPDATE members SET timestamp = " + std::to_string(ts) + "WHERE id = " + std::to_string(memberid));
    }

    if (flag){
        R = W.exec("SELECT actions.id, type, projectid, authorityid, upvotes, downvotes "
                   "FROM actions "
                   "JOIN members m ON m.id = actions.memberid " + condition + " ORDER BY actions.id");



        std::cout << "Found " << R.size() << " rows" << std::endl;

        for (auto row: R){
            for (auto &&i : row) {
                std::cout << i.c_str() << " ";
            }
            std::cout << std::endl;
        }

        W.commit();
        std::cout << confirmation << std::endl;
    }

    else{
        std::cout << negation << std::endl;
    }

}

void DatabaseFunctions::projects(nlohmann::json jobj) {
    bool flag = true;

    long ts = jobj["timestamp"];
    int memberid = jobj["member"];
    std::string mempass = jobj["password"];

    std::string condition;


    if (jobj["authority"] != nullptr){
        int authority = jobj["authority"];
        std::string s1 = " WHERE authorityid = " + std::to_string(authority);
        condition += s1;
    }


    pqxx::connection C("dbname=dbtest1");
    pqxx::work W(C);

    pqxx::result R;

    R =  W.exec("SELECT id FROM members WHERE id = " + std::to_string(memberid) + " AND password = '" + mempass + "' AND isleader = true");

    //if member data is incorrect
    if (R.empty()){
        flag = false;
    }

    else{
        //check if member is frozen
        R = W.exec("SELECT timestamp FROM members WHERE id = " + std::to_string(memberid));

        auto lastTS = R[0][0].as<long>();

        if (isFrozen(lastTS, ts)){
            flag = false;

        }

        else
            W.exec("UPDATE members SET timestamp = " + std::to_string(ts) + "WHERE id = " + std::to_string(memberid));
    }

    if (flag){
        R = W.exec("SELECT id, authorityid "
                   "FROM projects " + condition + " ORDER BY id");



        std::cout << "Found " << R.size() << " rows" << std::endl;

        for (auto row: R){
            for (auto &&i : row) {
                std::cout << i.c_str() << " ";
            }
            std::cout << std::endl;
        }

        W.commit();
        std::cout << confirmation << std::endl;
    }

    else{
        std::cout << negation << std::endl;
    }

}

void DatabaseFunctions::votes(nlohmann::json jobj) {
    bool flag = true, p = false;

    long ts = jobj["timestamp"];
    int memberid = jobj["member"];
    std::string mempass = jobj["password"];

    std::string condition;

    if (jobj["project"] != nullptr){
        int projectid = jobj["project"];
        std::string s1 = " JOIN actions a ON a.id = v.actionid WHERE a.projectid = " + std::to_string(projectid);
        condition += s1;
        p = true;
    }

    if (jobj["action"] != nullptr){
        int actionid = jobj["action"];
        std::string s2 = " actionid = " + std::to_string(actionid);

        condition += (p ? " AND " : " WHERE ");

        condition += s2;
    }


    pqxx::connection C("dbname=dbtest1");
    pqxx::work W(C);

    pqxx::result R;

    R =  W.exec("SELECT id FROM members WHERE id = " + std::to_string(memberid) + " AND password = '" + mempass + "' AND isleader = true");

    //if member data is incorrect
    if (R.empty()){
        flag = false;
    }

    else{
        //check if member is frozen
        R = W.exec("SELECT timestamp FROM members WHERE id = " + std::to_string(memberid));

        auto lastTS = R[0][0].as<long>();

        if (isFrozen(lastTS, ts)){
            flag = false;
        }

        else
            W.exec("UPDATE members SET timestamp = " + std::to_string(ts) + "WHERE id = " + std::to_string(memberid));
    }

    if (flag){
        R = W.exec("SELECT v.memberid, "
                   "COUNT (v.type) FILTER (WHERE v.type = 'upvote') AS num_upvotes, "
                   "COUNT (v.type) FILTER (WHERE v.type = 'downvote') AS num_downvotes "
                   "FROM votes v " + condition + " GROUP BY v.memberid " " ORDER BY v.memberid");



        std::cout << "Found " << R.size() << " rows" << std::endl;

        for (auto row: R){
            for (auto &&i : row) {
                std::cout << i.c_str() << " ";
            }
            std::cout << std::endl;
        }

        W.commit();
        std::cout << confirmation << std::endl;
    }

    else{
        std::cout << negation << std::endl;
    }


}

void DatabaseFunctions::trolls(nlohmann::json jobj) {

}

bool DatabaseFunctions::isFrozen(long lastTS, long currTS) {

    return currTS - lastTS > 31556908800;

}



