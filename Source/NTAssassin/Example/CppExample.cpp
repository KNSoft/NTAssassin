#include <stdio.h>
#include "NTAssassin\NTAssassin.h"
#include <sddl.h>

#pragma comment (lib, "NTAssassin.lib")

int main1() {
    

    PCWSTR psz = GDIP_SaveImageFilter;

    int iRet;

    // Get active session
    DWORD dwSessionID;
    if (!NT_GetActiveSession(&dwSessionID)) {
        return WIE_GetLastError();
    }

    // Duplicate csrss token and acquire privileges
    SE_PRIVILEGE PrivilegesNeeded[] = {
        SE_CREATE_TOKEN_PRIVILEGE,
        SE_ASSIGNPRIMARYTOKEN_PRIVILEGE,
        SE_INCREASE_QUOTA_PRIVILEGE,
        SE_TCB_PRIVILEGE
    };
    HANDLE hCsrssToken = NT_DuplicateSystemToken(NT_GetCsrPid(), TokenImpersonation, PrivilegesNeeded, ARRAYSIZE(PrivilegesNeeded));
    if (!hCsrssToken) {
        return WIE_GetLastStatus();
    }

    // Impersonate and do work
    if (NT_Impersonate(hCsrssToken)) {

        HANDLE hSuperToken = NULL, hRefToken = NULL;

        hRefToken = NT_GetSessionToken(dwSessionID);
        if (hRefToken) {
            PTOKEN_GROUPS RefGroups = (PTOKEN_GROUPS)NT_GetTokenInfo(hRefToken, TokenLogonSid);
            PTOKEN_PRIVILEGES Privileges = NT_InitTokenPrivileges(WELL_KNOWN_PRIVILEGES, WELL_KNOWN_PRIVILEGES_SIZE, SE_PRIVILEGE_ENABLED_BY_DEFAULT, NULL);
            typedef DEFINE_ANYSIZE_STRUCT(TOKEN_GROUPS_6, TOKEN_GROUPS, SID_AND_ATTRIBUTES, 6);
            TOKEN_GROUPS_6 Groups = {
                { 6, { RefGroups->Groups[0].Sid, SE_GROUP_ENABLED | SE_GROUP_ENABLED_BY_DEFAULT | SE_GROUP_MANDATORY | SE_GROUP_LOGON_ID } }, {
                    { PSID_ADMINISTRATORS, SE_GROUP_ENABLED | SE_GROUP_MANDATORY },
                    { PSID_EVERYONE, SE_GROUP_ENABLED | SE_GROUP_MANDATORY },
                    { PSID_TRUSTED_INSTALLER, SE_GROUP_ENABLED | SE_GROUP_MANDATORY },
                    { PSID_SYSTEM, SE_GROUP_ENABLED | SE_GROUP_OWNER },
                    { PSID_MANDATORY_SYSTEM, SE_GROUP_INTEGRITY_ENABLED | SE_GROUP_INTEGRITY }
                }
            };

            typedef DEFINE_ANYSIZE_STRUCT(SID_5, SID, DWORD, 5);
            SID_5 SidGuest = { SID_REVISION, 5, SECURITY_NT_AUTHORITY, { SECURITY_NT_NON_UNIQUE }, 2889329001, 1953759263, 2315344793, 501 };
            hSuperToken = NT_CreateToken(TokenPrimary, hRefToken, &SidGuest.Base, NULL, &Groups.Base, Privileges);
            if (hSuperToken) {
                NtSetInformationToken(hSuperToken, TokenSessionId, &dwSessionID, sizeof(dwSessionID));
                if (RProc_Create(hSuperToken, L"C:\\Windows\\System32\\cmd.exe", NULL, FALSE, NULL, NULL)) {
                    iRet = ERROR_SUCCESS;
                } else {
                    iRet = WIE_GetLastError();
                }
            } else {
                iRet = WIE_GetLastStatus();
            }

            if (RefGroups) {
                Mem_Free(RefGroups);
            }
            if (Privileges) {
                Mem_Free(Privileges);
            }
        } else {
            iRet = WIE_GetLastError();
        }

        NT_Impersonate(NULL);
    } else {
        iRet = WIE_GetLastStatus();
    }

    NtClose(hCsrssToken);

    return iRet;
}
