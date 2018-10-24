#include "status_to_string.hh"
#include <sstream>


namespace pEp
{

// in pEpEngine.h positive values are hex, negative are decimal. :-o
// TODO: the code should be generated!
std::string status_to_string(PEP_STATUS status)
{
    const char* status_string = "PEP_STATUS_UNKNOWN";
    switch(status)
    {
        case PEP_STATUS_OK                        : status_string = "PEP_STATUS_OK"; break;

        case PEP_INIT_CANNOT_LOAD_GPGME           : status_string = "PEP_INIT_CANNOT_LOAD_GPGME"; break;
        case PEP_INIT_GPGME_INIT_FAILED           : status_string = "PEP_INIT_GPGME_INIT_FAILED"; break;
        case PEP_INIT_NO_GPG_HOME                 : status_string = "PEP_INIT_NO_GPG_HOME"; break;
        case PEP_INIT_NETPGP_INIT_FAILED          : status_string = "PEP_INIT_NETPGP_INIT_FAILED"; break;
        case PEP_INIT_CANNOT_DETERMINE_GPG_VERSION: status_string = "PEP_INIT_CANNOT_DETERMINE_GPG_VERSION"; break;
        case PEP_INIT_UNSUPPORTED_GPG_VERSION     : status_string = "PEP_INIT_UNSUPPORTED_GPG_VERSION"; break;
        case PEP_INIT_CANNOT_CONFIG_GPG_AGENT     : status_string = "PEP_INIT_CANNOT_CONFIG_GPG_AGENT"; break;
        
        case PEP_INIT_SQLITE3_WITHOUT_MUTEX       : status_string = "PEP_INIT_SQLITE3_WITHOUT_MUTEX"; break;
        case PEP_INIT_CANNOT_OPEN_DB              : status_string = "PEP_INIT_CANNOT_OPEN_DB"; break;
        case PEP_INIT_CANNOT_OPEN_SYSTEM_DB       : status_string = "PEP_INIT_CANNOT_OPEN_SYSTEM_DB"; break;
        case PEP_UNKNOWN_DB_ERROR                 : status_string = "PEP_UNKNOWN_DB_ERROR"; break;
        
        case PEP_KEY_NOT_FOUND                    : status_string = "PEP_KEY_NOT_FOUND"; break;
        case PEP_KEY_HAS_AMBIG_NAME               : status_string = "PEP_KEY_HAS_AMBIG_NAME"; break;
        case PEP_GET_KEY_FAILED                   : status_string = "PEP_GET_KEY_FAILED"; break;
        case PEP_CANNOT_EXPORT_KEY                : status_string = "PEP_CANNOT_EXPORT_KEY"; break;
        case PEP_CANNOT_EDIT_KEY                  : status_string = "PEP_CANNOT_EDIT_KEY"; break;
        case PEP_KEY_UNSUITABLE                   : status_string = "PEP_KEY_UNSUITABLE"; break;
        case PEP_MALFORMED_KEY_RESET_MSG          : status_string = "PEP_MALFORMED_KEY_RESET_MSG"; break;
        case PEP_KEY_NOT_RESET                    : status_string = "PEP_KEY_NOT_RESET"; break;
        
        case PEP_CANNOT_FIND_IDENTITY             : status_string = "PEP_CANNOT_FIND_IDENTITY"; break;
        case PEP_CANNOT_SET_PERSON                : status_string = "PEP_CANNOT_SET_PERSON"; break;
        case PEP_CANNOT_SET_PGP_KEYPAIR           : status_string = "PEP_CANNOT_SET_PGP_KEYPAIR"; break;
        case PEP_CANNOT_SET_IDENTITY              : status_string = "PEP_CANNOT_SET_IDENTITY"; break;
        case PEP_CANNOT_SET_TRUST                 : status_string = "PEP_CANNOT_SET_TRUST"; break;
        case PEP_KEY_BLACKLISTED                  : status_string = "PEP_KEY_BLACKLISTED"; break;
        case PEP_CANNOT_FIND_PERSON               : status_string = "PEP_CANNOT_FIND_PERSON"; break;
        
        case PEP_CANNOT_FIND_ALIAS                : status_string = "PEP_CANNOT_FIND_ALIAS"; break;
        case PEP_CANNOT_SET_ALIAS                 : status_string = "PEP_CANNOT_SET_ALIAS"; break;
        
        case PEP_UNENCRYPTED                      : status_string = "PEP_UNENCRYPTED"; break;
        case PEP_VERIFIED                         : status_string = "PEP_VERIFIED"; break;
        case PEP_DECRYPTED                        : status_string = "PEP_DECRYPTED"; break;
        case PEP_DECRYPTED_AND_VERIFIED           : status_string = "PEP_DECRYPTED_AND_VERIFIED"; break;
        case PEP_DECRYPT_WRONG_FORMAT             : status_string = "PEP_DECRYPT_WRONG_FORMAT"; break;
        case PEP_DECRYPT_NO_KEY                   : status_string = "PEP_DECRYPT_NO_KEY"; break;
        case PEP_DECRYPT_SIGNATURE_DOES_NOT_MATCH : status_string = "PEP_DECRYPT_SIGNATURE_DOES_NOT_MATCH"; break;
        case PEP_VERIFY_NO_KEY                    : status_string = "PEP_VERIFY_NO_KEY"; break;
        case PEP_VERIFIED_AND_TRUSTED             : status_string = "PEP_VERIFIED_AND_TRUSTED"; break;
        case PEP_CANNOT_REENCRYPT                 : status_string = "PEP_CANNOT_REENCRYPT"; break;
        case PEP_CANNOT_DECRYPT_UNKNOWN           : status_string = "PEP_CANNOT_DECRYPT_UNKNOWN"; break;
        
        case PEP_TRUSTWORD_NOT_FOUND              : status_string = "PEP_TRUSTWORD_NOT_FOUND"; break;
        case PEP_TRUSTWORDS_FPR_WRONG_LENGTH      : status_string = "PEP_TRUSTWORDS_FPR_WRONG_LENGTH"; break;
        case PEP_TRUSTWORDS_DUPLICATE_FPR         : status_string = "PEP_TRUSTWORDS_DUPLICATE_FPR"; break;
        
        case PEP_CANNOT_CREATE_KEY                : status_string = "PEP_CANNOT_CREATE_KEY"; break;
        case PEP_CANNOT_SEND_KEY                  : status_string = "PEP_CANNOT_SEND_KEY"; break;
        
        case PEP_PHRASE_NOT_FOUND                 : status_string = "PEP_PHRASE_NOT_FOUND"; break;
        
        case PEP_SEND_FUNCTION_NOT_REGISTERED     : status_string = "PEP_SEND_FUNCTION_NOT_REGISTERED"; break;
        case PEP_CONTRAINTS_VIOLATED              : status_string = "PEP_CONTRAINTS_VIOLATED"; break;
        case PEP_CANNOT_ENCODE                    : status_string = "PEP_CANNOT_ENCODE"; break;
        
        case PEP_SYNC_NO_NOTIFY_CALLBACK          : status_string = "PEP_SYNC_NO_NOTIFY_CALLBACK"; break;
        case PEP_SYNC_ILLEGAL_MESSAGE             : status_string = "PEP_SYNC_ILLEGAL_MESSAGE"; break;
        case PEP_SYNC_NO_INJECT_CALLBACK          : status_string = "PEP_SYNC_NO_INJECT_CALLBACK"; break;
        case PEP_SYNC_NO_CHANNEL                  : status_string = "PEP_SYNC_NO_CHANNEL"; break;
        case PEP_SYNC_CANNOT_ENCRYPT              : status_string = "PEP_SYNC_CANNOT_ENCRYPT"; break;
        case PEP_SYNC_NO_MESSAGE_SEND_CALLBACK    : status_string = "PEP_SYNC_NO_MESSAGE_SEND_CALLBACK"; break;
        
        case PEP_CANNOT_INCREASE_SEQUENCE         : status_string = "PEP_CANNOT_INCREASE_SEQUENCE"; break;
        
        case PEP_STATEMACHINE_ERROR               : status_string = "PEP_STATEMACHINE_ERROR"; break;
        case PEP_NO_TRUST                         : status_string = "PEP_NO_TRUST"; break;
        case PEP_STATEMACHINE_INVALID_STATE       : status_string = "PEP_STATEMACHINE_INVALID_STATE"; break;
        case PEP_STATEMACHINE_INVALID_EVENT       : status_string = "PEP_STATEMACHINE_INVALID_EVENT"; break;
        case PEP_STATEMACHINE_INVALID_CONDITION   : status_string = "PEP_STATEMACHINE_INVALID_CONDITION"; break;
        case PEP_STATEMACHINE_INVALID_ACTION      : status_string = "PEP_STATEMACHINE_INVALID_ACTION"; break;
        case PEP_STATEMACHINE_INHIBITED_EVENT     : status_string = "PEP_STATEMACHINE_INHIBITED_EVENT"; break;
        case PEP_STATEMACHINE_CANNOT_SEND         : status_string = "PEP_STATEMACHINE_CANNOT_SEND"; break;
        
        case PEP_COMMIT_FAILED                    : status_string = "PEP_COMMIT_FAILED"; break;
        case PEP_MESSAGE_CONSUME                  : status_string = "PEP_MESSAGE_CONSUME"; break;
        case PEP_MESSAGE_IGNORE                   : status_string = "PEP_MESSAGE_IGNORE"; break;
        
        case PEP_RECORD_NOT_FOUND                 : status_string = "PEP_RECORD_NOT_FOUND"; break;
        case PEP_CANNOT_CREATE_TEMP_FILE          : status_string = "PEP_CANNOT_CREATE_TEMP_FILE"; break;
        case PEP_ILLEGAL_VALUE                    : status_string = "PEP_ILLEGAL_VALUE"; break;
        case PEP_BUFFER_TOO_SMALL                 : status_string = "PEP_BUFFER_TOO_SMALL"; break;
        case PEP_OUT_OF_MEMORY                    : status_string = "PEP_OUT_OF_MEMORY"; break;
        case PEP_UNKNOWN_ERROR                    : status_string = "PEP_UNKNOWN_ERROR"; break;
        
        case PEP_VERSION_MISMATCH                 : status_string = "PEP_VERSION_MISMATCH"; break;
    }
    
    std::stringstream ss;
    if(status>0)
    {
        ss << "0x" << std::hex << status;
    }else{
        ss << status;
    }
    return ss.str() + " \"" + status_string + '"';
}
    
} // end of namespace pEp
