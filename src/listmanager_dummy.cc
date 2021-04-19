#include "listmanager_dummy.hh"
#include "pEpSQLite.hh"
#include <exception>

using namespace std;

namespace pEp {
    bool ListManagerDummy::log_enabled = false;

    // public
    ListManagerDummy::ListManagerDummy(const string& db_path) : db(pEpSQLite(db_path))
    {
        pEpLogClass("called");
    }

    // private
    void ListManagerDummy::ensure_db_initialized()
    {
        if (!db.is_open()) {
            is_db_initialized = false;
            try {
                db.create_or_open_db();
            } catch (...) {
                db.close_db();
                DBException e{ "ListManagerDummy - error opening db" };
                throw_with_nested(e);
            }
        }
        if (!is_db_initialized) {
            try {
                db_config();
                create_tables();
            } catch (...) {
                db.close_db();
                DBException e{ "ListManagerDummy - db init failed" };
                throw_with_nested(e);
            }
            is_db_initialized = true;
        }
    }

    // private
    void ListManagerDummy::db_config()
    {
        try {
            string sql;
            sql = "PRAGMA foreign_keys=ON";
            db.execute(sql);
        } catch (...) {
            DBException e{ "ListManagerDummy - db config failed" };
            throw_with_nested(e);
        }
    }

    // private
    void ListManagerDummy::create_tables()
    {
        try {
            string sql;

            sql = "CREATE TABLE IF NOT EXISTS lists("
                  "address          TEXT     NOT NULL,"
                  "moderator_address  TEXT     NOT NULL,"
                  "PRIMARY KEY(address));";
            db.execute(sql);

            sql = "CREATE TABLE IF NOT EXISTS member_of("
                  "address      TEXT     NOT NULL,"
                  "list_address TEXT     NOT NULL,"
                  "PRIMARY KEY (address, list_address),"
                  "FOREIGN KEY(list_address) REFERENCES lists(address) ON DELETE CASCADE);";
            db.execute(sql);
        } catch (...) {
            DBException e("ListManagerDummy - create tables failed");
            throw_with_nested(e);
        }
    }

    // public
    void ListManagerDummy::close_db()
    {
        pEpLogClass("called");
        db.close_db();
    }

    // public
    void ListManagerDummy::delete_db()
    {
        pEpLogClass("called");
        try {
            db.delete_db();
        } catch (...) {
            DBException e{ "ListManagerDummy: delete_db() failed" };
            throw_with_nested(e);
        }
    }

    // public
    void ListManagerDummy::list_add(const std::string& addr_list, const std::string& addr_mgr)
    {
        pEpLogClass("list_add(addr_list: \"" + addr_list + "\", addr_mgr: \"" + addr_mgr + "\")");
        if (list_exists(addr_list)) {
            AlreadyExistsException e{ "list_add(addr_list: \"" + addr_list + "\", addr_mgr: \"" +
                                      addr_mgr + "\") - List already exists" };
            throw e;
        }

        try {
            string sql = "INSERT INTO lists(address, moderator_address) VALUES ('" + addr_list +
                         "','" + addr_mgr + "');";

            db.execute(sql);
        } catch (...) {
            DBException e{ "ListManagerDummy: list_add(addr_list: \"" + addr_list +
                           "\"\taddr_mgr: \"" + addr_mgr + "\") - failed with exception" };
            throw_with_nested(e);
        }
    }

    // public
    void ListManagerDummy::list_delete(const std::string& addr_list)
    {
        pEpLogClass("list_delete(addr_list: \"" + addr_list + "\")");
        if (!list_exists(addr_list)) {
            DoesNotExistException e{ "list_delete(addr_list: \"" + addr_list +
                                     "\") - List does not exist" };
            throw e;
        }

        try {
            string sql;
            sql = "DELETE FROM lists WHERE lists.address = '" + addr_list + "';";
            db.execute(sql);
        } catch (...) {
            DBException e{ "ListManagerDummy: list_delete(addr_list: \"" + addr_list +
                           "\") - failed with exception" };
            throw_with_nested(e);
        }
    }

    // public
    void ListManagerDummy::member_add(const std::string& addr_list, const std::string& addr_member)
    {
        pEpLogClass(
            "member_add(addr_list: \"" + addr_list + "\", addr_member: \"" + addr_member + "\")");
        if (member_exists(addr_list, addr_member)) {
            AlreadyExistsException e{ "member_add(addr_list: \"" + addr_list + "\", addr_member: \"" +
                                      addr_member + "\") - member already exists" };
            throw e;
        }

        try {
            string sql = "INSERT INTO member_of(address, list_address) VALUES ('" + addr_member +
                         "', '" + addr_list + "');";
            db.execute(sql);
        } catch (...) {
            DBException e{ "ListManagerDummy: member_add(addr_list: \"" + addr_list +
                           "\", addr_member: \"" + addr_member + "\") - failed with exception" };
            throw_with_nested(e);
        }
    }

