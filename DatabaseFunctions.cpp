#include <fstream>
#include "DatabaseFunctions.h"

DatabaseFunctions::DatabaseFunctions() {
    //creating json output objects
    confirmation["status"] = "OK";
    negation["status"] = "ERROR";
}

void DatabaseFunctions::openInit(nlohmann::json jobj) {

    std::string login = jobj["login"];
    std::string password = jobj["password"];
    std::string dbname = jobj["database"];

    if (login == "init" && password == "qwerty"){
        std::ifstream f("dbinit.sql");
        std::stringstream buffer;
        buffer << f.rdbuf();

        pqxx::connection C("dbname=" + dbname + " host=localhost user=" + login + " password=" + password);
        pqxx::work W(C);

        db_name = dbname;

        //create all tables, relations etc
        pqxx::result R = W.exec(buffer.str());


        //create user app
        R = W.exec("CREATE USER app WITH ENCRYPTED PASSWORD 'qwerty'");
        R = W.exec("GRANT ALL PRIVILEGES ON DATABASE " + dbname + " TO app");
        R = W.exec("ALTER USER app WITH SUPERUSER");


        W.commit();


        std::cout << confirmation << std::endl;
    }

    else{
        std::cout << negation << std::endl;
    }

}

void DatabaseFunctions::openApp(nlohmann::json jobj) {

    std::string login = jobj["login"];
    std::string password = jobj["password"];
    std::string dbname = jobj["database"];


    if (login == "app" && password == "qwerty"){
        db_name = dbname;
        std::cout << confirmation << std::endl;
    }

    else{
        std::cout << negation << std::endl;
    }

}

void DatabaseFunctions::leader(nlohmann::json jobj, pqxx::connection_base &C) {

    int id = jobj["member"];
    long ts = jobj["timestamp"];
    std::string pass = jobj["password"];

    pqxx::work W(C);

    //check if memberid is taken
    pqxx::result R = W.exec("SELECT id FROM takenid WHERE id = " + std::to_string(id));

    //id is not taken
    if (R.empty()){


        W.exec("INSERT INTO takenid VALUES (" + std::to_string(id) + ")");


        W.exec("INSERT INTO members (id, password, timestamp, isleader) VALUES (" + std::to_string(id) + ", crypt('" + pass + "', gen_salt('bf')), " + std::to_string(ts) + ", true)");

        W.commit();

        std::cout << confirmation << std::endl;
    }

    //id is taken
    else{
        std::cout << negation << std::endl;
    }
}

