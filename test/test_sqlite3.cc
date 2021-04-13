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

void execute_sql(string stmt)
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
        sql = "CREATE TABLE IF NOT EXISTS identities("
              "id           INTEGER PRIMARY KEY     AUTOINCREMENT,"
              "address      TEXT    NOT NULL,"
              "UNIQUE (address));";

        execute_sql(sql);

        sql = "CREATE TABLE IF NOT EXISTS groups("
              "id           INTEGER PRIMARY KEY     AUTOINCREMENT,"
              "address      TEXT    NOT NULL,"
              "manager_id   INT     NOT NULL,"
              "UNIQUE (address),"
              "FOREIGN KEY(manager_id) REFERENCES identities(id) ON DELETE RESTRICT);";

        execute_sql(sql);

        sql = "CREATE TABLE IF NOT EXISTS members("
              "id           INTEGER PRIMARY KEY     AUTOINCREMENT,"
              "group_id     INT     NOT NULL,"
              "ident_id     INT     NOT NULL,"
              "UNIQUE (group_id, ident_id),"
              "FOREIGN KEY(group_id) REFERENCES groups(id) ON DELETE CASCADE,"
              "FOREIGN KEY(ident_id) REFERENCES identities(id) ON DELETE RESTRICT);";

        execute_sql(sql);
    } catch (...) {
        runtime_error e("create_tables() - failed with exception");
        throw_with_nested(e);
    }
}

void prune_identities()
{
    pEpLog("called");
    try {
        string sql = "DELETE FROM identities WHERE identities.id NOT IN (SELECT ident_id FROM members) "
                     "AND  identities.id NOT IN (SELECT manager_id FROM groups) ";
        execute_sql(sql);
    } catch (...) {
        runtime_error e("prune_identities() - failed with exception");
        throw_with_nested(e);
    }
}

void insert_identity(string addr)
{
    pEpLog("insert_identity(addr: \"" + addr + "\")");
    try {
        string sql;
        sql = "INSERT OR IGNORE INTO identities (address) VALUES ('" + addr + "');";
        execute_sql(sql);
    } catch (...) {
        runtime_error e("insert_identity(" + addr + ") - failed with exception");
        throw_with_nested(e);
    }
}

void delete_identity(string addr)
{
    pEpLog("delete_identity(addr: \"" + addr + "\")");
    try {
        string sql;
        sql = "DELETE FROM identities WHERE identities.address = '" + addr + "'";
        execute_sql(sql);
    } catch (...) {
        throw_with_nested(runtime_error("delete_identity(" + addr + ") - failed with exception"));
    }
}

void insert_group(string addr_grp, string addr_mgr)
{
    pEpLog("insert_group(addr_grp: \"" + addr_grp + "\"\taddr_mgr: \"" + addr_mgr + "\")");
    try {
        insert_identity(addr_mgr);
        string sql = "INSERT INTO groups(address, manager_id)"
                     "VALUES ('" +
                     addr_grp + "',(SELECT id FROM identities WHERE identities.address = '" +
                     addr_mgr + "'));";
        execute_sql(sql);
    } catch (...) {
        prune_identities();
        runtime_error e("insert_group(" + addr_grp + ", " + addr_mgr + ") - failed with exception");
        throw_with_nested(e);
    }
}


void delete_group(string addr)
{
    pEpLog("delete_group(addr: \"" + addr + "\")");
    try {
        string sql;
        sql = "DELETE FROM groups WHERE groups.address = '" + addr + "'";
        execute_sql(sql);
        // dont need to delete the group fk from members table since ON DELETE CASCADE
        prune_identities();
    } catch (...) {
        prune_identities();
        runtime_error e("delete_group(" + addr + ") - failed with exception");
        throw_with_nested(e);
    }
}

void insert_member(string addr_grp, string addr_member)
{
    pEpLog("insert_member(addr_grp: \"" + addr_grp + "\"\taddr_member: \"" + addr_member + "\")");
    try {
        insert_identity(addr_member);
        string sql = "INSERT INTO members(group_id, ident_id)"
                     "VALUES ("
                     "(SELECT id FROM groups WHERE groups.address = '" +
                     addr_grp +
                     "'),"
                     "(SELECT id FROM identities WHERE identities.address = '" +
                     addr_member + "'));";
        execute_sql(sql);
    } catch (...) {
        prune_identities();
        runtime_error e(
            "insert_member(" + addr_grp + ", " + addr_member + ") - failed with exception");
        throw_with_nested(e);
    }
}

void delete_member(string addr_grp, string addr_member)
{
    pEpLog("delete_member(addr_grp: \"" + addr_grp + "\",addr_member: '\"" + addr_member + "\")");
    try {
        string sql;
        sql = "DELETE FROM members WHERE"
              "(members.ident_id = (SELECT id FROM identities WHERE identities.address = '" +
              addr_member +
              "'))"
              "AND (members.group_id = (SELECT id FROM groups WHERE groups.address = '" +
              addr_grp + "'));";
        execute_sql(sql);
        prune_identities();
    } catch (...) {
        prune_identities();
        runtime_error e(
            "delete_member(" + addr_grp + ", " + addr_member + ") - failed with exception");
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
        db_config();
        create_tables();

        insert_group("grp1@peptest.org", "alice@peptest.org");
        insert_group("grp2@peptest.org", "alice@peptest.org");

        try {
            insert_group("grp1@peptest.org", "bob@peptest.org");
            assert(false);
        } catch (const exception &e) {
            print_exception(e);
        }

        cin >> dummy_in;
        insert_member("grp1@peptest.org", "bob@peptest.org");
        insert_member("grp1@peptest.org", "carol@peptest.org");
        insert_member("grp1@peptest.org", "joe@peptest.org");

        cin >> dummy_in;
        delete_group("grp1@peptest.org");

        cin >> dummy_in;
        try {
            delete_identity("alice@peptest.org");
            assert(false);
        } catch (const exception &e) {
            print_exception(e);
        }

    } catch (const exception &e) {
        print_exception(e);
    }
    sqlite3_close(db);
}