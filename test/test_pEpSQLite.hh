// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef LIBPEPADAPTER_TEST_PEPSQLITE_HH
#define LIBPEPADAPTER_TEST_PEPSQLITE_HH

#include <string>
#include "../src/pEpSQLite.hh"

// ATTENTION ATTENTION ATTENTION ATTENTION ATTENTION ATTENTION ATTENTION
//
// THIS IS TERRIBLE, JUST TERRIBLE, THIS IS A ANTI-PATTERN
// Thats the single most stupid way of writing unit test ever!!!
// DONT EVER DO THIS!
//
// The idea is as follows:
// * Tests start simple and become increasingly complex
// * There are fixtures and tests, as usual
// * You start with basic fixtures to write the first test
// * A test becomes a fixture, if the operation is needed for another test
// * You systematically cover all the states possible in which a test can be executed
//
// THATS JUST TERRIBLE!!!
// I WILL NEVER DO THAT AGAIN!
//
// ATTENTION ATTENTION ATTENTION ATTENTION ATTENTION ATTENTION ATTENTION

namespace pEp {
    namespace Test {
        // FIXTURES
        // --------
        // filenames
        std::string fixture_db_filename_new();
        std::string fixture_db_filename_existing_and_verified();
        std::string fixture_db_filename_bad();
        std::string fixture_db_filename_corrupt();
        // prepared db's
        std::string fixture_init_db_new();
        std::string fixture_init_db_existing_and_verified();
        std::string fixture_init_db_corrupt();
        // instance
        pEpSQLite fixture_instance_of_new();
        pEpSQLite fixture_instance_of_existing_and_verified();
        pEpSQLite fixture_instance_of_bad();
        pEpSQLite fixture_instance_of_corrupt();
        // open
        pEpSQLite fixture_db_open_of_new();
        pEpSQLite fixture_db_open_of_existing_and_verified();
        pEpSQLite fixture_db_open_of_bad();
        pEpSQLite fixture_db_open_of_corrupt();
        pEpSQLite fixture_db_open_after_close();
        // tables
        pEpSQLite fixture_db_open_with_tables_of_new();
        // content
        pEpSQLite fixture_db_open_with_tables_and_content();
        // delete
        pEpSQLite fixture_db_open_after_delete();
        pEpSQLite fixture_db_open_after_close_after_delete();


        // TESTS
        // -----
        // instance creation
        pEpSQLite test_create_instance_on_new();          // OK
        pEpSQLite test_create_instance_on_existing();     // OK
        pEpSQLite test_create_instance_on_path_bad();     // OK
        pEpSQLite test_create_instance_on_path_corrupt(); // OK
        // db create_open (create_or_open())
        pEpSQLite test_createopen_db_new();      // OK, new db
        pEpSQLite test_createopen_db_existing(); // OK, open db
        pEpSQLite test_createopen_db_bad();      // ERR, cant create
        pEpSQLite test_createopen_db_corrupt();  // OK (cant detect corruption)
        // close()
        pEpSQLite test_close_before_open(); // OK
        pEpSQLite test_close_after_open();  // OK
        pEpSQLite test_close_idempotent();  // OK
        // create tables (execute())
        pEpSQLite test_db_create_tables_on_open_new();     // OK
        pEpSQLite test_db_create_tables_open_existing();   // ERR, Tables already exist
        pEpSQLite test_db_create_tables_on_open_bad();     // ERR, db closed
        pEpSQLite test_db_create_tables_on_open_corrupt(); // ERR, db corrupt
        // insert (execute())
        pEpSQLite test_db_insert_on_tables_exist();      // OK
        pEpSQLite test_db_insert_on_tables_dont_exist(); // ERR, Tables missing
        pEpSQLite test_db_insert_before_open();          // ERR, Tables missing
        pEpSQLite test_db_insert_after_close();          // ERR, Tables missing
        // verify contents (execute())
        pEpSQLite test_db_verify_content_existing_open_db();             // OK
        pEpSQLite test_db_verify_content_after_insert_on_tables_exist(); // OK
        pEpSQLite test_db_verify_content_no_tables();                    // ERR - no tables
        pEpSQLite test_db_verify_content_after_create_tables();          // ERR - err no data
        // get_path()
        pEpSQLite test_get_path_on_instance_good(); // OK
        pEpSQLite test_get_path_on_instance_bad();  // OK
        // delete_db()
        pEpSQLite test_delete_file_gone_before_open_new();      // OK?
        pEpSQLite test_delete_file_gone_before_open_existing(); // OK
        pEpSQLite test_delete_file_gone_after_close_new();      // OK
        pEpSQLite test_delete_file_gone_after_open_existing();  // OK
        pEpSQLite test_delete_file_gone_after_open_corrupt();   // OK
        pEpSQLite test_delete_file_gone_after_open_bad();       // ERR
        // is_open()
        pEpSQLite test_is_open_before_open_new();        // false
        pEpSQLite test_is_open_after_open_new();         // true
        pEpSQLite test_is_open_after_open_existing();    // true
        pEpSQLite test_is_open_after_open_bad();         // false
        pEpSQLite test_is_open_after_open_corrupt();     // true (cant detect corruption)
        pEpSQLite test_is_open_after_close();            // false
        pEpSQLite test_is_open_after_delete_on_open();   // false
        pEpSQLite test_is_open_after_delete_on_closed(); // false

    } // namespace Test
} // end of namespace pEp

#endif // LIBPEPADAPTER_TEST_PEPSQLITE_HH
