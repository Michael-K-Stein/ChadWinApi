// ChadWinApi.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"
#include "ChadWinApi.h"


int _strcmp_a(const char* s1, const char* s2)
{
	char c1, c2;

	if (s1 == s2)
		return 0;

	if (s1 == 0)
		return -1;

	if (s2 == 0)
		return 1;

	do {
		c1 = *s1;
		c2 = *s2;
		s1++;
		s2++;
	} while ((c1 != 0) && (c1 == c2));

	return (int)(c1 - c2);
}


ProcessIntegrityLevel GetProcessIntegrityLevel(HANDLE hProcess)
{
	HANDLE hToken = INVALID_HANDLE_VALUE;
	DWORD dwLengthNeeded = 0;
	PTOKEN_MANDATORY_LABEL pTIL = NULL;
	LPWSTR pStringSid = NULL;
	DWORD dwIntegrityLevel = 0;
	ProcessIntegrityLevel pil = ProcessIntegrityLevel::Unknown;

	if (FALSE == OpenProcessToken(hProcess, TOKEN_QUERY |
		TOKEN_QUERY_SOURCE, &hToken))
	{
		CHADWINAPIERROR("Failed to OpenProcessToken!");
		return ProcessIntegrityLevel::Unknown;
	}

	// Get token buffer length
	if (TRUE == GetTokenInformation(hToken, TokenIntegrityLevel, NULL, 0, &dwLengthNeeded))
	{
		CHADWINAPIERROR("Unexpected return value from GetTokenInformation!");
		goto cleanup_and_exit;
	}
	
	if (ERROR_INSUFFICIENT_BUFFER != GetLastError())
	{
		CHADWINAPIERROR("Unexpected return value from GetTokenInformation, unable to calculate buffer size!");
		goto cleanup_and_exit;
	}

	pTIL = (PTOKEN_MANDATORY_LABEL)LocalAlloc(0, dwLengthNeeded);
	if (NULL == pTIL)
	{
		CHADWINAPIERROR("Unable to allocate TOKEN_MANDATORY_LABEL buffer of size %lu!", dwLengthNeeded);
		goto cleanup_and_exit;
	}

	if (FALSE == GetTokenInformation(hToken, TokenIntegrityLevel, pTIL, dwLengthNeeded, &dwLengthNeeded))
	{
		CHADWINAPIERROR("Failed to GetTokenInformation!");
		goto cleanup_and_exit;
	}

	dwIntegrityLevel = *GetSidSubAuthority(pTIL->Label.Sid, (DWORD)(UCHAR)(*GetSidSubAuthorityCount(pTIL->Label.Sid) - 1));

	if (dwIntegrityLevel >= SECURITY_MANDATORY_UNTRUSTED_RID && dwIntegrityLevel < SECURITY_MANDATORY_LOW_RID)
	{
		pil = ProcessIntegrityLevel::Untrusted;
	}
	else if (dwIntegrityLevel >= SECURITY_MANDATORY_LOW_RID && dwIntegrityLevel < SECURITY_MANDATORY_MEDIUM_RID)
	{
		pil = ProcessIntegrityLevel::Low;
	}
	else if (dwIntegrityLevel >= SECURITY_MANDATORY_MEDIUM_RID && dwIntegrityLevel < SECURITY_MANDATORY_HIGH_RID)
	{
		pil = ProcessIntegrityLevel::Medium;
	}
	else if (dwIntegrityLevel >= SECURITY_MANDATORY_HIGH_RID && dwIntegrityLevel < SECURITY_MANDATORY_SYSTEM_RID)
	{
		pil = ProcessIntegrityLevel::High;
	}
	else if (dwIntegrityLevel >= SECURITY_MANDATORY_SYSTEM_RID && dwIntegrityLevel < SECURITY_MANDATORY_PROTECTED_PROCESS_RID)
	{
		pil = ProcessIntegrityLevel::System;
	}
	else if (dwIntegrityLevel >= SECURITY_MANDATORY_PROTECTED_PROCESS_RID)
	{
		pil = ProcessIntegrityLevel::Protected;
	}

cleanup_and_exit:
	CloseHandle(hToken);
	if (NULL != pTIL)
	{
		LocalFree(pTIL);
	}
	return pil;
	
}

DWORD GetProcessByIntegrityLevel(ProcessIntegrityLevel il)
{
	DWORD current_process_id = GetCurrentProcessId();
	size_t max_process_count = 0x1000;
	DWORD* process_ids = (DWORD*)calloc(max_process_count, sizeof(DWORD));
	if (process_ids == nullptr)
	{
		return 0;
	}
	DWORD process_id = 0;
	DWORD size_needed = 0;
	if (FALSE == EnumProcesses(process_ids, max_process_count * sizeof(DWORD), &size_needed))
	{
		goto cleanup_and_exit;
	}
	for (size_t index = 0; index < size_needed / sizeof(DWORD); ++index)
	{
		DWORD id = process_ids[index];
		if (id == current_process_id)
		{
			continue;
		}
		HANDLE phandle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, id);
		if (CWA_INVALID_HANDLE(phandle))
		{
			continue;
		}
		ProcessIntegrityLevel pil = GetProcessIntegrityLevel(phandle);
		if (pil != il)
		{
			CloseHandle(phandle);
			continue;
		}
		BOOL iswow_process = FALSE;
		if (FALSE == IsWow64Process(phandle, &iswow_process))
		{
			CloseHandle(phandle);
			continue;
		}
		if (iswow_process)
		{
			CloseHandle(phandle);
			continue;
		}
		PS_PROTECTION psp = {};
		GetNTDLLFunc(NtQueryInformationProcess);
		if (!NT_SUCCESS(NtQueryInformationProcess(phandle, (PROCESSINFOCLASS)ProcessProtectionInformation, &psp, sizeof(psp), NULL)))
		{
			CloseHandle(phandle);
			continue;
		}
		if (psp.Type != PsProtectedTypeNone)
		{
			CloseHandle(phandle);
			continue;
		}

		CloseHandle(phandle);
		process_id = id;
		goto cleanup_and_exit;

		break;
	}
cleanup_and_exit:

	free(process_ids);

	return process_id;
}



// TODO: This is an example of a library function
void fnChadWinApi()
{
}
