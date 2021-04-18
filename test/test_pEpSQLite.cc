#include "test_pEpSQLite.hh"
#include "../src/pEpSQLite.hh"
#include "../src/pEpLog.hh"
#include "framework/utils.hh"

#include <fstream>

using namespace std;
using namespace pEp;
using namespace pEp::Test;
using namespace pEp::Test::Log;
using namespace pEp::Test::Utils;

namespace pEp {
    namespace Test {
        // --------------------------------- FIXTURES ---------------------------------
        // filenames
        string fixture_db_filename_new()
        {
            //            pEpLog("called");
            string path = "new.db";
            return path;
        }

        string fixture_db_filename_bad()
        {
            //            pEpLog("called");
            string db_path_bad = "/will_not_create_dirs/bad.db";
            return db_path_bad;
        }

        string fixture_db_filename_existing_and_verified()
        {
            //            pEpLog("called");
            string path = "new.db";
            return path;
        }

        string fixture_db_filename_corrupt()
        {
            //            pEpLog("called");
            string path = "corrupt.db";
            return path;
        }

        // prepared db's
        string fixture_init_db_new()
        {
            //            pEpLog("called");
            string path = fixture_db_filename_new();
            //            cout << "fixture: \"" << path << "\" not existing" << endl;
            file_ensure_not_existing(path);
            return path;
        }

        string fixture_init_db_existing_and_verified()
        {
            //            pEpLog("called");
            string path = "existing.db";
            //            cout << "fixture: \"" << path << "\" not existing" << endl;
            file_ensure_not_existing(path);
            return path;
        }

        string fixture_init_db_corrupt()
        {
            //            pEpLog("called");
            //            cout << "creating corrupt db" << endl;
            string path = fixture_db_filename_corrupt();
            file_ensure_not_existing(path);
            ofstream db_corrupt = file_create(path);
            db_corrupt << "G4rbage" << endl;
            db_corrupt.close();
            return path;
        }

        // instance
        pEpSQLite fixture_instance_of_new()
        {
            //            pEpLog("called");
            return test_create_instance_on_new();
        }

        pEpSQLite fixture_instance_of_existing_and_verified()
        {
            //            pEpLog("called");
            return test_create_instance_on_existing();
        }

        pEpSQLite fixture_instance_of_bad()
        {
            //            pEpLog("called");
            return test_create_instance_on_path_bad();
        }

        pEpSQLite fixture_instance_of_corrupt()
        {
            //            pEpLog("called");
            return test_create_instance_on_path_corrupt();
        }

        // open
        pEpSQLite fixture_db_open_of_new()
        {
            //            pEpLog("called");
            return test_createopen_db_new();
        }

        pEpSQLite fixture_db_open_of_existing_and_verified()
        {
            //            pEpLog("called");
            return test_db_verify_content_after_insert_on_tables_exist();
        }

        pEpSQLite fixture_db_open_of_bad()
        {
            //            pEpLog("called");
            return test_createopen_db_bad();
        }

        pEpSQLite fixture_db_open_of_corrupt()
        {
            //            pEpLog("called");
            return test_createopen_db_corrupt();
        }

        pEpSQLite fixture_db_open_after_close()
        {
            //            pEpLog("called");
            return test_close_after_open();
        }

        // tables
        pEpSQLite fixture_db_open_with_tables_of_new()
        {
            //            pEpLog("called");
            return test_db_create_tables_on_open_new();
        }

        pEpSQLite fixture_db_open_with_tables_of_corrupt()
        {
            //            pEpLog("called");
            return test_db_create_tables_on_open_corrupt();
        }

        // content
        pEpSQLite fixture_db_open_with_tables_and_content()
        {
            //            pEpLog("called");
            return test_db_insert_on_tables_exist();
        }

        // delete
        pEpSQLite fixture_db_open_after_delete()
        {
            //            pEpLog("called");
            return test_delete_file_gone_after_open_existing();
        }

        pEpSQLite fixture_db_open_after_close_after_delete()
        {
            //            pEpLog("called");
            return test_delete_file_gone_after_close_new();
        }

        // --------------------------------- TESTS ---------------------------------

        // instance creation
        // OK
        pEpSQLite test_create_instance_on_new()
        {
            pEpLog("called");
            pEpSQLite db(fixture_init_db_new());
            return db;
        }

        // OK
        pEpSQLite test_create_instance_on_existing()
        {
            pEpLog("called");
            pEpSQLite db(fixture_init_db_existing_and_verified());
            return db;
        }

        // OK
        pEpSQLite test_create_instance_on_path_bad()
        {
            pEpLog("called");
            pEpSQLite db(fixture_db_filename_bad());
            return db;
        }

