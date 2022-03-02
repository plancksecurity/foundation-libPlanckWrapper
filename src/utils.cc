// This file is under GNU General Public License 3.0
// see LICENSE.txt

#include "utils.hh"
#include <pEp/identity_list.h>
#include <iostream>

using namespace std;
using namespace pEp;

namespace pEp {
    namespace Utils {
        std::vector<::pEp_identity *> to_cxx(const ::identity_list &idl)
        {
            vector<pEp_identity *> ret{};
            for (const ::identity_list *curr = &idl; curr != nullptr; curr = curr->next) {
                if (curr->ident) {
                    ret.push_back(curr->ident);
                }
            }
            return ret;
        }

        string to_string(const ::pEp_identity * ident, bool full, int indent)
        {
            stringstream builder;
            if (ident != nullptr) {
                if (full) {
                    builder << endl;
                    builder << std::string(indent, '\t') << "{" << endl;
                    indent++;
                    builder << std::string(indent, '\t') << "address: '"
                            << (ident->address != nullptr ? ident->address : "NULL") << "'" << endl;
                    builder << std::string(indent, '\t') << "user_id: '"
                            << (ident->user_id != nullptr ? ident->user_id : "NULL") << "'" << endl;
                    builder << std::string(indent, '\t') << "username: '"
                            << (ident->username != nullptr ? ident->username : "NULL") << "'"
                            << endl;
                    builder << std::string(indent, '\t') << "fpr: '"
                            << (ident->fpr != nullptr ? ident->fpr : "NULL") << "'" << endl;
                    builder << std::string(indent, '\t') << "comm_type: " << ident->comm_type << endl;
                    builder << std::string(indent, '\t') << "lang: '"
                            << static_cast<string>(ident->lang) << "'" << endl;
                    builder << std::string(indent, '\t') << "me: " << ident->me << endl;
                    builder << std::string(indent, '\t') << "major_ver: " << ident->major_ver << endl;
                    builder << std::string(indent, '\t') << "minor_ver: " << ident->minor_ver << endl;
                    builder << std::string(indent, '\t') << "enc_format: " << ident->enc_format
                            << endl;
                    builder << std::string(indent, '\t') << "flags: " << ident->flags << endl;
                    indent--;
                    builder << std::string(indent, '\t') << "}";
                } else {
                    builder << std::string(indent, '\t') << "{ '"
                            << (ident->address != nullptr ? ident->address : "NULL") << "' / '"
                            << (ident->user_id != nullptr ? ident->user_id : "NULL") << "' / '"
                            << (ident->username != nullptr ? ident->username : "NULL") << "' / '"
                            << (ident->fpr != nullptr ? ident->fpr : "NULL") << "' }";
                }
            } else {
                builder << "NULL";
            }

            return builder.str();
        }

        std::string to_string(const ::bloblist_t * blob, bool full, int indent)
        {
            stringstream builder;
            if (blob != nullptr) {
                builder << endl;
                builder << std::string(indent, '\t') << "[" << endl;
                indent++;
                for (const ::bloblist_t *curr = blob; curr != nullptr; curr = curr->next) {
                    if (full) {
                        builder << std::string(indent, '\t') << "{" << endl;
                        indent++;
                        builder << std::string(indent, '\t') << "mime_type: '"
                                << (curr->mime_type != nullptr ? std::string(curr->mime_type) : "NULL")
                                << "'" << endl;
                        builder << std::string(indent, '\t') << "filename: '"
                                << (curr->filename != nullptr ? curr->filename : "NULL") << "'"
                                << endl;
                        builder << std::string(indent, '\t') << "size: " << curr->size << endl;
                        builder << std::string(indent, '\t') << "value: '"
                                << (curr->value != nullptr
                                        ? Utils::tldr(std::string(curr->value), 300)
                                        : "NULL")
                                << "'" << endl;
                        indent--;
                        builder << std::string(indent, '\t') << "}" << endl;
                    } else {
                        builder << std::string(indent, '\t');
                        builder << "{ '"
                                << (curr->mime_type != nullptr ? std::string(curr->mime_type)
                                                               : "NULL");
                        builder << "' / '" << (curr->filename != nullptr ? curr->filename : "NULL");
                        builder << "' / '" << curr->size << "'";
                        builder << " }" << endl;
                    }
                }
                indent--;
                builder << std::string(indent, '\t') << "]" << endl;
            } else {
                builder << "NULL";
            }

            return builder.str();
        }

