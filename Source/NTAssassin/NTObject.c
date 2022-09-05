#pragma once

#include "include\NTAssassin\NTADef.h"

typedef DEFINE_ANYSIZE_STRUCT(SID_2, SID, DWORD, 2);
typedef DEFINE_ANYSIZE_STRUCT(SID_6, SID, DWORD, 6);

SID SID_EVERYONE = { SID_REVISION, 1, SECURITY_WORLD_SID_AUTHORITY, { SECURITY_WORLD_RID } };
PSID PSID_EVERYONE = &SID_EVERYONE;
SID SID_SYSTEM = { SID_REVISION, 1, SECURITY_NT_AUTHORITY, { SECURITY_LOCAL_SYSTEM_RID } };
PSID PSID_SYSTEM = &SID_SYSTEM;
SID_2 SID_ADMINISTRATORS = { { SID_REVISION, 2, SECURITY_NT_AUTHORITY, { SECURITY_BUILTIN_DOMAIN_RID } }, DOMAIN_ALIAS_RID_ADMINS };
PSID PSID_ADMINISTRATORS = &SID_ADMINISTRATORS.Base;
SID_2 SID_USERS = { { SID_REVISION, 2, SECURITY_NT_AUTHORITY, { SECURITY_BUILTIN_DOMAIN_RID } }, DOMAIN_ALIAS_RID_USERS };
PSID PSID_USERS = &SID_USERS.Base;
SID SID_AUTHENTICATED_USER = { SID_REVISION, 1, SECURITY_NT_AUTHORITY, { SECURITY_AUTHENTICATED_USER_RID } };
PSID PSID_AUTHENTICATED_USER = &SID_AUTHENTICATED_USER;
SID SID_LOCAL_SERVICE = { SID_REVISION, 1, SECURITY_NT_AUTHORITY, { SECURITY_LOCAL_SERVICE_RID } };
PSID PSID_LOCAL_SERVICE = &SID_LOCAL_SERVICE;
SID SID_NETWORK_SERVICE = { SID_REVISION, 1, SECURITY_NT_AUTHORITY, { SECURITY_NETWORK_SERVICE_RID } };
PSID PSID_NETWORK_SERVICE = &SID_NETWORK_SERVICE;
SID_6 SID_TRUSTED_INSTALLER = { SID_REVISION, SECURITY_SERVICE_ID_RID_COUNT, SECURITY_NT_AUTHORITY, { SECURITY_SERVICE_ID_BASE_RID }, SECURITY_TRUSTED_INSTALLER_RID1, SECURITY_TRUSTED_INSTALLER_RID2, SECURITY_TRUSTED_INSTALLER_RID3, SECURITY_TRUSTED_INSTALLER_RID4, SECURITY_TRUSTED_INSTALLER_RID5 };
PSID PSID_TRUSTED_INSTALLER = &SID_TRUSTED_INSTALLER.Base;

SID SID_LOCAL_ACCOUNT = { SID_REVISION, 1, SECURITY_NT_AUTHORITY, { SECURITY_LOCAL_ACCOUNT_RID } };
PSID PSID_LOCAL_ACCOUNT = &SID_LOCAL_ACCOUNT;
SID SID_LOCAL_ACCOUNT_AND_ADMIN = { SID_REVISION, 1, SECURITY_NT_AUTHORITY, { SECURITY_LOCAL_ACCOUNT_AND_ADMIN_RID } };
PSID PSID_LOCAL_ACCOUNT_AND_ADMIN = &SID_LOCAL_ACCOUNT_AND_ADMIN;
SID SID_LOCAL = { SID_REVISION, 1, SECURITY_LOCAL_SID_AUTHORITY, { SECURITY_LOCAL_RID } };
PSID PSID_LOCAL = &SID_LOCAL;
SID SID_LOCAL_LOGON = { SID_REVISION, 1, SECURITY_LOCAL_SID_AUTHORITY, { SECURITY_LOCAL_LOGON_RID } };
PSID PSID_LOCAL_LOGON = &SID_LOCAL_LOGON;
SID SID_INTERACTIVE = { SID_REVISION, 1, SECURITY_NT_AUTHORITY, { SECURITY_INTERACTIVE_RID } };
PSID PSID_INTERACTIVE = &SID_INTERACTIVE;
SID_2 SID_NTLM = { {SID_REVISION, SECURITY_PACKAGE_RID_COUNT, SECURITY_NT_AUTHORITY, { SECURITY_PACKAGE_BASE_RID }}, SECURITY_PACKAGE_NTLM_RID };
PSID PSID_NTLM = &SID_NTLM.Base;
SID SID_THIS_ORGANIZATION = { SID_REVISION, 1, SECURITY_NT_AUTHORITY, { SECURITY_THIS_ORGANIZATION_RID } };
PSID PSID_THIS_ORGANIZATION = &SID_THIS_ORGANIZATION;

