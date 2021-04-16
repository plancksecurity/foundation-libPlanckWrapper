// This file is under GNU General Public License 3.0
// see LICENSE.txt
#ifndef LIBPEPADAPTER_PEPSQLITE_HH
#define LIBPEPADAPTER_PEPSQLITE_HH

#include "sqlite3.h"
#include "pEpLog.hh"
#include <iostream>
#include <vector>
#include <map>


using ResultSet = std::vector<std::map<std::string, std::string>>;
using RSRecord = std::map<std::string, std::string>;

namespace pEp {
    class pEpSQLite {
    public:
        pEpSQLite() = delete;

        // The database file as a constant for the obj lifetime
        pEpSQLite(std::string db_path);
        std::string get_db_path();

        // Creates the database file not existsing
        // Will not create any dirs
        void create_or_open_db();
        void close_db();

        // Delete the database file
        void delete_db();
        ResultSet execute(const std::string& stmt);
        std::string resultset_to_string(const ResultSet& rs);

        // logging
        static bool log_enabled;
        Adapter::pEpLog::pEpLogger logger{"pEpSQLite", log_enabled};
        Adapter::pEpLog::pEpLogger& m4gic_logger_n4ame = logger;
        ~pEpSQLite();
    private:
        ::sqlite3 *db = nullptr;
        std::string db_path;
        ResultSet resultset;

        static int callback(void *obj, int argc, char **argv, char **azColName);
    };
} // namespace pEp

#endif // LIBPEPADAPTER_PEPSQLITE_HH
