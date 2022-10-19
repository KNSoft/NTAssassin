#include "include\NTAssassin\NTANT.h"
#include "include\NTAssassin\NTAEH.h"
#include "include\NTAssassin\NTARProc.h"
#include "include\NTAssassin\NTAMem.h"

BOOL NTAPI NT_EqualSid(_In_ PSID Sid1, _In_ PSID Sid2) {
    SIZE_T Length = NT_LengthSid(Sid1);
    return RtlCompareMemory(Sid1, Sid2, Length) == Length;
}

_Success_(NT_SUCCESS(return))
NTSTATUS NTAPI NT_CopySid(_In_ ULONG Size, _Out_ PSID SidDst, _In_ PSID SidSrc) {
    SIZE_T Length = NT_LengthSid(SidSrc);
    if (Size < Length) {
        return STATUS_BUFFER_TOO_SMALL;
    } else {
        RtlMoveMemory(SidDst, SidSrc, Length);
        return STATUS_SUCCESS;
    }
}

DWORD NTAPI NT_GetLsaPid() {
    DWORD Pid = 0;
    HANDLE hKey = NT_RegOpenKey(PRTL_CONSTANT_UNICODE_STRING("\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Control\\Lsa"), KEY_QUERY_VALUE);
    if (hKey) {
        NT_RegGetDword(hKey, PRTL_CONSTANT_UNICODE_STRING("LsaPid"), &Pid);
        NtClose(hKey);
    }
    return Pid;
}

HANDLE NTAPI NT_DuplicateCsrssToken(_In_ TOKEN_TYPE Type, _In_reads_(PrivilegeCount) PSE_PRIVILEGE PrivilegeToEnable, _In_ DWORD PrivilegeCount) {
    HANDLE hDuplicatedToken = NULL;
    HANDLE hProc = RProc_Open(PROCESS_QUERY_LIMITED_INFORMATION, NT_GetCsrPid());
    if (hProc) {
        HANDLE hToken;
        NTSTATUS Status;
        Status = NtOpenProcessToken(hProc, TOKEN_QUERY | TOKEN_DUPLICATE, &hToken);
        if (NT_SUCCESS(Status)) {
            POBJECT_ATTRIBUTES poa;
            if (Type == TokenImpersonation) {
                SECURITY_QUALITY_OF_SERVICE sqos = { sizeof(sqos), DEFAULT_IMPERSONATION_LEVEL, SECURITY_DYNAMIC_TRACKING, FALSE };
                OBJECT_ATTRIBUTES oa = { sizeof(oa), NULL, NULL, 0, NULL, &sqos };
                poa = &oa;
            } else {
                poa = NULL;
            }
            Status = NtDuplicateToken(hToken, TOKEN_ALL_ACCESS, poa, FALSE, Type, &hDuplicatedToken);
            if (NT_SUCCESS(Status)) {
                if (PrivilegeCount) {
                    ULONG PrivLength;
                    PTOKEN_PRIVILEGES Priv = NT_InitTokenPrivileges(PrivilegeToEnable, PrivilegeCount, SE_PRIVILEGE_ENABLED, &PrivLength);
                    if (Priv) {
                        Status = NtAdjustPrivilegesToken(hDuplicatedToken, FALSE, Priv, PrivLength, NULL, NULL);
                        if (Status != STATUS_SUCCESS) {
                            NtClose(hDuplicatedToken);
                            hDuplicatedToken = NULL;
                        }
                        Mem_Free(Priv);
                    } else {
                        Status = EH_GetLastStatus();
                    }
                }
            }
            NtClose(hToken);
        }
        if (!hDuplicatedToken) {
            EH_SetLastStatus(Status);
        }
        NtClose(hProc);
    }
    return hDuplicatedToken;
}

PVOID NTAPI NT_GetTokenInfo(_In_ HANDLE TokenHandle, _In_ TOKEN_INFORMATION_CLASS TokenInformationClass) {
    PVOID Info = NULL;
    ULONG Length;
    NTSTATUS Status;
    Status = NtQueryInformationToken(TokenHandle, TokenInformationClass, NULL, 0, &Length);
    if (Status == STATUS_BUFFER_TOO_SMALL) {
        if (Length) {
            Info = Mem_Alloc(Length);
            if (Info) {
                Status = NtQueryInformationToken(TokenHandle, TokenInformationClass, Info, Length, &Length);
                if (!NT_SUCCESS(Status)) {
                    Mem_Free(Info);
                    Info = NULL;
                }
            } else {
                return NULL;
            }
        } else {
            Status = STATUS_NOT_FOUND;
        }
    }
    if (!Info) {
        EH_SetLastStatus(Status);
    }
    return Info;
}

BOOL NTAPI NT_AdjustPrivilege(_In_ HANDLE TokenHandle, _In_ SE_PRIVILEGE Privilege, _In_ DWORD Attributes) {
    TOKEN_PRIVILEGES stTokenPrivilege = { 1, { { { Privilege, 0 }, Attributes} } };
    NTSTATUS Status = NtAdjustPrivilegesToken(TokenHandle, FALSE, &stTokenPrivilege, sizeof(stTokenPrivilege), NULL, NULL);
    if (Status == STATUS_SUCCESS) {
        return TRUE;
    } else {
        EH_SetLastStatus(Status);
        return FALSE;
    }
}

