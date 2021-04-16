#include "pEpSQLite.hh"
#include "pEpLog.hh"
#include <iostream>
#include <cstdio>
#include <stdexcept>

using namespace std;

namespace pEp {
    bool pEpSQLite::log_enabled = false;

    pEpSQLite::pEpSQLite(std::string db_path) : db_path(db_path)
    {
        pEpLogClass("called with: db_path = "+ db_path +"");
    }

    void pEpSQLite::create_or_open_db()
    {
        pEpLogClass("called");
        int rc{::sqlite3_open(db_path.c_str(), &db)};

        if (rc) {
            ::sqlite3_close(db);
            runtime_error e{string("Can't open database (" + db_path + "): " + ::sqlite3_errmsg(db))};
            throw (e);
        }
    }

    string pEpSQLite::get_db_path()
    {
        pEpLogClass("called");
        return db_path;
    }

    void pEpSQLite::close_db()
    {
        pEpLogClass("called");
        ::sqlite3_close(db);
    }

    void pEpSQLite::delete_db()
    {
        pEpLogClass("called");
        int status = remove(db_path.c_str());
        if (status) {
            runtime_error e{string("could not delete db (" + db_path + "): " + strerror(errno))};
            throw (e);
        }
    }

    int pEpSQLite::callback(void *obj, int argc, char **argv, char **azColName)
    {
        RSRecord record;
        for (int col = 0; col < argc; col++) {
            const string key = string{azColName[col]};
            // TODO: NULL is not correct, could be a valid value
            const string val = string{argv[col] ? argv[col] : "NULL"};
            record.insert({key, val});
        }
        (static_cast<pEpSQLite *>(obj))->resultset.push_back(record);
        return 0;
    }

    ResultSet pEpSQLite::execute(const string& stmt)
    {
        if(db == nullptr) {
            runtime_error e{string("execute(): - Error: db is not open")};
            throw (e);
        } else {
            pEpLogClass("called");
            this->resultset.clear();
            char *zErrMsg = nullptr;
            int rc = ::sqlite3_exec(db, stmt.c_str(), (int (*)(void *, int, char **, char **)) &callback, this, &zErrMsg);
            if (rc != SQLITE_OK) {
                runtime_error e{string("execute: " + string(::sqlite3_errmsg(db)) + ":" + string(zErrMsg))};
                ::sqlite3_free(zErrMsg);
                throw (e);
            }
        }
        return resultset;
    }

    string pEpSQLite::resultset_to_string(const ResultSet& rs)
    {
        pEpLogClass("called");
        stringstream ss;
        int i = 0;
        for (const RSRecord& rec : rs) {
            for (const auto& item : rec) {
                ss << "RESULTSET[" << i << "][" << item.first << "] = " << item.second << "\"" << endl;
            }
            i++;
        }
        return ss.str();
    }

    pEpSQLite::~pEpSQLite()
    {
        pEpLogClass("called");
        close_db();
    }
}
