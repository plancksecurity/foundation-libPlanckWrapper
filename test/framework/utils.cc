// This file is under GNU General Public License 3.0
// see LICENSE.txt

#include "utils.hh"

#include <pEp/keymanagement.h>

#include "../../src/Adapter.hh"
#include "../../src/adapter_group.h"

using namespace std;
using namespace pEp;

namespace pEp {
    namespace Test {
        namespace Utils {
            string identity_to_string(::pEp_identity* ident, bool full, int indent)
            {

                stringstream builder;
                if (ident != nullptr) {
                    if (full) {
                        builder << endl;
                        builder << std::string(indent, '\t') << "{" << endl;
                        indent++;
                        builder << std::string(indent, '\t') << "address: "
                                << (ident->address != nullptr ? ident->address : "NULL") << endl;
                        builder << std::string(indent, '\t') << "user_id: "
                                << (ident->user_id != nullptr ? ident->user_id : "NULL") << endl;
                        builder << std::string(indent, '\t') << "username: "
                                << (ident->username != nullptr ? ident->username : "NULL") << endl;
                        builder << std::string(indent, '\t')
                                << "fpr: " << (ident->fpr != nullptr ? ident->fpr : "NULL") << endl;
                        builder << std::string(indent, '\t') << "comm_type: " << ident->comm_type
                                << endl;
                        builder << std::string(indent, '\t')
                                << "lang: " << static_cast<string>(ident->lang) << endl;
                        builder << std::string(indent, '\t') << "me: " << ident->me << endl;
                        builder << std::string(indent, '\t') << "major_ver: " << ident->major_ver
                                << endl;
                        builder << std::string(indent, '\t') << "minor_ver: " << ident->minor_ver
                                << endl;
                        builder << std::string(indent, '\t') << "enc_format: " << ident->enc_format
                                << endl;
                        builder << std::string(indent, '\t') << "flags: " << ident->flags << endl;
                        indent--;
                        builder << std::string(indent, '\t') << "}";
                    } else {
                        builder << "{ " << (ident->address != nullptr ? ident->address : "NULL")
                                << "/" << (ident->user_id != nullptr ? ident->user_id : "NULL")
                                << "/" << (ident->username != nullptr ? ident->username : "NULL")
                                << "/" << (ident->fpr != nullptr ? ident->fpr : "NULL") << " }";
                    }
                } else {
                    builder << "NULL";
                }

                return builder.str();
            }


            string member_to_string(::pEp_member* member, bool full, int indent)
            {
                stringstream builder;
                if (member != nullptr) {
                    builder << std::string(indent, '\t') << "{" << endl;
                    indent++;
                    builder << std::string(indent, '\t')
                            << "ident: " << identity_to_string(member->ident, full, indent) << endl;
                    builder << std::string(indent, '\t') << "joined: " << member->joined << endl;
                    indent--;
                    builder << std::string(indent, '\t') << "}";
                } else {
                    builder << "NULL";
                }

                return builder.str();
            }

            string memberlist_to_string(::member_list* mbl, bool full, int indent)
            {
                stringstream builder;
                if (mbl != nullptr) {
                    builder << endl;
                    builder << std::string(indent, '\t') << "[" << endl;
                    indent++;
                    for (member_list* curr_member = mbl; curr_member != nullptr;
                         curr_member = curr_member->next) {
                        builder << member_to_string(curr_member->member, full, indent) << endl;
                    }
                    indent--;
                    builder << std::string(indent, '\t') << "]";
                } else {
                    builder << "NULL";
                }

                return builder.str();
            }

            string group_to_string(::pEp_group* group, bool full, int indent)
            {
                stringstream builder;
                if (group != nullptr) {
                    builder << endl;
                    builder << std::string(indent, '\t') << "{" << endl;
                    indent++;
                    builder << std::string(indent, '\t') << "group_identity: "
                            << identity_to_string(group->group_identity, full, indent) << endl;
                    builder << std::string(indent, '\t')
                            << "manager: " << identity_to_string(group->manager, full, indent)
                            << endl;
                    builder << std::string(indent, '\t') << "active: " << group->active << endl;
                    builder << std::string(indent, '\t')
                            << "members: " << memberlist_to_string(group->members, full, indent)
                            << endl;
                    indent--;
                    builder << std::string(indent, '\t') << "]";
                } else {
                    builder << "NULL";
                }

                return builder.str();
            }
        } // namespace Utils
    }     // namespace Test
} // namespace pEp