BOOL NTAPI NT_Impersonate(HANDLE TokenHandle) {
    NTSTATUS Status = NtSetInformationThread(CURRENT_THREAD_HANDLE, ThreadImpersonationToken, &TokenHandle, sizeof(TokenHandle));
    if (NT_SUCCESS(Status)) {
        return TRUE;
    } else {
        EH_SetLastStatus(Status);
        return FALSE;
    }
}

_Success_(return != FALSE)
PTOKEN_PRIVILEGES NTAPI NT_InitTokenPrivileges(_In_reads_(PrivilegeCount) PSE_PRIVILEGE Privileges, _In_ DWORD PrivilegeCount, _In_ DWORD Attributes, _Out_opt_ PDWORD ReturnLength) {
    ULONG Length = ANYSIZE_STRUCT_SIZE(TOKEN_PRIVILEGES, Privileges, PrivilegeCount);
    PTOKEN_PRIVILEGES Buffer = Mem_Alloc(Length);
    if (Buffer) {
        Buffer->PrivilegeCount = PrivilegeCount;
        ULONG i;
        for (i = 0; i < PrivilegeCount; i++) {
            Buffer->Privileges[i].Luid = (LUID){ Privileges[i], 0 };
            Buffer->Privileges[i].Attributes = Attributes;
        }
        if (ReturnLength) {
            *ReturnLength = Length;
        }
    }
    return Buffer;
}

_Success_(return != FALSE)
BOOL NTAPI NT_GetActiveSession(_Out_ PDWORD SessionId) {
    PSESSIONID pBuffer = NULL, pSessionInfo;
    DWORD dwCount;
    BOOL bRet = FALSE;
    if (WinStationEnumerateW(SERVERNAME_CURRENT, &pBuffer, &dwCount) && pBuffer) {
        pSessionInfo = pBuffer;
        while (dwCount--) {
            if (pSessionInfo->State == State_Active) {
                *SessionId = pSessionInfo->_SessionId_LogonId_union.SessionId;
                bRet = TRUE;
                break;
            }
            pSessionInfo++;
        }
        WinStationFreeMemory(pBuffer);
    }
    return bRet;
}

_Success_(return != FALSE)
BOOL NTAPI NT_GetLogonSessionInfo(_In_opt_ PSID LogonUserSid, _Out_opt_ PSID UserSid, _In_opt_ ULONG UserSidSize, _Out_opt_ PLUID LogonSessionId, _Out_opt_ PDWORD SessionId) {
    BOOL bRet = FALSE;

    DWORD dwSessionId;
    if (!LogonUserSid &&
        !NT_GetActiveSession(&dwSessionId)) {
        return FALSE;
    }

    ULONG i;
    ULONG LogonSessionCount;
    PLUID LogonSessionIDs;
    NTSTATUS Status = LsaEnumerateLogonSessions(&LogonSessionCount, &LogonSessionIDs);
    if (Status == STATUS_SUCCESS) {
        for (i = 0; i < LogonSessionCount && !bRet; i++) {
            PSECURITY_LOGON_SESSION_DATA LogonSessionData;
            Status = LsaGetLogonSessionData(&LogonSessionIDs[i], &LogonSessionData);
            if (Status == STATUS_SUCCESS) {
                if (LogonUserSid ?
                    (LogonSessionData->Sid && NT_EqualSid(LogonSessionData->Sid, LogonUserSid)) :
                    LogonSessionData->Session == dwSessionId) {
                    if (UserSid) {
                        Status = NT_CopySid(UserSidSize, UserSid, LogonSessionData->Sid);
                        if (!NT_SUCCESS(Status)) {
                            LsaFreeReturnBuffer(LogonSessionData);
                            break;
                        }
                    }
                    if (LogonSessionId) {
                        *LogonSessionId = LogonSessionIDs[i];
                    }
                    if (SessionId) {
                        *SessionId = LogonSessionData->Session;
                    }
                    bRet = TRUE;
                }
                LsaFreeReturnBuffer(LogonSessionData);
            } else {
                break;
            }
        }
        LsaFreeReturnBuffer(LogonSessionIDs);
    }

    if (!bRet) {
        EH_SetLastNTError(Status);
    }
    return bRet;
}