        // OK
        pEpSQLite test_create_instance_on_path_corrupt()
        {
            pEpLog("called");
            pEpSQLite db(fixture_init_db_corrupt());
            return db;
        }

        // db create_open (create_or_open())
        // OK, new db
        pEpSQLite test_createopen_db_new()
        {
            pEpLog("called");
            pEpSQLite db = fixture_instance_of_new();
            assert(!file_exists(fixture_db_filename_new()));
            db.create_or_open_db();
            assert(file_exists(fixture_db_filename_new()));
            return db;
        }

        // OK, open db
        pEpSQLite test_createopen_db_existing()
        {
            pEpLog("called");
            pEpSQLite db = fixture_instance_of_existing_and_verified();
            assert(file_exists(fixture_db_filename_existing_and_verified()));
            db.create_or_open_db();
            return db;
        }

        // ERR, cant create
        pEpSQLite test_createopen_db_bad()
        {
            pEpLog("called");
            assert(!file_exists(fixture_db_filename_bad()));
            pEpSQLite db = fixture_instance_of_bad();
            assert(!file_exists(fixture_db_filename_bad()));
            try {
                db.create_or_open_db();
                assert(false);
            } catch (const exception& e) {
                pEp::Test::Utils::print_exception(e);
            }
            assert(!file_exists(fixture_db_filename_bad()));
            return db;
        }

        // OK(cant detect corruption)
        pEpSQLite test_createopen_db_corrupt()
        {
            pEpLog("called");
            pEpSQLite db = fixture_instance_of_corrupt();
            assert(file_exists(fixture_db_filename_corrupt()));
            db.create_or_open_db();
            assert(file_exists(fixture_db_filename_corrupt()));
            return db;
        }

        // close()
        // OK
        pEpSQLite test_close_before_open()
        {
            pEpLog("called");
            pEpSQLite db = fixture_instance_of_new();
            db.close_db();
            return db;
        }

        // OK
        pEpSQLite test_close_after_open()
        {
            pEpLog("called");
            pEpSQLite db = fixture_db_open_of_new();
            db.close_db();
            return db;
        }

        // OK
        pEpSQLite test_close_idempotent()
        {
            pEpLog("called");
            pEpSQLite db = fixture_db_open_of_new();
            db.close_db();
            db.close_db();
            db.close_db();
            db.close_db();
            return db;
        }

        // create tables (execute())
        // OK
        pEpSQLite test_db_create_tables_on_open_new()
        {
            pEpLog("called");
            pEpSQLite db = fixture_db_open_of_new();
            db.execute("CREATE TABLE test(i,i_squared);");
            return db;
        }

        // ERR, Tables already exist
        pEpSQLite test_db_create_tables_open_existing()
        {
            pEpLog("called");
            pEpSQLite db = fixture_db_open_of_existing_and_verified();
            try {
                db.execute("CREATE TABLE test(i,i_squared);");
                assert(false);
            } catch (const exception& e) {
                pEp::Test::Utils::print_exception(e);
            }
            return db;
        }

        // ERR, db closed
        pEpSQLite test_db_create_tables_on_open_bad()
        {
            pEpLog("called");
            pEpSQLite db = fixture_db_open_of_bad();
            try {
                db.execute("CREATE TABLE test(i,i_squared);");
                assert(false);
            } catch (const exception& e) {
                pEp::Test::Utils::print_exception(e);
            }
            return db;
        }

        // ERR, db corrupt
        pEpSQLite test_db_create_tables_on_open_corrupt()
        {
            pEpLog("called");
            pEpSQLite db = fixture_db_open_of_corrupt();
            try {
                db.execute("CREATE TABLE test(i,i_squared);");
                assert(false);
            } catch (const exception& e) {
                pEp::Test::Utils::print_exception(e);
            }
            return db;
        }


        // insert (execute())
        void insert_operation(pEpSQLite& db)
        {
            pEpLog("called");
            for (int i = 0; i < 9; i = (i + 2)) {
                db.execute(
                    "INSERT INTO test(i,i_squared) VALUES ('" + to_string(i) + "', '" +
                    to_string(i * i) + "');");
            }
        }

        // OK
        pEpSQLite test_db_insert_on_tables_exist()
        {
            pEpLog("called");
            pEpSQLite db = fixture_db_open_with_tables_of_new();
            insert_operation(db);
            return db;
        }

        // ERR, Tables missing
        pEpSQLite test_db_insert_on_tables_dont_exist()
        {
            pEpSQLite db = fixture_db_open_of_new();
            try {
                insert_operation(db);
                assert(false);
            } catch (const exception& e) {
                pEp::Test::Utils::print_exception(e);
            }
            return db;
        }