        std::string to_string(const ::stringpair_list_t * spl, bool full, int indent)
        {
            stringstream builder;
            if (spl != nullptr) {
                builder << endl;
                builder << std::string(indent, '\t') << "[" << endl;
                indent++;
                for (const ::stringpair_list_t *curr = spl; curr != nullptr; curr = curr->next) {
                    builder << std::string(indent, '\t') << "{ '";
                    if (curr->value != nullptr) {
                        builder << (curr->value->key ? curr->value->key : "NULL");
                        builder << "' : '";
                        builder << (curr->value->value ? curr->value->value : "NULL");
                    }
                    builder << "' }" << endl;
                }
                indent--;
                builder << std::string(indent, '\t') << "]" << endl;
            } else {
                builder << "NULL";
            }

            return builder.str();
        }

        std::string to_string(const ::message * msg, bool full, int indent)
        {
            stringstream builder;
            if (msg != nullptr) {
                builder << endl;
                builder << std::string(indent, '\t') << "{" << endl;
                indent++;
                builder << std::string(indent, '\t') << "from: "
                        << (msg->from != nullptr ? to_string(msg->from, full, indent) : "NULL")
                        << endl;
                builder << std::string(indent, '\t') << "to: "
                        << (msg->to != nullptr ? to_string(msg->to, full, indent) : "NULL") << endl;
                builder << std::string(indent, '\t') << "shortmsg: '"
                        << (msg->shortmsg != nullptr ? msg->shortmsg : "NULL") << "'" << endl;
                builder << std::string(indent, '\t') << "longmsg: '"
                        << (msg->longmsg != nullptr ? msg->longmsg : "NULL") << "'" << endl;
                builder << std::string(indent, '\t') << "enc_format: " << msg->enc_format << endl;
                builder << std::string(indent, '\t')
                        << "dir: " << (msg->dir == 0 ? "incomming" : "outgoing") << endl;
                builder << std::string(indent, '\t') << "id: '"
                        << (msg->id != nullptr ? msg->id : "NULL") << "'" << endl;
                builder << std::string(indent, '\t') << "opt_fields: "
                        << (msg->opt_fields ? to_string(msg->opt_fields, full, indent) : "NULL")
                        << endl;
                builder << std::string(indent, '\t') << "attachments: "
                        << (msg->attachments ? to_string(msg->attachments, full, indent) : "NULL")
                        << endl;
                indent--;
                builder << std::string(indent, '\t') << "}" << endl;
            } else {
                builder << "NULL";
            }

            return builder.str();
        }

        std::string to_string(const ::identity_list * idl, bool full, int indent)
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

        string to_string(const ::pEp_member * member, bool full, int indent)
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

        string to_string(const ::member_list * mbl, bool full, int indent)
        {
            stringstream builder;
            if (mbl != nullptr) {
                builder << endl;
                builder << std::string(indent, '\t') << "[" << endl;
                indent++;
                for (const member_list *curr = mbl; curr != nullptr; curr = curr->next) {
                    builder << to_string(curr->member, full, indent) << endl;
                }
                indent--;
                builder << std::string(indent, '\t') << "]";
            } else {
                builder << "NULL";
            }

            return builder.str();
        }

        string to_string(const ::pEp_group * group, bool full, int indent)
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


        // TODO: Move to std_utils
        std::string readKey()
        {
            std::string ret;
            std::cin >> ret;
            return ret;
        }
    } // namespace Utils
} // namespace pEp

std::ostream& operator<<(std::ostream& o, const ::pEp_identity* pEp_c_dt)
{
    return o << pEp::Utils::to_string(pEp_c_dt);
}
std::ostream& operator<<(std::ostream& o, const ::bloblist_t* pEp_c_dt)
{
    return o << pEp::Utils::to_string(pEp_c_dt);
}
std::ostream& operator<<(std::ostream& o, const ::stringpair_list_t* pEp_c_dt)
{
    return o << pEp::Utils::to_string(pEp_c_dt);
}
std::ostream& operator<<(std::ostream& o, const ::message* pEp_c_dt)
{
    return o << pEp::Utils::to_string(pEp_c_dt);
}
std::ostream& operator<<(std::ostream& o, const ::identity_list* pEp_c_dt)
{
    return o << pEp::Utils::to_string(pEp_c_dt);
}
std::ostream& operator<<(std::ostream& o, const ::pEp_member* pEp_c_dt)
{
    return o << pEp::Utils::to_string(pEp_c_dt);
}
std::ostream& operator<<(std::ostream& o, const ::member_list* pEp_c_dt)
{
    return o << pEp::Utils::to_string(pEp_c_dt);
}
std::ostream& operator<<(std::ostream& o, const ::pEp_group* pEp_c_dt)
{
    return o << pEp::Utils::to_string(pEp_c_dt);
}