void DatabaseFunctions::createAction(nlohmann::json jobj, const std::string &type, pqxx::connection_base &C) {
    bool flag = true;

    int memberid = jobj["member"];
    long ts = jobj["timestamp"];
    std::string mempass = jobj["password"];
    int actionid = jobj["action"];
    int projectid = jobj["project"];

    pqxx::work W(C);
    pqxx::result R;

    R = W.exec("SELECT id FROM members WHERE id = " + std::to_string(memberid));
    //if such member doesnt exist
    if (R.empty()){

        //check if memberid is taken
        R = W.exec("SELECT id FROM takenid WHERE id = " + std::to_string(memberid));

        //is not taken
        if (R.empty()){
            //we can create new member
            W.exec("INSERT INTO takenid VALUES (" + std::to_string(memberid) + ")");
            W.exec("INSERT INTO members (id, password, timestamp, isleader) "
                   "VALUES (" + std::to_string(memberid) + ", crypt('" + mempass + "', gen_salt('bf')), " + std::to_string(ts) + ", false)");
        }

        else{
            //memberid is taken
            flag = false;
        }
    }

    //such member already exist
    else{
        R = W.exec("SELECT id FROM members WHERE id = " + std::to_string(memberid) + " AND password = crypt('" + mempass + "', password)");

        //check if password is not correct
        if (R.empty()){
            flag = false;
        }

        //password is correct
        else{
            //check if member is frozen
            R = W.exec("SELECT timestamp FROM members WHERE id = " + std::to_string(memberid));

            auto lastTS = R[0][0].as<long>();

            if (isFrozen(lastTS, ts)){
                flag = false;
            }

            else {
                //update timestamp of member
                W.exec("UPDATE members SET timestamp = " + std::to_string(ts) + " WHERE id = " + std::to_string(memberid));
            }
        }
    }


    if (flag){
        //check if project already exist
        R = W.exec("SELECT id FROM projects WHERE id = " + std::to_string(projectid));

        //if doesnt exist
        if (R.empty()){

            int authorityid = jobj["authority"];

            //check if authority already exist
            R = W.exec("SELECT id FROM projects WHERE authorityid = " + std::to_string(authorityid));

            std::string condition;
            bool auth_exist = true;
            //authority not exist
            if (R.empty()){
                condition += " OR id = ";
                condition += std::to_string(authorityid);
                auth_exist = false;
            }

            //check if actionid, project and ?authorityid? is not taken
            R = W.exec("SELECT id FROM takenid WHERE id = " + std::to_string(actionid) + " OR id = " + std::to_string(projectid) + condition);

            // ids not taken
            if (R.empty() && actionid != projectid && projectid != authorityid){

                W.exec("INSERT INTO projects "
                       "VALUES (" + std::to_string(projectid) + ", " + std::to_string(authorityid) + ")");


                W.exec("INSERT INTO actions "
                       "VALUES (" + std::to_string(actionid) + ", " + std::to_string(projectid)  + ", " + std::to_string(memberid) + ", " + std::to_string(authorityid) + ", '" + type + "', " + std::to_string(ts) + ", 0, 0)");

                W.exec("INSERT INTO takenid VALUES (" + std::to_string(projectid) + ")");
                W.exec("INSERT INTO takenid VALUES (" + std::to_string(actionid) + ")");


                if (!auth_exist)
                    W.exec("INSERT INTO takenid VALUES (" + std::to_string(authorityid) + ")");

            }

            else{
                flag = false;
            }

        }

        //project already exist
        else{
            //add new action

            R = W.exec("SELECT authorityid FROM projects WHERE id = " + std::to_string(projectid));

            auto authid = R[0][0].as<int>();

            //check if actionid is not taken
            R = W.exec("SELECT id FROM takenid WHERE id = " + std::to_string(actionid));

            if (R.empty()){
                W.exec("INSERT INTO actions "
                           "VALUES (" + std::to_string(actionid) + ", " + std::to_string(projectid)  + ", " + std::to_string(memberid) + ", " + std::to_string(authid) + ", '" + type + "', " + std::to_string(ts) + ", 0, 0)");

                W.exec("INSERT INTO takenid VALUES (" + std::to_string(actionid) + ")");
            }

            else{
                flag = false;
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

void DatabaseFunctions::vote(nlohmann::json jobj, const std::string &type, pqxx::connection_base &C) {
    bool flag = true;

    int memberid = jobj["member"];
    long ts = jobj["timestamp"];
    std::string mempass = jobj["password"];
    int actionid = jobj["action"];

    pqxx::work W(C);
    pqxx::result R;

    R = W.exec("SELECT id FROM members WHERE id = " + std::to_string(memberid));
    //if such member doesnt exist
    if (R.empty()){

        //check if memberid is taken
        R = W.exec("SELECT id FROM takenid WHERE id = " + std::to_string(memberid));

        //is not taken
        if (R.empty()){
            //we can create new member
            W.exec("INSERT INTO takenid VALUES (" + std::to_string(memberid) + ")");
            W.exec("INSERT INTO members (id, password, timestamp, isleader) "
                   "VALUES (" + std::to_string(memberid) + ", crypt('" + mempass + "', gen_salt('bf')), " + std::to_string(ts) + ", false)");
        }

        else{
            //memberid is taken
            flag = false;
        }
    }

        //such member already exist
    else{
        R = W.exec("SELECT id FROM members WHERE id = " + std::to_string(memberid) + " AND password = crypt('" + mempass + "', password)");

        //check if password is not correct
        if (R.empty()){
            flag = false;
        }

            //password is correct
        else{
            //check if member is frozen
            R = W.exec("SELECT timestamp FROM members WHERE id = " + std::to_string(memberid));

            auto lastTS = R[0][0].as<long>();

            if (isFrozen(lastTS, ts)){
                flag = false;
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


void DatabaseFunctions::actions(nlohmann::json jobj, pqxx::connection_base &C) {
    bool flag = true;

    long ts = jobj["timestamp"];
    int memberid = jobj["member"];
    std::string mempass = jobj["password"];

    std::string condition = "WHERE true ";

    if (!jobj["type"].is_null()){
        std::string type = jobj["type"];
        std::string s1 = " AND type = '" + type + "'";
        condition += s1;
    }

    if (!jobj["project"].is_null()){
        int project = jobj["project"];
        std::string s2 = " AND projectid = " + std::to_string(project);
        condition += s2;
    }

    if (!jobj["authority"].is_null()){
        int authority = jobj["authority"];
        std::string s3 = " AND authorityid = " + std::to_string(authority);
        condition += s3;
    }

    pqxx::work W(C);
    pqxx::result R;

    R =  W.exec("SELECT id FROM members WHERE id = " + std::to_string(memberid) + " AND password = crypt('" + mempass + "', password) AND isleader ");

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


        nlohmann::json res;
        res["status"] = "OK";

        nlohmann::json l, d;

        for (auto row: R){
            l.clear();
            for (int i = 0 ; i < row.size() ; i++) {

                if (i == 1)
                    l.push_back(row[i].as<std::string>());
                else
                    l.push_back(row[i].as<int>());
            }

            d.push_back(l);
        }

        if (d.is_null()){
            std::vector<int> v {};
            nlohmann::json j_vec(v);
            d = j_vec;
        }

        res["data"] = d;

        std::cout << res << std::endl;

        W.commit();
    }

    else{
        std::cout << negation << std::endl;
    }

}

void DatabaseFunctions::projects(nlohmann::json jobj, pqxx::connection_base &C) {
    bool flag = true;

    long ts = jobj["timestamp"];
    int memberid = jobj["member"];
    std::string mempass = jobj["password"];

    std::string condition;


    if (!jobj["authority"].is_null()){
        int authority = jobj["authority"];
        std::string s1 = " WHERE authorityid = " + std::to_string(authority);
        condition += s1;
    }

    pqxx::work W(C);
    pqxx::result R;

    R =  W.exec("SELECT id FROM members WHERE id = " + std::to_string(memberid) + " AND password = crypt('" + mempass + "', password) AND isleader ");

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

        nlohmann::json res;
        res["status"] = "OK";

        std::vector<int> l;
        std::vector<std::vector<int>> d;

        for (auto row: R){
            l.clear();
            for (int i = 0 ; i < row.size() ; i++) {
                l.push_back(row[i].as<int>());
            }

            d.push_back(l);
        }


        nlohmann::json j_vec(d);
        res["data"] = j_vec;

        std::cout << res << std::endl;

        W.commit();
    }

    else{
        std::cout << negation << std::endl;
    }

}

void DatabaseFunctions::votes(nlohmann::json jobj, pqxx::connection_base &C) {
    bool flag = true;

    long ts = jobj["timestamp"];
    int memberid = jobj["member"];
    std::string mempass = jobj["password"];

    std::string condition1, condition2;

    if (!jobj["project"].is_null()){
        int projectid = jobj["project"];
        condition1 = " LEFT JOIN actions a ON a.id = v.actionid";
        std::string s1 = " AND a.projectid = " + std::to_string(projectid);
        condition2 += s1;
    }

    if (!jobj["action"].is_null()){
        int actionid = jobj["action"];
        std::string s2 = " AND v.actionid = " + std::to_string(actionid);

        condition2 += s2;
    }

    pqxx::work W(C);
    pqxx::result R;

    R =  W.exec("SELECT id FROM members WHERE id = " + std::to_string(memberid) + " AND password = crypt('" + mempass + "', password) AND isleader ");

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
        R = W.exec("SELECT m.id, "
                   "COUNT (v.type) FILTER (WHERE v.type = 'upvote'" + condition2 + ") AS num_upvotes, "
                   "COUNT (v.type) FILTER (WHERE v.type = 'downvote'" + condition2 + ") AS num_downvotes "
                   "FROM votes v FULL OUTER JOIN members m ON m.id = v.memberid " + condition1 + " GROUP BY m.id  ORDER BY 1");

        nlohmann::json res;
        res["status"] = "OK";

        std::vector<int> l;
        std::vector<std::vector<int>> d;

        for (auto row: R){
            l.clear();
            for (int i = 0 ; i < row.size() ; i++) {
                l.push_back(row[i].as<int>());
            }

            d.push_back(l);
        }


        nlohmann::json j_vec(d);
        res["data"] = j_vec;

        std::cout << res << std::endl;

        W.commit();
    }

    else{
        std::cout << negation << std::endl;
    }


}

void DatabaseFunctions::trolls(nlohmann::json jobj, pqxx::connection_base &C) {
    bool flag = true;

    long ts = jobj["timestamp"];

    pqxx::work W(C);
    pqxx::result R;

    R = W.exec("SELECT * FROM (SELECT memberid, SUM(upvotes) AS num_upvotes, SUM(downvotes) "
               "AS num_downvotes, CASE WHEN " + std::to_string(ts) +
               " BETWEEN m.timestamp AND m.timestamp + 31556900000 "
               " THEN true ELSE false END AS active FROM actions JOIN members m ON m.id = actions.memberid "
               " GROUP BY memberid, m.timestamp) AS trolls WHERE num_upvotes < num_downvotes "
               " ORDER BY num_downvotes-num_upvotes DESC, memberid");


    nlohmann::json res;
    res["status"] = "OK";

    nlohmann::json l, d;

    for (auto row: R){
        l.clear();
        for (int i = 0 ; i < 3 ; i++) {
            l.push_back(row[i].as<int>());
        }


        l.push_back(row[3].as<bool>() ? "true" : "false");
        d.push_back(l);
    }

    if (d.is_null()){
        std::vector<int> v {};
        nlohmann::json j_vec(v);
        d = j_vec;
    }


    res["data"] = d;


    std::cout << res << std::endl;
}


bool DatabaseFunctions::isFrozen(long lastTS, long currTS) {

                            //one year in milliseconds
    return currTS - lastTS > 31556900000;

}