HANDLE NTAPI NT_GetSessionToken(DWORD SessionId, BOOL UseLinkedToken) {
    HANDLE Token;
    ULONG Length;
    WINSTATIONUSERTOKEN WinStaUserToken = { (HANDLE)NT_GetTEBMember(ClientId.UniqueProcess), (HANDLE)NT_GetTEBMember(ClientId.UniqueThread), NULL };
    Token = WinStationQueryInformationW(SERVERNAME_CURRENT, SessionId, WinStationUserToken, &WinStaUserToken, sizeof(WinStaUserToken), &Length) ?
        WinStaUserToken.UserToken : NULL;
    if (Token && UseLinkedToken) {
        HANDLE LinkedToken;
        NTSTATUS Status = NtQueryInformationToken(Token, TokenLinkedToken, &LinkedToken, sizeof(LinkedToken), &Length);
        if (NT_SUCCESS(Status)) {
            NtClose(Token);
            Token = LinkedToken;
        } else if (Status != STATUS_NO_SUCH_LOGON_SESSION) {
            EH_SetLastNTError(Status);
            NtClose(Token);
            Token = NULL;
        }
    }
    return Token;
}

HANDLE NTAPI NT_CreateTokenEx(_In_ TOKEN_TYPE Type, _In_ PSID OwnerSid, _In_ PLUID AuthenticationId, _In_ PTOKEN_GROUPS Groups, _In_ PTOKEN_PRIVILEGES Privileges) {
    HANDLE Token;
    LARGE_INTEGER ExpirationTime = { 0 };
    TOKEN_OWNER Owner = { OwnerSid };
    TOKEN_PRIMARY_GROUP PrimaryGroup = { OwnerSid };
    TOKEN_USER User = { { OwnerSid } };
    TOKEN_SOURCE Source = { 0 };
    POBJECT_ATTRIBUTES pObjectAttribute;
    if (Type == TokenImpersonation) {
        SECURITY_QUALITY_OF_SERVICE Qos = { sizeof(Qos), DEFAULT_IMPERSONATION_LEVEL, SECURITY_DYNAMIC_TRACKING, FALSE };
        OBJECT_ATTRIBUTES ObjectAttribute = { sizeof(ObjectAttribute), NULL, NULL, 0, NULL, &Qos };
        pObjectAttribute = &ObjectAttribute;
    } else {
        pObjectAttribute = NULL;
    }
    NTSTATUS Status = NtCreateToken(
        &Token,
        TOKEN_ALL_ACCESS,
        pObjectAttribute,
        Type,
        AuthenticationId,
        &ExpirationTime,
        &User,
        Groups,
        Privileges,
        &Owner,
        &PrimaryGroup,
        NULL,
        &Source);
    if (NT_SUCCESS(Status)) {
        return Token;
    } else {
        EH_SetLastStatus(Status);
        return NULL;
    }
}

HANDLE NTAPI NT_CreateToken(_In_ TOKEN_TYPE Type, _In_opt_ HANDLE RefToken, _In_opt_ PSID OwnerSid, _In_opt_ PLUID AuthenticationId, _In_opt_ PTOKEN_GROUPS Groups, _In_opt_ PTOKEN_PRIVILEGES Privileges) {
    HANDLE Token = NULL;
    PSID LocalSid;
    LUID LocalAuthenticationId;
    PTOKEN_USER LocalUser = NULL;
    PTOKEN_GROUPS LocalGroups;
    PTOKEN_PRIVILEGES LocalPrivileges;

    if (OwnerSid) {
        LocalSid = OwnerSid;
    } else if (RefToken) {
        LocalUser = NT_GetTokenInfo(RefToken, TokenUser);
        if (LocalUser) {
            LocalSid = LocalUser->User.Sid;
        } else {
            return NULL;
        }
    } else {
        EH_SetLastStatus(STATUS_INVALID_PARAMETER);
        return NULL;
    }

    if (AuthenticationId) {
        LocalAuthenticationId = *AuthenticationId;
    } else if (RefToken) {
        PTOKEN_STATISTICS Statistics = NT_GetTokenInfo(RefToken, TokenStatistics);
        if (Statistics) {
            LocalAuthenticationId = Statistics->AuthenticationId;
        } else {
            goto Label_0;
        }
    } else {
        EH_SetLastStatus(STATUS_INVALID_PARAMETER);
        goto Label_0;
    }

    if (Groups) {
        LocalGroups = Groups;
    } else if (RefToken) {
        LocalGroups = NT_GetTokenInfo(RefToken, TokenGroups);
        if (!LocalGroups) {
            goto Label_0;
        }
    } else {
        EH_SetLastStatus(STATUS_INVALID_PARAMETER);
        goto Label_0;
    }

    if (Privileges) {
        LocalPrivileges = Privileges;
    } else if (RefToken) {
        LocalPrivileges = NT_GetTokenInfo(RefToken, TokenPrivileges);
        if (!LocalPrivileges) {
            goto Label_1;
        }
    } else {
        EH_SetLastStatus(STATUS_INVALID_PARAMETER);
        goto Label_1;
    }

    Token = NT_CreateTokenEx(Type, LocalSid, &LocalAuthenticationId, LocalGroups, LocalPrivileges);

    if (!Privileges) {
        Mem_Free(LocalPrivileges);
    }

Label_1:
    if (!Groups) {
        Mem_Free(LocalGroups);
    }

Label_0:
    if (!OwnerSid) {
        Mem_Free(LocalUser);
    }

    return Token;
}
