#include "../src/sqlite3.h"
#include "../src/pEpLog.hh"
#include <iostream>
#include <cstdio>
#include <exception>
#include <stdexcept>
#include <vector>
#include <map>

//TODO: add const
using namespace std;
using namespace pEp;

using ResultSet = vector<map<string, string>>;
using RSRecord = map<string, string>;

namespace pEp {
    namespace SQLite3 {
        ::sqlite3 *db;
        string db_path;

        ResultSet resultset;

        void create_or_open_db()
        {
            pEpLog("called");
            int rc{::sqlite3_open(db_path.c_str(), &db)};

            if (rc) {
                runtime_error e{string("Can't open database (" + db_path + "):" + ::sqlite3_errmsg(db))};
                throw (e);
            }
        }

        void close_db()
        {
            ::sqlite3_close(db);
        }

        void delete_db()
        {
            pEpLog("called");
            remove(db_path.c_str());
            if (errno) {
                cerr << "could not delete db (" + db_path + "): " << strerror(errno) << endl;
            }
        }

        ResultSet execute(const string& stmt)
        {
            //    pEpLog("execute(\"" + stmt + "\")");
            resultset.clear();
            char *zErrMsg = nullptr;
            int rc = ::sqlite3_exec(db, stmt.c_str(), [](void *NotUsed, int argc, char **argv, char **azColName) -> int {
                RSRecord record;
                for (int col = 0; col < argc; col++) {
                    const string key = string{azColName[col]};
                    // TODO: NULL is not correct, could be a valid value
                    const string val = string{argv[col] ? argv[col] : "NULL"};
                    record.insert({key, val});
                }
                resultset.push_back(record);
                return 0;
            }, 0, &zErrMsg);
            if (rc != SQLITE_OK) {
                runtime_error e{string("execute: " + string(::sqlite3_errmsg(db)) + ":" + string(zErrMsg))};
                ::sqlite3_free(zErrMsg);
                throw (e);
            }
            return resultset;
        }

