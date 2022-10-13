// This file is under GNU General Public License 3.0
// see LICENSE.txt
#ifndef LIBPEPADAPTER_PEPSQLITE_HH
#define LIBPEPADAPTER_PEPSQLITE_HH

// Include guard for "some" sqlite.h which use _SQLITE3_H_ as include guard (e.g. System SDK sqlite3.h on macOS)
#ifndef _SQLITE3_H_
    #include "internal/sqlite3.h"
#endif
#include <pEp/pEpLog.hh>
#include <iostream>
#include <vector>
#include <map>


using ResultSet = std::vector<std::map<std::string, std::string>>;
using RSRecord = std::map<std::string, std::string>;

namespace pEp {
    class pEpSQLite {
    public:
        // The database file as a constant for the obj lifetime
        explicit pEpSQLite(const std::string& db_path);

        pEpSQLite() = delete;
//        TODO:
//        pEpSQLite(const pEpSQLite&) = delete;
//        pEpSQLite(const pEpSQLite&&) = delete;
//        pEpSQLite& operator=(const pEpSQLite&) = delete;
//        pEpSQLite& operator=(const pEpSQLite&&) = delete;

        std::string get_db_path() const;

        // Creates the database file not existsing
        // Will not create any dirs
        void create_or_open_db();
        void close_db();
        bool is_open() const;

        // Delete the database file
        void delete_db();
        ResultSet execute(const std::string& stmt);

        // Utils
        static std::string to_string(const RSRecord& rec);
        static std::string to_string(const ResultSet& rs);
        static int eval_sql_count(const ResultSet& rs, const std::string& countfieldname);

        // Logging
        static bool log_enabled;
        Adapter::pEpLog::pEpLogger logger{ "pEpSQLite", log_enabled };
        ~pEpSQLite();

    private:
        ::sqlite3* db = nullptr;
        std::string db_path;
        ResultSet resultset;
        Adapter::pEpLog::pEpLogger& m4gic_logger_n4me = logger;
        static int callback(void* obj, int argc, char** argv, char** azColName);
    };

    class DBNotOpenException : public std::runtime_error {
    public:
        DBNotOpenException(const std::string& string) : runtime_error(string) {}
    };

    class ConstraintException : public std::runtime_error {
    public:
        ConstraintException(const std::string& string) : runtime_error(string) {}
    };
} // namespace pEp

#endif // LIBPEPADAPTER_PEPSQLITE_HH
