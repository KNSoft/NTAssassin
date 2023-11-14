#include "Include\NTANT.h"

#include <ntsecapi.h>
#include <Wintexports/WIE_WinSta_API.h>

#include "Include\NTAEH.h"
#include "Include\NTARProc.h"
#include "Include\NTAMem.h"

#pragma comment(lib, "winsta.lib")
#pragma comment(lib, "Secur32.lib")

BOOL NTAPI NT_EqualSid(_In_ PSID Sid1, _In_ PSID Sid2)
{
    SIZE_T Length = NT_LengthSid(Sid1);
    return RtlCompareMemory(Sid1, Sid2, Length) == Length;
}

_Success_(NT_SUCCESS(return))
NTSTATUS NTAPI NT_CopySid(_In_ ULONG Size, _Out_ PSID SidDst, _In_ PSID SidSrc)
{
    SIZE_T Length = NT_LengthSid(SidSrc);
    if (Size < Length)
    {
        return STATUS_BUFFER_TOO_SMALL;
    } else
    {
        RtlCopyMemory(SidDst, SidSrc, Length);
        return STATUS_SUCCESS;
    }
}

static UNICODE_STRING g_LsaKeyPath = RTL_CONSTANT_STRING(L"\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Control\\Lsa");
static UNICODE_STRING g_LsaPidKeyName = RTL_CONSTANT_STRING(L"LsaPid");

DWORD NTAPI NT_GetLsaPid()
{
    DWORD Pid = 0;
    HANDLE hKey;
    NTSTATUS Status;
    OBJECT_ATTRIBUTES ObjectAttributes = RTL_CONSTANT_OBJECT_ATTRIBUTES(&g_LsaKeyPath, 0);

    Status = NtOpenKey(&hKey, KEY_QUERY_VALUE, &ObjectAttributes);
    if (!NT_SUCCESS(Status))
    {
        WIE_SetLastStatus(Status);
        return 0;
    }

    NT_RegGetDword(hKey, &g_LsaPidKeyName, &Pid);
    NtClose(hKey);
    return Pid;
}

HANDLE NTAPI NT_DuplicateSystemToken(_In_ DWORD PID, _In_ TOKEN_TYPE Type, _In_reads_(PrivilegeCount) PULONG PrivilegeToEnable, _In_ DWORD PrivilegeCount)
{
    HANDLE hDuplicatedToken = NULL;
    SECURITY_QUALITY_OF_SERVICE sqos;
    OBJECT_ATTRIBUTES oa;

    HANDLE hProc = RProc_Open(PROCESS_QUERY_LIMITED_INFORMATION, PID);
    if (hProc)
    {
        HANDLE hToken;
        NTSTATUS Status;
        Status = NtOpenProcessToken(hProc, TOKEN_QUERY | TOKEN_DUPLICATE, &hToken);
        if (NT_SUCCESS(Status))
        {
            POBJECT_ATTRIBUTES poa;
            if (Type == TokenImpersonation)
            {
                sqos = (SECURITY_QUALITY_OF_SERVICE){ sizeof(sqos), DEFAULT_IMPERSONATION_LEVEL, SECURITY_STATIC_TRACKING, FALSE };
                oa = (OBJECT_ATTRIBUTES){ sizeof(oa), NULL, NULL, 0, NULL, &sqos };
                poa = &oa;
            } else
            {
                poa = NULL;
            }
            Status = NtDuplicateToken(hToken, TOKEN_ALL_ACCESS, poa, FALSE, Type, &hDuplicatedToken);
            if (NT_SUCCESS(Status))
            {
                if (PrivilegeCount)
                {
                    ULONG PrivLength;
                    PTOKEN_PRIVILEGES Priv = NT_InitTokenPrivileges(PrivilegeToEnable, PrivilegeCount, SE_PRIVILEGE_ENABLED, &PrivLength);
                    if (Priv)
                    {
                        ULONG ReturnLength;
                        Status = NtAdjustPrivilegesToken(hDuplicatedToken, FALSE, Priv, PrivLength, NULL, &ReturnLength);
                        if (Status != STATUS_SUCCESS)
                        {
                            NtClose(hDuplicatedToken);
                            hDuplicatedToken = NULL;
                        }
                        Mem_Free(Priv);
                    } else
                    {
                        Status = WIE_GetLastStatus();
                    }
                }
            }
            NtClose(hToken);
        }
        if (!hDuplicatedToken)
        {
            WIE_SetLastStatus(Status);
        }
        NtClose(hProc);
    }
    return hDuplicatedToken;
}

