﻿#pragma once

#include "NTADef.h"

// Well-known SIDs
EXTERN_C NTA_API PSID PSID_EVERYONE;
EXTERN_C NTA_API PSID PSID_SYSTEM;
EXTERN_C NTA_API PSID PSID_ADMINISTRATORS;
EXTERN_C NTA_API PSID PSID_USERS;
EXTERN_C NTA_API PSID PSID_AUTHENTICATED_USER;
EXTERN_C NTA_API PSID PSID_LOCAL_SERVICE;
EXTERN_C NTA_API PSID PSID_NETWORK_SERVICE;
EXTERN_C NTA_API PSID PSID_TRUSTED_INSTALLER;

EXTERN_C NTA_API PSID PSID_LOCAL_ACCOUNT;
EXTERN_C NTA_API PSID PSID_LOCAL_ACCOUNT_AND_ADMIN;
EXTERN_C NTA_API PSID PSID_LOCAL;
EXTERN_C NTA_API PSID PSID_LOCAL_LOGON;
EXTERN_C NTA_API PSID PSID_INTERACTIVE;
EXTERN_C NTA_API PSID PSID_NTLM;
EXTERN_C NTA_API PSID PSID_THIS_ORGANIZATION;

EXTERN_C NTA_API PSID PSID_MANDATORY_UNTRUSTED;
EXTERN_C NTA_API PSID PSID_MANDATORY_LOW;
EXTERN_C NTA_API PSID PSID_MANDATORY_MEDIUM;
EXTERN_C NTA_API PSID PSID_MANDATORY_MEDIUM_PLUS;
EXTERN_C NTA_API PSID PSID_MANDATORY_HIGH;
EXTERN_C NTA_API PSID PSID_MANDATORY_SYSTEM;
EXTERN_C NTA_API PSID PSID_MANDATORY_PROTECTED_PROCESS;

// Well-known privileges
EXTERN_C NTA_API PSE_PRIVILEGE WELL_KNOWN_PRIVILEGES;
EXTERN_C NTA_API DWORD WELL_KNOWN_PRIVILEGES_SIZE;
