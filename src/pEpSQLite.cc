#include "pEpSQLite.hh"
#include "pEpLog.hh"
#include <iostream>
#include <cstdio>
#include <stdexcept>
#include <string>
#include <cstring>

using namespace std;

namespace pEp {
    bool pEpSQLite::log_enabled = false;

    pEpSQLite::pEpSQLite(const std::string &db_path) : db_path(db_path)
    {
        pEpLogClass("called with db_path: " + db_path + "");
    }

    void pEpSQLite::create_or_open_db()
    {
        pEpLogClass("called");
        int rc{ ::sqlite3_open(db_path.c_str(), &db) };

        if (rc) {
            runtime_error e{ "pEpSQLite: create_or_open_db(\"" + db_path +
                             "\") - failed with sqlite3 error: " + std::to_string(rc) + " - " +
                             ::sqlite3_errmsg(db) };
            close_db();
            throw(e);
        }
    }

    string pEpSQLite::get_db_path() const
    {
        pEpLogClass("called");
        return db_path;
    }

    void pEpSQLite::close_db()
    {
        pEpLogClass("called");
        if (db != nullptr) {
            ::sqlite3_close(db);
            db = nullptr;
        }
    }

    bool pEpSQLite::is_open() const
    {
        if (db == nullptr) {
            return false;
        } else {
            return true;
        }
    }


    void pEpSQLite::delete_db()
    {
        pEpLogClass("called");
        close_db();
        int status = remove(db_path.c_str());
        if (status) {
            runtime_error e{ "pEpSQLite: delete_db(\"" + db_path + "\"): failed with error: " +
                             std::to_string(status) + " - " + strerror(errno) };
            throw(e);
        }
    }

    int pEpSQLite::callback(void *obj, int argc, char **argv, char **azColName)
    {
        RSRecord record;
        for (int col = 0; col < argc; col++) {
            const string key = string{ azColName[col] };
            // TODO: NULL is not correct, could be a valid value
            const string val = string{ argv[col] ? argv[col] : "NULL" };
            record.insert({ key, val });
        }
        (static_cast<pEpSQLite *>(obj))->resultset.push_back(record);
        return 0;
    }

    ResultSet pEpSQLite::execute(const string &stmt)
    {
        if (!is_open()) {
            DBNotOpenException e{ "pEpSQLite: execute() failed - db is not open:" };
            throw(e);
        } else {
            pEpLogClass("called");
            this->resultset.clear();
            char *zErrMsg = nullptr;
            int rc = ::sqlite3_exec(
                db,
                stmt.c_str(),
                (int (*)(void *, int, char **, char **)) & callback,
                this,
                &zErrMsg);
            if (rc != SQLITE_OK) {
                if (rc == SQLITE_CONSTRAINT) {
                    ConstraintException e{ "pEpSQLite: execute() failed with sqlite error: " +
                                           std::to_string(rc) + " - " + string(zErrMsg) };
                    ::sqlite3_free(zErrMsg);
                    throw(e);
                }
                runtime_error e{ "pEpSQLite: execute() failed with sqlite error: " +
                                 std::to_string(rc) + " - " + string(zErrMsg) };
                ::sqlite3_free(zErrMsg);
                throw(e);
            }
        }
        return resultset;
    }

    // Utils
    string pEpSQLite::to_string(const RSRecord &rec)
    {
        stringstream ss;
        for (const auto &col : rec) {
            ss << "[\"" << col.first << "\"] = \"" << col.second << "\"" << endl;
        }
        return ss.str();
    }

    string pEpSQLite::to_string(const ResultSet &rs)
    {
        stringstream ss;
        ss << "ROWCOUNT: " << rs.size() << endl;
        int i = 0;
        for (const RSRecord &rec : rs) {
            ss << "ROW[" << i << "]" << endl << to_string(rec);
            i++;
        }
        return ss.str();
    }

    //Helper
    int pEpSQLite::eval_sql_count(const ResultSet& rs, const string& countfieldname)
    {
        int rescount = 0;
        // Get row
        RSRecord rec{};
        if (rs.size() != 1) {
            runtime_error e{ "ListManagerDummy: eval_sql_count() - row count != 1" };
            throw_with_nested(e);
        }
        try {
            rec = rs.at(0);
        } catch (...) {
            runtime_error e{ "ListManagerDummy: eval_sql_count() - cant get row nr 0" };
            throw_with_nested(e);
        }
        // Get field
        try {
            rescount = stoi(rec.at(countfieldname));
        } catch (...) {
            runtime_error e{ "ListManagerDummy: eval_sql_count() - field not existing" };
            throw_with_nested(e);
        }
        return rescount;
    }

    pEpSQLite::~pEpSQLite()
    {
        pEpLogClass("called");
        close_db();
    }
} // namespace pEp
