// This file is under GNU General Public License 3.0
// see LICENSE.txt

#include "utils.hh"

#include <iostream>
#include <iostream>
#include <fstream>

#include <pEp/identity_list.h>

#include "Adapter.hh"
#include "adapter_group.h"

using namespace std;
using namespace pEp;

namespace pEp {
    namespace Utils {

        string to_string(const ::pEp_identity *const ident, bool full, int indent)
        {
            stringstream builder;
            if (ident != nullptr) {
                if (full) {
                    builder << endl;
                    builder << std::string(indent, '\t') << "{" << endl;
                    indent++;
                    builder << std::string(indent, '\t')
                            << "address: " << (ident->address != nullptr ? ident->address : "NULL")
                            << endl;
                    builder << std::string(indent, '\t')
                            << "user_id: " << (ident->user_id != nullptr ? ident->user_id : "NULL")
                            << endl;
                    builder << std::string(indent, '\t') << "username: "
                            << (ident->username != nullptr ? ident->username : "NULL") << endl;
                    builder << std::string(indent, '\t')
                            << "fpr: " << (ident->fpr != nullptr ? ident->fpr : "NULL") << endl;
                    builder << std::string(indent, '\t') << "comm_type: " << ident->comm_type << endl;
                    builder << std::string(indent, '\t')
                            << "lang: " << static_cast<string>(ident->lang) << endl;
                    builder << std::string(indent, '\t') << "me: " << ident->me << endl;
                    builder << std::string(indent, '\t') << "major_ver: " << ident->major_ver << endl;
                    builder << std::string(indent, '\t') << "minor_ver: " << ident->minor_ver << endl;
                    builder << std::string(indent, '\t') << "enc_format: " << ident->enc_format
                            << endl;
                    builder << std::string(indent, '\t') << "flags: " << ident->flags << endl;
                    indent--;
                    builder << std::string(indent, '\t') << "}";
                } else {
                    builder << "{ " << (ident->address != nullptr ? ident->address : "NULL") << "/"
                            << (ident->user_id != nullptr ? ident->user_id : "NULL") << "/"
                            << (ident->username != nullptr ? ident->username : "NULL") << "/"
                            << (ident->fpr != nullptr ? ident->fpr : "NULL") << " }";
                }
            } else {
                builder << "NULL";
            }

            return builder.str();
        }

        std::string to_string(const ::identity_list *const idl, bool full, int indent)
        {
            stringstream builder;
            if (idl != nullptr) {
                builder << endl;
                builder << std::string(indent, '\t') << "[" << endl;
                indent++;
                for (const ::identity_list *curr = idl; curr != nullptr; curr = curr->next) {
                    builder << to_string(curr->ident, full, indent) << endl;
                }
                indent--;
                builder << std::string(indent, '\t') << "]";
            } else {
                builder << "NULL";
            }

            return builder.str();
        }

        string to_string(const ::pEp_member *const member, bool full, int indent)
        {
            stringstream builder;
            if (member != nullptr) {
                builder << std::string(indent, '\t') << "{" << endl;
                indent++;
                builder << std::string(indent, '\t')
                        << "ident: " << to_string(member->ident, full, indent) << endl;
                builder << std::string(indent, '\t') << "joined: " << member->joined << endl;
                indent--;
                builder << std::string(indent, '\t') << "}";
            } else {
                builder << "NULL";
            }

            return builder.str();
        }

        string to_string(const ::member_list *const mbl, bool full, int indent)
        {
            stringstream builder;
            if (mbl != nullptr) {
                builder << endl;
                builder << std::string(indent, '\t') << "[" << endl;
                indent++;
                for (const member_list *curr_member = mbl; curr_member != nullptr;
                     curr_member = curr_member->next) {
                    builder << to_string(curr_member->member, full, indent) << endl;
                }
                indent--;
                builder << std::string(indent, '\t') << "]";
            } else {
                builder << "NULL";
            }

            return builder.str();
        }

        string to_string(const ::pEp_group *const group, bool full, int indent)
        {
            stringstream builder;
            if (group != nullptr) {
                builder << endl;
                builder << std::string(indent, '\t') << "{" << endl;
                indent++;
                builder << std::string(indent, '\t')
                        << "group_identity: " << to_string(group->group_identity, full, indent)
                        << endl;
                builder << std::string(indent, '\t')
                        << "manager: " << to_string(group->manager, full, indent) << endl;
                builder << std::string(indent, '\t') << "active: " << group->active << endl;
                builder << std::string(indent, '\t')
                        << "members: " << to_string(group->members, full, indent) << endl;
                indent--;
                builder << std::string(indent, '\t') << "]";
            } else {
                builder << "NULL";
            }

            return builder.str();
        }

        string nested_exception_to_string(const exception &e, int level, string src)
        {
            src += string(level, ' ') + "exception: " + e.what() + "\n";
            try {
                rethrow_if_nested(e);
            } catch (const exception &e) {
                src = nested_exception_to_string(e, level + 1, src);
            } catch (...) {
            }
            return src;
        }


        //            void print_exception(const exception &e, int level)
        //            {
        //                cerr << string(level, ' ') << "exception: " << e.what() << endl;
        //                try {
        //                    rethrow_if_nested(e);
        //                } catch (const exception &e) {
        //                    print_exception(e, level + 1);
        //                } catch (...) {
        //                }
        //            }


        // File utils
        ofstream file_create(const string &filename)
        {
            ofstream outfile{ filename };
            return outfile;
        }

        bool file_exists(const string &filename)
        {

            ifstream ifile(filename.c_str());
            return (bool)ifile;
        }

        void file_delete(const string &filename)
        {
            int status = remove(filename.c_str());
            if (status) {
                runtime_error e{ string("file_delete(\"" + filename + "\") -  " + strerror(errno)) };
                throw(e);
            }
        }

        void file_ensure_not_existing(const string &path)
        {
            while (file_exists(path)) {
                file_delete(path);
            }
        }

    } // namespace Utils
} // namespace pEp
