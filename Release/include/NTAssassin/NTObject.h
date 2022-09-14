﻿#pragma once

#include "NTStruct.h"

// Well-known SIDs
EXTERN_C PSID PSID_EVERYONE;
EXTERN_C PSID PSID_SYSTEM;
EXTERN_C PSID PSID_ADMINISTRATORS;
EXTERN_C PSID PSID_USERS;
EXTERN_C PSID PSID_AUTHENTICATED_USER;
EXTERN_C PSID PSID_LOCAL_SERVICE;
EXTERN_C PSID PSID_NETWORK_SERVICE;
EXTERN_C PSID PSID_TRUSTED_INSTALLER;

EXTERN_C PSID PSID_LOCAL_ACCOUNT;
EXTERN_C PSID PSID_LOCAL_ACCOUNT_AND_ADMIN;
EXTERN_C PSID PSID_LOCAL;
EXTERN_C PSID PSID_LOCAL_LOGON;
EXTERN_C PSID PSID_INTERACTIVE;
EXTERN_C PSID PSID_NTLM;
EXTERN_C PSID PSID_THIS_ORGANIZATION;

EXTERN_C PSID PSID_MANDATORY_UNTRUSTED;
EXTERN_C PSID PSID_MANDATORY_LOW;
EXTERN_C PSID PSID_MANDATORY_MEDIUM;
EXTERN_C PSID PSID_MANDATORY_MEDIUM_PLUS;
EXTERN_C PSID PSID_MANDATORY_HIGH;
EXTERN_C PSID PSID_MANDATORY_SYSTEM;
EXTERN_C PSID PSID_MANDATORY_PROTECTED_PROCESS;

// Well-known privileges
EXTERN_C PSE_PRIVILEGE WELL_KNOWN_PRIVILEGES;
EXTERN_C DWORD WELL_KNOWN_PRIVILEGES_SIZE;
