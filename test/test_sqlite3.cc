#include "../src/sqlite3.h"
#include "../src/pEpLog.hh"
#include <iostream>
#include <cstdio>
#include <exception>
#include <stdexcept>

//TODO: add const

using namespace std;

sqlite3 *db;
string db_path;

void print_exception(const exception &e, int level = 0)
{
    cerr << string(level, ' ') << "exception: " << e.what() << endl;
    try {
        rethrow_if_nested(e);
    } catch (const exception &e) {
        print_exception(e, level + 1);
    } catch (...) {
    }
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    int i;
    for (i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

void execute_sql(const string &stmt)
{
    //    pEpLog("execute_sql(\"" + stmt + "\")");
    char *zErrMsg = nullptr;
    int rc = sqlite3_exec(db, stmt.c_str(), callback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        runtime_error e{ string("execute_sql: " + string(sqlite3_errmsg(db)) + ":" + string(zErrMsg)) };
        sqlite3_free(zErrMsg);
        throw(e);
    }
}

void create_or_open_db()
{
    pEpLog("called");
    int rc = sqlite3_open(db_path.c_str(), &db);

    if (rc) {
        runtime_error e{ string("Can't open database (" + db_path + "):" + sqlite3_errmsg(db)) };
        throw(e);
    }
}

void delete_db()
{
    pEpLog("called");
    remove(db_path.c_str());
    if (errno) {
        cerr << "could not delete db (" + db_path + "): " << strerror(errno) << endl;
    }
}

void db_config()
{
    pEpLog("called");
    try {
        string sql;
        sql = "PRAGMA foreign_keys=ON";
        execute_sql(sql);
    } catch (...) {
        runtime_error e("db_config() - failed with exception");
        throw_with_nested(e);
    }
}

void create_tables()
{
    pEpLog("called");
    try {
        string sql;

        sql = "CREATE TABLE IF NOT EXISTS lists("
              "address      TEXT     NOT NULL,"
              "manager_id   TEXT     NOT NULL,"
              "PRIMARY KEY(address));";
        execute_sql(sql);

        sql = "CREATE TABLE IF NOT EXISTS member_of("
              "address      TEXT     NOT NULL,"
              "list_address TEXT     NOT NULL,"
              "PRIMARY KEY (address, list_address),"
              "FOREIGN KEY(list_address) REFERENCES lists(address) ON DELETE CASCADE);";
        execute_sql(sql);
    } catch (...) {
        runtime_error e("create_tables() - failed with exception");
        throw_with_nested(e);
    }
}

void list_add(const string &addr_list, const string &addr_mgr)
{
    pEpLog("list_add(addr_list: \"" + addr_list + "\"\taddr_mgr: \"" + addr_mgr + "\")");
    try {
        string sql = "INSERT INTO lists(address, manager_id)"
                     "VALUES ('" +
                     addr_list + "','" + addr_mgr + "');";
        execute_sql(sql);
    } catch (...) {
        runtime_error e(
            "list_add(addr_list: \"" + addr_list + "\"\taddr_mgr: \"" + addr_mgr +
            "\") - failed with exception");
        throw_with_nested(e);
    }
}

void list_delete(const string &addr_list)
{
    pEpLog("list_delete(addr_list: \"" + addr_list + "\")");
    try {
        string sql;
        sql = "DELETE FROM lists WHERE lists.address = '" + addr_list + "'";
        execute_sql(sql);
    } catch (...) {
        runtime_error e("list_delete(addr_list: \"" + addr_list + "\") - failed with exception");
        throw_with_nested(e);
    }
}

void member_add(const string &addr_list, const string &addr_member)
{
    pEpLog("member_add(addr_list: \"" + addr_list + "\", addr_member: \"" + addr_member + "\")");
    try {
        string sql = "INSERT INTO member_of(address, list_address)"
                     "VALUES ('" +
                     addr_member +
                     "',"
                     "'" +
                     addr_list + "');";
        execute_sql(sql);
    } catch (...) {
        runtime_error e(
            "member_add(addr_list: \"" + addr_list + "\", addr_member: \"" + addr_member +
            "\") - failed with exception");
        throw_with_nested(e);
    }
}

void member_remove(const string &addr_list, const string &addr_member)
{
    pEpLog("member_remove(addr_list: \"" + addr_list + "\", addr_member: '\"" + addr_member + "\")");
    try {
        string sql;
        sql = "DELETE FROM member_of WHERE"
              "(member_of.address = '" +
              addr_member + "') AND (member_of.list_address = '" + addr_list + "');";
        execute_sql(sql);
    } catch (...) {
        runtime_error e(
            "member_remove(" + addr_list + ", " + addr_member + ") - failed with exception");
        throw_with_nested(e);
    }
}

int main(int argc, char *argv[])
{
    pEp::Adapter::pEpLog::set_enabled(true);
    db_path = "test.db";
    string dummy_in;
    try {
        delete_db();
        create_or_open_db();
        cin >> dummy_in;
        db_config();
        create_tables();

        list_add("grp1@peptest.org", "alice@peptest.org");
        list_add("grp2@peptest.org", "alice@peptest.org");

        try {
            list_add("grp1@peptest.org", "bob@peptest.org");
            assert(false);
        } catch (const exception &e) {
            print_exception(e);
        }

        cin >> dummy_in;
        member_add("grp1@peptest.org", "bob@peptest.org");
        member_add("grp1@peptest.org", "carol@peptest.org");
        member_add("grp1@peptest.org", "joe@peptest.org");

        cin >> dummy_in;
        member_remove("grp1@peptest.org", "joe@peptest.org");

        cin >> dummy_in;
        list_delete("grp1@peptest.org");

        cin >> dummy_in;
    } catch (const exception &e) {
        print_exception(e);
    }
    sqlite3_close(db);
}