PVOID NTAPI NT_GetTokenInfo(_In_ HANDLE TokenHandle, _In_ TOKEN_INFORMATION_CLASS TokenInformationClass)
{
    PVOID Info = NULL;
    ULONG Length;
    NTSTATUS Status;
    Status = NtQueryInformationToken(TokenHandle, TokenInformationClass, NULL, 0, &Length);
    if (Status == STATUS_BUFFER_TOO_SMALL)
    {
        if (Length)
        {
            Info = Mem_Alloc(Length);
            if (Info)
            {
                Status = NtQueryInformationToken(TokenHandle, TokenInformationClass, Info, Length, &Length);
                if (!NT_SUCCESS(Status))
                {
                    Mem_Free(Info);
                    Info = NULL;
                }
            } else
            {
                return NULL;
            }
        } else
        {
            Status = STATUS_NOT_FOUND;
        }
    }
    if (!Info)
    {
        WIE_SetLastStatus(Status);
    }
    return Info;
}

BOOL NTAPI NT_AdjustPrivilege(_In_ HANDLE TokenHandle, _In_ ULONG Privilege, _In_ DWORD Attributes)
{
    ULONG ReturnLength;
    TOKEN_PRIVILEGES stTokenPrivilege = { 1, { { { Privilege, 0 }, Attributes } } };
    NTSTATUS Status = NtAdjustPrivilegesToken(TokenHandle, FALSE, &stTokenPrivilege, sizeof(stTokenPrivilege), NULL, &ReturnLength);
    if (Status == STATUS_SUCCESS)
    {
        return TRUE;
    } else
    {
        WIE_SetLastStatus(Status);
        return FALSE;
    }
}

BOOL NTAPI NT_AdjustPrivileges(_In_ HANDLE TokenHandle, _In_reads_(PrivilegeCount) PULONG Privileges, _In_ DWORD PrivilegeCount, _In_ DWORD Attributes)
{
    BOOL bRet = FALSE;
    NTSTATUS Status;
    ULONG Length;
    PTOKEN_PRIVILEGES aPrivileges = NT_InitTokenPrivileges(Privileges, PrivilegeCount, Attributes, &Length);
    if (aPrivileges)
    {
        Status = NtAdjustPrivilegesToken(TokenHandle, FALSE, aPrivileges, Length, NULL, &Length);
        if (NT_SUCCESS(Status))
        {
            bRet = TRUE;
        } else
        {
            WIE_SetLastStatus(Status);
        }
        Mem_Free(aPrivileges);
    }
    return bRet;
}

BOOL NTAPI NT_Impersonate(HANDLE TokenHandle)
{
    NTSTATUS Status = NtSetInformationThread(CURRENT_THREAD_HANDLE, ThreadImpersonationToken, &TokenHandle, sizeof(TokenHandle));
    if (NT_SUCCESS(Status))
    {
        return TRUE;
    } else
    {
        WIE_SetLastStatus(Status);
        return FALSE;
    }
}

_Success_(return != FALSE)
PTOKEN_PRIVILEGES NTAPI NT_InitTokenPrivileges(_In_reads_(PrivilegeCount) PULONG Privileges, _In_ ULONG PrivilegeCount, _In_ DWORD Attributes, _Out_opt_ PULONG ReturnLength)
{
    ULONG Length = ANYSIZE_STRUCT_SIZE(TOKEN_PRIVILEGES, Privileges, PrivilegeCount);
    PTOKEN_PRIVILEGES Buffer = Mem_Alloc(Length);
    if (Buffer)
    {
        Buffer->PrivilegeCount = PrivilegeCount;
        ULONG i;
        for (i = 0; i < PrivilegeCount; i++)
        {
            Buffer->Privileges[i].Luid = (LUID){ Privileges[i], 0 };
            Buffer->Privileges[i].Attributes = Attributes;
        }
        if (ReturnLength)
        {
            *ReturnLength = Length;
        }
    }
    return Buffer;
}