        // ERR, Tables missing
        pEpSQLite test_db_insert_before_open()
        {
            pEpLog("called");
            pEpSQLite db = fixture_instance_of_new();
            try {
                insert_operation(db);
                assert(false);
            } catch (const exception& e) {
                pEp::Test::Utils::print_exception(e);
            }
            return db;
        }

        // ERR, Tables missing
        pEpSQLite test_db_insert_after_close()
        {
            pEpLog("called");
            pEpSQLite db = fixture_db_open_after_close();
            try {
                insert_operation(db);
                assert(false);
            } catch (const exception& e) {
                pEp::Test::Utils::print_exception(e);
            }
            return db;
        }

        // verify contents (execute())
        void verify_operation(pEpSQLite& db)
        {
            pEpLog("called");
            for (int i = 0; i < 9; i++) {
                ResultSet rs = db.execute(
                    "SELECT i, i_squared FROM test "
                    "WHERE (test.i == '" +
                    to_string(i) + "');");
                //                cout << "RESULT: " << endl << pEpSQLite::to_string(rs) << endl;
                if (i % 2) {
                    if (rs.size() != 0) {
                        runtime_error e{ "Exception verifying database content" };
                        throw(e);
                    }
                } else {
                    if(rs.size() != 1) {
                        runtime_error e{ "Exception verifying database content" };
                        throw(e);
                    }
                    for (const RSRecord& r : rs) {
                        const int x = stoi(r.at("i"));
                        const int x_squared = stoi(r.at("i_squared"));
                        if((x * x) != x_squared) {
                            runtime_error e{ "Exception verifying database content" };
                            throw(e);
                        }
                    }
                }
            }
        }

        // OK
        pEpSQLite test_db_verify_content_existing_open_db()
        {
            pEpLog("called");
            pEpSQLite db = fixture_db_open_of_existing_and_verified();
            verify_operation(db);
            return db;
        }

        // OK
        pEpSQLite test_db_verify_content_after_insert_on_tables_exist()
        {
            pEpLog("called");
            pEpSQLite db = fixture_db_open_with_tables_and_content();
            verify_operation(db);
            return db;
        }

        // ERR - no tables
        pEpSQLite test_db_verify_content_no_tables()
        {
            pEpLog("called");
            pEpSQLite db = fixture_db_open_of_new();
            try {
                verify_operation(db);
                assert(false);
            } catch (const exception& e) {
                pEp::Test::Utils::print_exception(e);
            }
            return db;
        }

        // ERR - err no data
        pEpSQLite test_db_verify_content_after_create_tables()
        {
            pEpLog("called");
            pEpSQLite db = fixture_db_open_with_tables_of_new();
            try {
                verify_operation(db);
                assert(false);
            } catch (const exception& e) {
                pEp::Test::Utils::print_exception(e);
            }
            return db;
        }


        // get_path()
        // OK
        pEpSQLite test_get_path_on_instance_good()
        {
            pEpLog("called");
            pEpSQLite db = fixture_instance_of_new();
            assert(db.get_db_path() == fixture_db_filename_new());
            return db;
        }

        // OK
        pEpSQLite test_get_path_on_instance_bad()
        {
            pEpLog("called");
            pEpSQLite db = fixture_instance_of_bad();
            assert(db.get_db_path() == fixture_db_filename_bad());
            return db;
        }

        // delete_db()
        // ERR, file not found
        pEpSQLite test_delete_file_gone_before_open_new()
        {
            pEpLog("called");
            pEpSQLite db = fixture_instance_of_new();
            try {
                db.delete_db();
                assert(false);
            } catch (const exception& e) {
                pEp::Test::Utils::print_exception(e);
            }
            assert(!file_exists(fixture_db_filename_new()));
            return db;
        }

        // ERR, file not found
        pEpSQLite test_delete_file_gone_before_open_existing()
        {
            pEpLog("called");
            pEpSQLite db = fixture_instance_of_existing_and_verified();
            try {
                db.delete_db();
                assert(false);
            } catch (const exception& e) {
                pEp::Test::Utils::print_exception(e);
            }
            assert(!file_exists(fixture_db_filename_existing_and_verified()));
            return db;
        }

        // OK
        pEpSQLite test_delete_file_gone_after_close_new()
        {
            pEpLog("called");
            pEpSQLite db = fixture_db_open_after_close();
            db.delete_db();
            assert(!file_exists(fixture_db_filename_new()));
            return db;
        }