SID SID_MANDATORY_UNTRUSTED = { SID_REVISION, 1, SECURITY_MANDATORY_LABEL_AUTHORITY, { SECURITY_MANDATORY_UNTRUSTED_RID } };
PSID PSID_MANDATORY_UNTRUSTED = &SID_MANDATORY_UNTRUSTED;
SID SID_MANDATORY_LOW = { SID_REVISION, 1, SECURITY_MANDATORY_LABEL_AUTHORITY, { SECURITY_MANDATORY_LOW_RID } };
PSID PSID_MANDATORY_LOW = &SID_MANDATORY_LOW;
SID SID_MANDATORY_MEDIUM = { SID_REVISION, 1, SECURITY_MANDATORY_LABEL_AUTHORITY, { SECURITY_MANDATORY_MEDIUM_RID } };
PSID PSID_MANDATORY_MEDIUM = &SID_MANDATORY_MEDIUM;
SID SID_MANDATORY_MEDIUM_PLUS = { SID_REVISION, 1, SECURITY_MANDATORY_LABEL_AUTHORITY, { SECURITY_MANDATORY_MEDIUM_PLUS_RID } };
PSID PSID_MANDATORY_MEDIUM_PLUS = &SID_MANDATORY_MEDIUM_PLUS;
SID SID_MANDATORY_HIGH = { SID_REVISION, 1, SECURITY_MANDATORY_LABEL_AUTHORITY, { SECURITY_MANDATORY_HIGH_RID } };
PSID PSID_MANDATORY_HIGH = &SID_MANDATORY_HIGH;
SID SID_MANDATORY_SYSTEM = { SID_REVISION, 1, SECURITY_MANDATORY_LABEL_AUTHORITY, { SECURITY_MANDATORY_SYSTEM_RID } };
PSID PSID_MANDATORY_SYSTEM = &SID_MANDATORY_SYSTEM;
SID SID_MANDATORY_PROTECTED_PROCESS = { SID_REVISION, 1, SECURITY_MANDATORY_LABEL_AUTHORITY, { SECURITY_MANDATORY_PROTECTED_PROCESS_RID } };
PSID PSID_MANDATORY_PROTECTED_PROCESS = &SID_MANDATORY_PROTECTED_PROCESS;

static SE_PRIVILEGE WellKnownPrivileges[] = {
    SE_CREATE_TOKEN_PRIVILEGE,
    SE_ASSIGNPRIMARYTOKEN_PRIVILEGE,
    SE_LOCK_MEMORY_PRIVILEGE,
    SE_INCREASE_QUOTA_PRIVILEGE,
    SE_MACHINE_ACCOUNT_PRIVILEGE,
    SE_TCB_PRIVILEGE,
    SE_SECURITY_PRIVILEGE,
    SE_TAKE_OWNERSHIP_PRIVILEGE,
    SE_LOAD_DRIVER_PRIVILEGE,
    SE_SYSTEM_PROFILE_PRIVILEGE,
    SE_SYSTEMTIME_PRIVILEGE,
    SE_PROF_SINGLE_PROCESS_PRIVILEGE,
    SE_INC_BASE_PRIORITY_PRIVILEGE,
    SE_CREATE_PAGEFILE_PRIVILEGE,
    SE_CREATE_PERMANENT_PRIVILEGE,
    SE_BACKUP_PRIVILEGE,
    SE_RESTORE_PRIVILEGE,
    SE_SHUTDOWN_PRIVILEGE,
    SE_DEBUG_PRIVILEGE,
    SE_AUDIT_PRIVILEGE,
    SE_SYSTEM_ENVIRONMENT_PRIVILEGE,
    SE_CHANGE_NOTIFY_PRIVILEGE,
    SE_REMOTE_SHUTDOWN_PRIVILEGE,
    SE_UNDOCK_PRIVILEGE,
    SE_SYNC_AGENT_PRIVILEGE,
    SE_ENABLE_DELEGATION_PRIVILEGE,
    SE_MANAGE_VOLUME_PRIVILEGE,
    SE_IMPERSONATE_PRIVILEGE,
    SE_CREATE_GLOBAL_PRIVILEGE,
    SE_TRUSTED_CREDMAN_ACCESS_PRIVILEGE,
    SE_RELABEL_PRIVILEGE,
    SE_INC_WORKING_SET_PRIVILEGE,
    SE_TIME_ZONE_PRIVILEGE,
    SE_CREATE_SYMBOLIC_LINK_PRIVILEGE,
    SE_DELEGATE_SESSION_USER_IMPERSONATE_PRIVILEGE
};

PSE_PRIVILEGE WELL_KNOWN_PRIVILEGES = WellKnownPrivileges;
DWORD WELL_KNOWN_PRIVILEGES_SIZE = ARRAYSIZE(WellKnownPrivileges);