    // public
    void ListManagerDummy::member_remove(const std::string& addr_list, const std::string& addr_member)
    {
        pEpLogClass(
            "member_remove(addr_list: \"" + addr_list + "\", addr_member: '\"" + addr_member + "\")");
        if (!member_exists(addr_list, addr_member)) {
            DoesNotExistException e{ "member_remove(addr_list: \"" + addr_list +
                                     "\", addr_member: '\"" + addr_member +
                                     "\") - member does not exist" };
            throw e;
        }

        try {
            string sql;
            sql = "DELETE FROM member_of WHERE (member_of.address = '" + addr_member +
                  "') AND (member_of.list_address = '" + addr_list + "');";
            db.execute(sql);
        } catch (...) {
            DBException e{ "ListManagerDummy: member_remove(" + addr_list + ", " + addr_member +
                           ") - failed with exception" };
            throw_with_nested(e);
        }
    }

    // public
    std::vector<std::string> ListManagerDummy::lists()
    {
        pEpLogClass("called");
        ensure_db_initialized();
        vector<string> ret;
        ResultSet rs;

        try {
            string sql;
            sql = "SELECT address FROM lists";
            rs = db.execute(sql);
        } catch (...) {
            DBException e{ "ListManagerDummy: lists() failed" };
            throw_with_nested(e);
        }

        for (const RSRecord& rec : rs) {
            ret.push_back(rec.at("address"));
        }

        return ret;
    }

    // public
    std::string ListManagerDummy::moderator(const std::string& addr_list)
    {
        pEpLogClass("moderator(list_address:\"" + addr_list + "\")");
        if (!list_exists(addr_list)) {
            DoesNotExistException e{ "moderator(list_address:\"" + addr_list +
                                     "\") - List does not exist" };
            throw e;
        }

        string ret;
        ResultSet rs;

        try {
            string sql;
            sql = "SELECT moderator_address FROM lists "
                  "WHERE lists.address = '" +
                  addr_list + "';";
            rs = db.execute(sql);
        } catch (...) {
            DBException e{ "ListManagerDummy: moderator(list_address:\"" + addr_list +
                           "\") - failed" };
            throw_with_nested(e);
        }

        if (!rs.empty()) {
            for (const RSRecord& rec : rs) {
                ret = rec.at("moderator_address");
            }
        }

        return ret;
    }

    // public
    std::vector<std::string> ListManagerDummy::members(const std::string& addr_list)
    {
        pEpLogClass("members(list_address:\"" + addr_list + "\")");
        if (!list_exists(addr_list)) {
            DoesNotExistException e{ "members(list_address:\"" + addr_list +
                                     "\") - List does not exist" };
            throw e;
        }

        vector<string> ret;
        ResultSet rs;

        try {
            string sql;
            sql = "SELECT address FROM member_of "
                  "WHERE list_address = '" +
                  addr_list + "'";
            rs = db.execute(sql);
        } catch (...) {
            DBException e{ "ListManagerDummy: members(list_address:\"" + addr_list + "\")" };
            throw_with_nested(e);
        }

        if (!rs.empty()) {
            for (const RSRecord& rec : rs) {
                ret.push_back(rec.at("address"));
            }
        }

        return ret;
    }

    bool ListManagerDummy::list_exists(const std::string& addr_list)
    {
        pEpLogClass("list_exists(addr_list:\"" + addr_list + "\")");
        bool ret{ false };
        ensure_db_initialized();

        ResultSet rs;
        int rescount = 0;
        try {
            string sql;
            sql = "SELECT COUNT(address) AS rescount "
                  "FROM lists "
                  "WHERE address = '" +
                  addr_list + "';";
            rs = db.execute(sql);
            rescount = pEpSQLite::eval_sql_count(rs, "rescount");
        } catch (...) {
            DBException e{ "ListManagerDummy: list_exists(addr_list:\"" + addr_list + "\")" };
            throw_with_nested(e);
        }
        // Check FATAL inconsistency
        if (rescount > 1) {
            DBException e{ "ListManagerDummy: list_exists(addr_list:\"" + addr_list +
                           "\") - FATAL DB CONSTRAINT ERROR: list exists more than once" };
            throw_with_nested(e);
        }

        if (rescount == 1) {
            ret = true;
        }
        return ret;
    }

    bool ListManagerDummy::member_exists(const std::string& addr_list, const std::string& addr_member)
    {
        pEpLogClass(
            "member_exists(addr_list:\"" + addr_list + "\", addr_member:\"" + addr_member + "\")");
        bool ret{ false };
        ensure_db_initialized();

        ResultSet rs;
        int rescount = 0;
        try {
            string sql;
            sql = "SELECT COUNT(address) AS rescount "
                  "FROM member_of "
                  "WHERE (address = '" +
                  addr_member + "' AND list_address = '" + addr_list + "');";
            rs = db.execute(sql);
            rescount = pEpSQLite::eval_sql_count(rs, "rescount");
        } catch (...) {
            DBException e{ "member_exists(addr_list:\"" + addr_list + "\", addr_member:\"" +
                           addr_member + "\")" };
            throw_with_nested(e);
        }
        // Check FATAL inconsistency
        if (rescount > 1) {
            DBException e{ "member_exists(addr_list:\"" + addr_list + "\", addr_member:\"" +
                           addr_member +
                           "\") - FATAL DB CONSTRAINT ERROR: list exists more than once" };
            throw_with_nested(e);
        }

        if (rescount == 1) {
            ret = true;
        }
        return ret;
    }


    // public
    ListManagerDummy::~ListManagerDummy()
    {
        pEpLogClass("called");
        db.close_db();
    }
} // namespace pEp