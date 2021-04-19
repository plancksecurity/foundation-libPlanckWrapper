// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef LIBPEPADAPTER_LISTMANAGER_DUMMY_HH
#define LIBPEPADAPTER_LISTMANAGER_DUMMY_HH

#include "pEpSQLite.hh"
#include "pEpLog.hh"
#include <vector>
#include <string>

namespace pEp {
    class ListManagerDummy {
    public:
        ListManagerDummy(const std::string& db_path);
        // Update
        void list_add(const std::string& addr_list, const std::string& addr_mgr);
        void list_delete(const std::string& addr_list);
        void member_add(const std::string& addr_list, const std::string& addr_member);
        void member_remove(const std::string& addr_list, const std::string& addr_member);
        // Query
        std::vector<std::string> lists();
        std::string moderator(std::string list_address);
        std::vector<std::string> members(std::string list_address);
        // db
        void close_db();
        void delete_db();
        // Logging
        static bool log_enabled;
        Adapter::pEpLog::pEpLogger logger{"ListManagerDummy", log_enabled};
        ~ListManagerDummy();
    private:
        pEpSQLite db;
        bool is_db_initialized {false};
        void ensure_db_initialized();
        void db_config();
        void create_tables();
        Adapter::pEpLog::pEpLogger& m4gic_logger_n4ame = logger;
    };
} // namespace pEp

#endif // LIBPEPADAPTER_LISTMANAGER_DUMMY_HH