_Success_(return != FALSE)
BOOL NTAPI NT_GetFirstActiveSession(_Out_ PDWORD SessionId)
{
    PSESSIONID pBuffer = NULL, pSessionInfo;
    ULONG Count;
    BOOL bRet = FALSE;
    if (WinStationEnumerateW(SERVERNAME_CURRENT, &pBuffer, &Count) && pBuffer)
    {
        pSessionInfo = pBuffer;
        while (Count--)
        {
            if (pSessionInfo->State == State_Active)
            {
                *SessionId = pSessionInfo->_SessionId_LogonId_union.SessionId;
                bRet = TRUE;
                break;
            }
            if (Count)
            {
                pSessionInfo++;
            } else
            {
                WIE_SetLastError(ERROR_NO_SUCH_LOGON_SESSION);
                break;
            }
        }
        WinStationFreeMemory(pBuffer);
    }
    return bRet;
}

_Success_(return != FALSE)
BOOL NTAPI NT_GetLogonSessionInfo(_In_opt_ PSID LogonUserSid, _Out_opt_ PSID UserSid, _In_opt_ ULONG UserSidSize, _Out_opt_ PLUID LogonSessionId, _Out_opt_ PDWORD SessionId)
{
    BOOL bRet = FALSE;

    DWORD dwSessionId;
    if (!LogonUserSid &&
        !NT_GetFirstActiveSession(&dwSessionId))
    {
        return FALSE;
    }

    ULONG i;
    ULONG LogonSessionCount;
    PLUID LogonSessionIDs;
    NTSTATUS Status = LsaEnumerateLogonSessions(&LogonSessionCount, &LogonSessionIDs);
    if (Status == STATUS_SUCCESS)
    {
        for (i = 0; i < LogonSessionCount && !bRet; i++)
        {
            PSECURITY_LOGON_SESSION_DATA LogonSessionData;
            Status = LsaGetLogonSessionData(&LogonSessionIDs[i], &LogonSessionData);
            if (Status == STATUS_SUCCESS)
            {
                if (LogonUserSid ?
                    (LogonSessionData->Sid && NT_EqualSid(LogonSessionData->Sid, LogonUserSid)) :
                    LogonSessionData->Session == dwSessionId)
                {
                    if (UserSid)
                    {
                        Status = NT_CopySid(UserSidSize, UserSid, LogonSessionData->Sid);
                        if (!NT_SUCCESS(Status))
                        {
                            LsaFreeReturnBuffer(LogonSessionData);
                            break;
                        }
                    }
                    if (LogonSessionId)
                    {
                        *LogonSessionId = LogonSessionIDs[i];
                    }
                    if (SessionId)
                    {
                        *SessionId = LogonSessionData->Session;
                    }
                    bRet = TRUE;
                }
                LsaFreeReturnBuffer(LogonSessionData);
            } else
            {
                break;
            }
        }
        LsaFreeReturnBuffer(LogonSessionIDs);
    }

    if (!bRet)
    {
        EH_SetLastNTError(Status);
    }
    return bRet;
}

HANDLE NTAPI NT_GetSessionToken(DWORD SessionId)
{
    ULONG Length;
    WINSTATIONUSERTOKEN WinStaUserToken = { (HANDLE)WIE_ReadTEB(ClientId.UniqueProcess), (HANDLE)WIE_ReadTEB(ClientId.UniqueThread), NULL };
    return WinStationQueryInformationW(SERVERNAME_CURRENT, SessionId, WinStationUserToken, &WinStaUserToken, sizeof(WinStaUserToken), &Length) ?
        WinStaUserToken.UserToken : NULL;
}

