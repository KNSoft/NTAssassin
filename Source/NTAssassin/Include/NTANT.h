// Native library of NTAssassin provides interfaces to access down-level x86/x64 CPU and Windows NT in R3.

#pragma once

#include "NTAssassin_Base.h"

#include "NTANT_Object.h"

/// <summary>
/// Gets PID of CSRSS
/// </summary>
#define NT_GetCsrPid() ((DWORD)(DWORD_PTR)CsrGetProcessId())

NTA_API DWORD NTAPI NT_GetLsaPid();

/// <summary>
/// Initializes OBJECT_ATTRIBUTES structure
/// </summary>
#define NT_InitObject(Object, RootDirectory, ObjectName, Attributes) InitializeObjectAttributes(Object, ObjectName, Attributes, RootDirectory, NULL)

/// <summary>
/// Initializes OBJECT_ATTRIBUTES structure for a path
/// </summary>
/// <returns>TRUE if succeeded, or FALSE if failed, no error code reports</returns>
/// <remarks>ObjectName.Buffer should be freed by <c>Mem_HeapFree</c> when no longer needs</remarks>
NTA_API BOOL NTAPI NT_InitPathObject(POBJECT_ATTRIBUTES Object, _In_z_ PCWSTR Path, HANDLE RootDirectory, PUNICODE_STRING ObjectName);

/// <summary>
/// Gets a DWORD value from registry key
/// </summary>
/// <seealso cref="NtQueryValueKey"/>
_Success_(return != FALSE) NTA_API BOOL NTAPI NT_RegGetDword(_In_ HANDLE KeyHandle, _In_ PUNICODE_STRING KeyName, _Out_ PDWORD Value);

/// <seealso cref="RtlLengthSid"/>
#define NT_LengthSid(sid) ANYSIZE_STRUCT_SIZE(SID, SubAuthority, ((sid)->SubAuthorityCount))

/// <summary>
/// Compares sids
/// </summary>
/// <seealso cref="RtlEqualSid"/>
NTA_API BOOL NTAPI NT_EqualSid(_In_ PSID Sid1, _In_ PSID Sid2);

/// <summary>
/// Copies sid
/// </summary>
/// <seealso cref="RtlCopySid"/>
_Success_(NT_SUCCESS(return)) NTA_API NTSTATUS NTAPI NT_CopySid(_In_ ULONG Size, _Out_ PSID SidDst, _In_ PSID SidSrc);

/// <summary>
/// Gets information of a token
/// </summary>
/// <seealso cref="NtQueryInformationToken"/>
/// <returns>Pointer to a new allocated buffer contains information, should be freed by <c>Mem_Free</c></returns>
NTA_API PVOID NTAPI NT_GetTokenInfo(_In_ HANDLE TokenHandle, _In_ TOKEN_INFORMATION_CLASS TokenInformationClass);

/// <summary>
/// Adjusts privilege of a token
/// </summary>
/// <param name="TokenHandle">Handle to the token</param>
/// <param name="Privilege">Privilege value to adjust, SE_XXX_PRIVILEGE</param>
/// <param name="Attributes">Attribute of privilege, SE_XXX_PRIVILEGE, or 0 to disable the privilege</param>
/// <returns>TRUE if succeeded, or FALSE if failed, error code storaged in last STATUS</returns>
NTA_API BOOL NTAPI NT_AdjustPrivilege(_In_ HANDLE TokenHandle, _In_ ULONG Privilege, _In_ DWORD Attributes);

NTA_API BOOL NTAPI NT_AdjustPrivileges(_In_ HANDLE TokenHandle, _In_reads_(PrivilegeCount) PULONG Privileges, _In_ DWORD PrivilegeCount, _In_ DWORD Attributes);

/// <summary>
/// Sets impersonate token to current thread
/// </summary>
/// <param name="TokenHandle">Handle to the impersonate token, or NULL to terminates the impersonation</param>
/// <returns>TRUE if succeeded, or FALSE if failed, error code storaged in last STATUS</returns>
NTA_API BOOL NTAPI NT_Impersonate(HANDLE TokenHandle);

/// <summary>
/// Duplicates process token from system PPL process
/// </summary>
/// <returns>Handle to the impersonate token created, or NULL if failed, error code storaged in last STATUS</returns>
NTA_API HANDLE NTAPI NT_DuplicateSystemToken(_In_ DWORD PID, _In_ TOKEN_TYPE Type, _In_reads_(PrivilegeCount) PULONG PrivilegeToEnable, _In_ DWORD PrivilegeCount);