        string resultset_to_string(ResultSet rs)
        {
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
    }
}


void print_exception(const exception& e, int level = 0)
{
    cerr << string(level, ' ') << "exception: " << e.what() << endl;
    try {
        rethrow_if_nested(e);
    } catch (const exception& e) {
        print_exception(e, level + 1);
    } catch (...) {
    }
}

void db_config()
{
    pEpLog("called");
    try {
        string sql;
        sql = "PRAGMA foreign_keys=ON";
        SQLite3::execute(sql);
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
              "address          TEXT     NOT NULL,"
              "moderator_address  TEXT     NOT NULL,"
              "PRIMARY KEY(address));";
        SQLite3::execute(sql);

        sql = "CREATE TABLE IF NOT EXISTS member_of("
              "address      TEXT     NOT NULL,"
              "list_address TEXT     NOT NULL,"
              "PRIMARY KEY (address, list_address),"
              "FOREIGN KEY(list_address) REFERENCES lists(address) ON DELETE CASCADE);";
        SQLite3::execute(sql);
    } catch (...) {
        runtime_error e("create_tables() - failed with exception");
        throw_with_nested(e);
    }
}

void list_add(const string& addr_list, const string& addr_mgr)
{
    pEpLog("list_add(addr_list: \"" + addr_list + "\", addr_mgr: \"" + addr_mgr + "\")");
    try {
        string sql = "INSERT INTO lists(address, moderator_address) VALUES ('" + addr_list + "','" +
                     addr_mgr + "');";
        SQLite3::execute(sql);
    } catch (...) {
        runtime_error e(
                "list_add(addr_list: \"" + addr_list + "\"\taddr_mgr: \"" + addr_mgr +
                "\") - failed with exception");
        throw_with_nested(e);
    }
}

void list_delete(const string& addr_list)
{
    pEpLog("list_delete(addr_list: \"" + addr_list + "\")");
    try {
        string sql;
        sql = "DELETE FROM lists WHERE lists.address = '" + addr_list + "';";
        SQLite3::execute(sql);
    } catch (...) {
        runtime_error e("list_delete(addr_list: \"" + addr_list + "\") - failed with exception");
        throw_with_nested(e);
    }
}

void member_add(const string& addr_list, const string& addr_member)
{
    pEpLog("member_add(addr_list: \"" + addr_list + "\", addr_member: \"" + addr_member + "\")");
    try {
        string sql = "INSERT INTO member_of(address, list_address) VALUES ('" + addr_member +
                     "', '" + addr_list + "');";
        SQLite3::execute(sql);
    } catch (...) {
        runtime_error e(
                "member_add(addr_list: \"" + addr_list + "\", addr_member: \"" + addr_member +
                "\") - failed with exception");
        throw_with_nested(e);
    }
}

void member_remove(const string& addr_list, const string& addr_member)
{
    pEpLog("member_remove(addr_list: \"" + addr_list + "\", addr_member: '\"" + addr_member + "\")");
    try {
        string sql;
        sql = "DELETE FROM member_of WHERE (member_of.address = '" + addr_member +
              "') AND (member_of.list_address = '" + addr_list + "');";
        SQLite3::execute(sql);
    } catch (...) {
        runtime_error e(
                "member_remove(" + addr_list + ", " + addr_member + ") - failed with exception");
        throw_with_nested(e);
    }
}

vector<string> lists()
{
    pEpLog("called");
    vector<string> ret;
    ResultSet rs;

    try {
        string sql;
        sql = "SELECT address FROM lists";
        rs = SQLite3::execute(sql);
    } catch (...) {
        runtime_error e("lists() -  failed with exception");
        throw_with_nested(e);
    }

    for (const RSRecord& rec : rs) {
        ret.push_back(rec.at("address"));
    }

    return ret;
}

// Exceptions:
// * ListNotFound
string moderator(string list_address)
{
    pEpLog("called");
    string ret;
    ResultSet rs;

    try {
        string sql;
        sql = "SELECT moderator_address FROM lists "
              "WHERE lists.address = '" + list_address + "';";
        rs = SQLite3::execute(sql);
    } catch (...) {
        runtime_error e("lists() -  failed with exception");
        throw_with_nested(e);
    }

    if (!rs.empty()) {
        for (const RSRecord& rec : rs) {
            ret = rec.at("moderator_address");
        }
    }

    return ret;
}

// Exceptions:
// * ListNotFound
vector<string> members(string list_address)
{
    pEpLog("called");
    vector<string> ret;
    ResultSet rs;

    try {
        string sql;
        sql = "SELECT address FROM member_of "
              "WHERE list_address = '" + list_address + "'";
        rs = SQLite3::execute(sql);
    } catch (...) {
        runtime_error e("lists() -  failed with exception");
        throw_with_nested(e);
    }

    if (!rs.empty()) {
        for (const RSRecord& rec : rs) {
            ret.push_back(rec.at("address"));
        }
    }

    return ret;
}

template<typename T>
string vector_to_string(vector<T> v)
{
    stringstream ss;
    for (const T& elem : v) {
        ss << elem << endl;
    }
    return ss.str();
}

int main(int argc, char *argv[])
{
    pEp::Adapter::pEpLog::set_enabled(true);
    SQLite3::db_path = "test.db";
    string dummy_in;


    try {
        SQLite3::db_path = "test.db";
        SQLite3::delete_db();
        SQLite3::create_or_open_db();
        db_config();
        create_tables();

        list_add("grp1@peptest.org", "alice@peptest.org");
        cout << vector_to_string(lists());
        list_add("grp2@peptest.org", "alice@peptest.org");

        cout << vector_to_string(lists());

        try {
            list_add("grp1@peptest.org", "bob@peptest.org");
            assert(false);
        } catch (const exception& e) {
            print_exception(e);
        }

        member_add("grp1@peptest.org", "bob@peptest.org");
        member_add("grp1@peptest.org", "carol@peptest.org");
        member_add("grp1@peptest.org", "joe@peptest.org");

        cout << vector_to_string(members("grp1@peptest.org")) << endl;

        cout << moderator("grp1@peptest.org") << endl;
        member_remove("grp1@peptest.org", "joe@peptest.org");

        list_delete("grp1@peptest.org");
        cout << vector_to_string(lists());
    } catch (const exception& e) {
        print_exception(e);
    }
    SQLite3::close_db();
}