HANDLE NTAPI NT_CreateTokenEx(_In_ TOKEN_TYPE Type, _In_ PSID OwnerSid, _In_ PLUID AuthenticationId, _In_ PTOKEN_GROUPS Groups, _In_ PTOKEN_PRIVILEGES Privileges)
{
    HANDLE Token;
    LARGE_INTEGER ExpirationTime = { 0 };
    TOKEN_OWNER Owner = { OwnerSid };
    TOKEN_PRIMARY_GROUP PrimaryGroup = { OwnerSid };
    TOKEN_USER User = { { OwnerSid } };
    TOKEN_SOURCE Source = { 0 };
    POBJECT_ATTRIBUTES pObjectAttribute;
    if (Type == TokenImpersonation)
    {
        SECURITY_QUALITY_OF_SERVICE Qos = { sizeof(Qos), DEFAULT_IMPERSONATION_LEVEL, SECURITY_DYNAMIC_TRACKING, FALSE };
        OBJECT_ATTRIBUTES ObjectAttribute = { sizeof(ObjectAttribute), NULL, NULL, 0, NULL, &Qos };
        pObjectAttribute = &ObjectAttribute;
    } else
    {
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
    if (NT_SUCCESS(Status))
    {
        return Token;
    } else
    {
        WIE_SetLastStatus(Status);
        return NULL;
    }
}

HANDLE NTAPI NT_CreateToken(_In_ TOKEN_TYPE Type, _In_opt_ HANDLE RefToken, _In_opt_ PSID OwnerSid, _In_opt_ PLUID AuthenticationId, _In_opt_ PTOKEN_GROUPS Groups, _In_opt_ PTOKEN_PRIVILEGES Privileges)
{
    HANDLE Token = NULL;
    PSID LocalSid;
    LUID LocalAuthenticationId;
    PTOKEN_USER LocalUser = NULL;
    PTOKEN_GROUPS LocalGroups;
    PTOKEN_PRIVILEGES LocalPrivileges;

    if (OwnerSid)
    {
        LocalSid = OwnerSid;
    } else if (RefToken)
    {
        LocalUser = NT_GetTokenInfo(RefToken, TokenUser);
        if (LocalUser)
        {
            LocalSid = LocalUser->User.Sid;
        } else
        {
            return NULL;
        }
    } else
    {
        WIE_SetLastStatus(STATUS_INVALID_PARAMETER);
        return NULL;
    }

    if (AuthenticationId)
    {
        LocalAuthenticationId = *AuthenticationId;
    } else if (RefToken)
    {
        PTOKEN_STATISTICS Statistics = NT_GetTokenInfo(RefToken, TokenStatistics);
        if (Statistics)
        {
            LocalAuthenticationId = Statistics->AuthenticationId;
            Mem_Free(Statistics);
        } else
        {
            goto Label_0;
        }
    } else
    {
        WIE_SetLastStatus(STATUS_INVALID_PARAMETER);
        goto Label_0;
    }

    if (Groups)
    {
        LocalGroups = Groups;
    } else if (RefToken)
    {
        LocalGroups = NT_GetTokenInfo(RefToken, TokenGroups);
        if (!LocalGroups)
        {
            goto Label_0;
        }
    } else
    {
        WIE_SetLastStatus(STATUS_INVALID_PARAMETER);
        goto Label_0;
    }

    if (Privileges)
    {
        LocalPrivileges = Privileges;
    } else if (RefToken)
    {
        LocalPrivileges = NT_GetTokenInfo(RefToken, TokenPrivileges);
        if (!LocalPrivileges)
        {
            goto Label_1;
        }
    } else
    {
        WIE_SetLastStatus(STATUS_INVALID_PARAMETER);
        goto Label_1;
    }

    Token = NT_CreateTokenEx(Type, LocalSid, &LocalAuthenticationId, LocalGroups, LocalPrivileges);

    if (!Privileges)
    {
        Mem_Free(LocalPrivileges);
    }

Label_1:
    if (!Groups)
    {
        Mem_Free(LocalGroups);
    }

Label_0:
    if (!OwnerSid)
    {
        Mem_Free(LocalUser);
    }

    return Token;
}