        // OK
        pEpSQLite test_delete_file_gone_after_open_existing()
        {
            pEpLog("called");
            pEpSQLite db = fixture_db_open_of_existing_and_verified();
            db.delete_db();
            assert(!file_exists(fixture_db_filename_existing_and_verified()));
            return db;
        }

        // OK
        pEpSQLite test_delete_file_gone_after_open_corrupt()
        {
            pEpLog("called");
            pEpSQLite db = fixture_db_open_of_corrupt();
            db.delete_db();
            assert(!file_exists(fixture_db_filename_corrupt()));
            return db;
        }

        // ERR
        pEpSQLite test_delete_file_gone_after_open_bad()
        {
            pEpLog("called");
            pEpSQLite db = fixture_db_open_of_bad();
            try {
                db.delete_db();
                assert(false);
            } catch (const exception& e) {
                pEp::Test::Utils::print_exception(e);
            }
            assert(!file_exists(fixture_db_filename_bad()));
            return db;
        }

        // is_open()
        // false
        pEpSQLite test_is_open_before_open_new()
        {
            pEpLog("called");
            pEpSQLite db = fixture_instance_of_new();
            assert(!db.is_open());
            return db;
        }

        // true
        pEpSQLite test_is_open_after_open_new()
        {
            pEpLog("called");
            pEpSQLite db = fixture_db_open_of_new();
            assert(db.is_open());
            return db;
        }

        // true
        pEpSQLite test_is_open_after_open_existing()
        {
            pEpLog("called");
            pEpSQLite db = fixture_db_open_of_existing_and_verified();
            assert(db.is_open());
            return db;
        }

        // false
        pEpSQLite test_is_open_after_open_bad()
        {
            pEpLog("called");
            pEpSQLite db = fixture_db_open_of_bad();
            assert(!db.is_open());
            return db;
        }

        // true (cant detect corruption)
        pEpSQLite test_is_open_after_open_corrupt()
        {
            pEpLog("called");
            pEpSQLite db = fixture_db_open_of_corrupt();
            assert(db.is_open());
            return db;
        }

        // false
        pEpSQLite test_is_open_after_close()
        {
            pEpLog("called");
            pEpSQLite db = fixture_db_open_after_close();
            assert(!db.is_open());
            return db;
        }

        // false
        pEpSQLite test_is_open_after_delete_on_open()
        {
            pEpLog("called");
            pEpSQLite db = fixture_db_open_after_delete();
            assert(!db.is_open());
            return db;
        }

        // false
        pEpSQLite test_is_open_after_delete_on_closed()
        {
            pEpLog("called");
            pEpSQLite db = fixture_db_open_after_close_after_delete();
            assert(!db.is_open());
            return db;
        }
    } // namespace Test
} // namespace pEp

int main(int argc, char* argv[])
{
    // Enable logging for all instances of pEpSQLite
    pEpSQLite::log_enabled = true;
    Adapter::pEpLog::set_enabled(true);
    // instance creation
    test_create_instance_on_new();
    test_create_instance_on_existing();
    test_create_instance_on_path_bad();
    test_create_instance_on_path_corrupt();
    // db create_open (create_or_open())
    test_createopen_db_new();
    test_createopen_db_existing();
    test_createopen_db_bad();
    test_createopen_db_corrupt();
    // close()
    test_close_before_open();
    test_close_after_open();
    test_close_idempotent();
    // create tables (execute())
    test_db_create_tables_on_open_new();
    test_db_create_tables_open_existing();
    test_db_create_tables_on_open_bad();
    test_db_create_tables_on_open_corrupt();
    // insert (execute())
    test_db_insert_on_tables_exist();
    test_db_insert_on_tables_dont_exist();
    test_db_insert_before_open();
    test_db_insert_after_close();
    // verify contents (execute())
    test_db_verify_content_existing_open_db();
    test_db_verify_content_after_insert_on_tables_exist();
    test_db_verify_content_no_tables();
    test_db_verify_content_after_create_tables();
    // get_path()
    test_get_path_on_instance_good();
    test_get_path_on_instance_bad();
    // delete_db()
    test_delete_file_gone_before_open_new();
    test_delete_file_gone_before_open_existing();
    test_delete_file_gone_after_close_new();
    test_delete_file_gone_after_open_existing();
    test_delete_file_gone_after_open_corrupt();
    test_delete_file_gone_after_open_bad();
    // is_open()
    test_is_open_before_open_new();
    test_is_open_after_open_new();
    test_is_open_after_open_existing();
    test_is_open_after_open_bad();
    test_is_open_after_open_corrupt();
    test_is_open_after_close();
    test_is_open_after_delete_on_open();
    test_is_open_after_delete_on_closed();
    return 0;
}