/// <summary>
/// Gets the first active session
/// </summary>
/// <param name="SessionId">Pointer to a DWORD variable receives Session ID returned</param>
/// <returns>TRUE if succeeded, or FALSE if failed, error code storaged in last ERROR</returns>
_Success_(return != FALSE) NTA_API BOOL NTAPI NT_GetFirstActiveSession(_Out_ PDWORD SessionId);


/// <summary>
/// Allocates and initialize a TOKEN_PRIVILEGES structure
/// </summary>
/// <param name="Privileges">Pointer to an array of privilege values (SE_XXX_PRIVILEGE)</param>
/// <param name="PrivilegeCount">Number of entries in the Privileges array</param>
/// <param name="Attributes">Privilege attributes (SE_PRIVILEGE_XXX)</param>
/// <param name="ReturnLength">Size in bytes of allocated buffer</param>
/// <returns>Pointer to the allocated TOKEN_PRIVILEGES structure, or NULL if failed, both of last error and last status will set</returns>
_Success_(return != FALSE) NTA_API PTOKEN_PRIVILEGES NTAPI NT_InitTokenPrivileges(_In_reads_(PrivilegeCount) PULONG Privileges, _In_ ULONG PrivilegeCount, _In_ DWORD Attributes, _Out_opt_ PULONG ReturnLength);

/// <summary>
/// Gets token of specified session
/// </summary>
/// <param name="SessionId">Id to the session</param>
/// <returns>Handle to the token, or NULL if failed, error code storaged in last ERROR</returns>
NTA_API HANDLE NTAPI NT_GetSessionToken(DWORD SessionId);

/// <summary>
/// Gets logon session information
/// </summary>
/// <param name="LogonUserSid">Inputs SID of logon user, if specify NULL, information of active logon session returned</param>
/// <param name="UserSid">User SID</param>
/// <param name="UserSidSize">Size in bytes of UserSid</param>
/// <param name="LogonSessionId">Logon session ID</param>
/// <param name="SessionId">Session ID</param>
/// <returns>TRUE if succeeded, or FALSE if failed, error code storaged in last ERROR</returns>
_Success_(return != FALSE) NTA_API BOOL NTAPI NT_GetLogonSessionInfo(_In_opt_ PSID LogonUserSid, _Out_opt_ PSID UserSid, _In_opt_ ULONG UserSidSize, _Out_opt_ PLUID LogonSessionId, _Out_opt_ PDWORD SessionId);

/// <summary>
/// Creates a token
/// </summary>
/// <param name="Type">Type of token</param>
/// <param name="OwnerSid">SID of owner</param>
/// <param name="AuthenticationId">Logon session ID</param>
/// <param name="Groups">Token groups</param>
/// <param name="Privileges">Token privileges</param>
/// <returns>Handle to the token new created, or NULL if failed, error code storaged in last STATUS</returns>
NTA_API HANDLE NTAPI NT_CreateTokenEx(_In_ TOKEN_TYPE Type, _In_ PSID OwnerSid, _In_ PLUID AuthenticationId, _In_ PTOKEN_GROUPS Groups, _In_ PTOKEN_PRIVILEGES Privileges);

/// <summary>
/// Creates a token according to an existing token
/// </summary>
/// <param name="Type">Type of token</param>
/// <param name="RefToken">Handle to an existing token to reference</param>
/// <param name="OwnerSid">SID of owner, or NULL to use owner of RefToken</param>
/// <param name="AuthenticationId">Logon session ID, or NULL to use logon session ID of RefToken</param>
/// <param name="Groups">Token groups, or NULL to use groups of RefToken</param>
/// <param name="Privileges">Token privileges, or NULL to use privileges of RefToken</param>
/// <returns>Handle to the token new created, or NULL if failed, error code storaged in last STATUS</returns>
NTA_API HANDLE NTAPI NT_CreateToken(_In_ TOKEN_TYPE Type, _In_opt_ HANDLE RefToken, _In_opt_ PSID OwnerSid, _In_opt_ PLUID AuthenticationId, _In_opt_ PTOKEN_GROUPS Groups, _In_opt_ PTOKEN_PRIVILEGES Privileges);

/// <summary>
/// Gets system information
/// </summary>
/// <seealso cref="NtQuerySystemInformation"/>
/// <returns>Pointer to a new allocated buffer contains information, should be freed by <c>Mem_Free</c></returns>
NTA_API PVOID NTAPI NT_GetSystemInfo(_In_ SYSTEM_INFORMATION_CLASS SystemInformationClass);

/// <seealso cref="IsProcessorFeaturePresent"/>
#define NT_TestCPUFeature(Feature) ((Feature) < PROCESSOR_FEATURE_MAX ? (BOOL)SharedUserData->ProcessorFeatures[(Feature)] : FALSE)
