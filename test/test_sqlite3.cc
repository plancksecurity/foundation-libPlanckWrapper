#include "../src/sqlite3.h"
#include <iostream>

using namespace std;
sqlite3 *db;

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
    char *zErrMsg = 0;
    int rc = sqlite3_exec(db, stmt.c_str(), callback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        cout << "SQL error: " << zErrMsg << endl;
        sqlite3_free(zErrMsg);
    }
}

void open_db()
{
    int rc = sqlite3_open("test.db", &db);

    if (rc) {
        cout << "Can't open database:" << sqlite3_errmsg(db) << endl;
    } else {
        cout << "Opened database successfully" << endl;
    }
}

int main(int argc, char *argv[])
{
    string sql;
    open_db();
    sql = "CREATE TABLE GROUPS("
          "ADDRESS      TEXT    PRIMARY KEY NOT NULL,"
          "MANAGER_ADDR TEXT    NOT NULL);";

    execute_sql(sql);

    sql = "INSERT INTO GROUPS (ADDRESS, MANAGER_ADDR) "
          "VALUES ('fdjklsf@fsffd.com', 'mgr1@fsd.com');";

    execute_sql(sql);

    sqlite3_close(db);
}