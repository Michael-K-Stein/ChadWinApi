#pragma once
#include "pch.h"
#include <Windows.h>
#include <winternl.h>
#include <Psapi.h>
#include <memory>
#include <stdlib.h>
#include "SmartWinApiException.h"

#ifdef _DEBUG
#define CHADWINAPILOG(format, ...) fprintf(stdout, "[*] ChadWinApi log : " __FILE__ " : " __FUNCTION__ " : " format "\n", __VA_ARGS__)
#define CHADWINAPIERROR(format, ...) do { int err = GetLastError(); fprintf(stderr, "[!] ChadWinApi error %d : " __FILE__ " : " __FUNCTION__ " : " format "\n", err, __VA_ARGS__); SetLastError(err); } while(0);
#else
#define CHADWINAPILOG(format, ...)
#define CHADWINAPIERROR(format, ...)
#endif

#define CWA_INVALID_HANDLE(x) (INVALID_HANDLE_VALUE == x || 0 == x)

#define ProcessProtectionInformation (61)

typedef BOOL(WINAPI* VirtualProtect_f)(
	_In_  LPVOID lpAddress,
	_In_  SIZE_T dwSize,
	_In_  DWORD flNewProtect,
	_Out_ PDWORD lpflOldProtect
	);
typedef LPVOID(WINAPI* VirtualAlloc_f)(
	_In_opt_ LPVOID lpAddress,
	_In_     SIZE_T dwSize,
	_In_     DWORD flAllocationType,
	_In_     DWORD flProtect
	);
typedef DWORD(WINAPI* GetModuleFileNameW_f)(
	_In_opt_ HMODULE hModule,
	_Out_writes_to_(nSize, ((return < nSize) ? (return +1) : nSize)) LPWSTR lpFilename,
	_In_ DWORD nSize
	);
typedef VOID(NTAPI* RtlExitUserProcess_f)(
	_In_ NTSTATUS ExitStatus);
typedef PVOID(NTAPI* RtlImageDirectoryEntryToData_f)(
	_In_ PVOID BaseOfImage,
	_In_ BOOLEAN MappedAsImage,
	_In_ USHORT DirectoryEntry,
	_Out_ PULONG Size);
typedef NTSTATUS(NTAPI* RtlQueryElevationFlags_f)(
	_Inout_ ULONG* ElevationFlags);
typedef HRESULT(__cdecl* CoInitializeEx_f)(
	_In_opt_ LPVOID pvReserved,
	_In_ DWORD dwCoInit
	);
typedef void (WINAPI* InitCommonControls_f)(void);
typedef BOOL(WINAPI* VirtualFree_f)(
	_Pre_notnull_ _When_(dwFreeType == MEM_DECOMMIT, _Post_invalid_) _When_(dwFreeType == MEM_RELEASE, _Post_ptr_invalid_) LPVOID lpAddress,
	_In_ SIZE_T dwSize,
	_In_ DWORD dwFreeType
	);
typedef NTSTATUS(WINAPI* NtQueryInformationProcess_f)(
	_In_      HANDLE           ProcessHandle,
	_In_      PROCESSINFOCLASS ProcessInformationClass,
	_Out_     PVOID            ProcessInformation,
	_In_      ULONG            ProcessInformationLength,
	_Out_opt_ PULONG           ReturnLength
	);

typedef enum _PS_PROTECTED_TYPE {
	PsProtectedTypeNone = 0,
	PsProtectedTypeProtectedLight = 1,
	PsProtectedTypeProtected = 2
} PS_PROTECTED_TYPE, * PPS_PROTECTED_TYPE;
typedef enum _PS_PROTECTED_SIGNER {
	PsProtectedSignerNone = 0,
	PsProtectedSignerAuthenticode,
	PsProtectedSignerCodeGen,
	PsProtectedSignerAntimalware,
	PsProtectedSignerLsa,
	PsProtectedSignerWindows,
	PsProtectedSignerWinTcb,
	PsProtectedSignerWinSystem,
	PsProtectedSignerApp,
	PsProtectedSignerMax
} PS_PROTECTED_SIGNER, * PPS_PROTECTED_SIGNER;
typedef struct _PS_PROTECTION {
	union {
		UCHAR Level;
		struct {
			UCHAR Type : 3;
			UCHAR Audit : 1;                  // Reserved
			UCHAR Signer : 4;
		};
	};
} PS_PROTECTION, * PPS_PROTECTION;

#if defined(__cplusplus)
#define GetNTDLLFunc(x) HMODULE x##_ntdll = LoadLibrary(TEXT("ntdll")); \
	if (NULL == ##x##_ntdll) {\
		throw SmartWinApiException("Failed to load NTDLL"); \
	} \
	x##_f x = (x##_f)GetProcAddress(x##_ntdll, #x); \
	if (NULL == x) {\
		throw SmartWinApiException("Failed to find function " #x "!"); \
	}

#define GetKernel32Func(x) HMODULE x##_ntdll = LoadLibrary(TEXT("Kernel32.dll")); \
	if (NULL == ##x##_ntdll) {\
		throw SmartWinApiException("Failed to load Kernel32.dll"); \
	} \
	x##_f x = (x##_f)GetProcAddress(x##_ntdll, #x); \
	if (NULL == x) {\
		throw SmartWinApiException("Failed to find function " #x "!"); \
	}
#else
#define GetNTDLLFunc(x) do {HMODULE x##_ntdll = LoadLibrary(TEXT("ntdll")); \
	if (NULL == ##x##_ntdll) {\
		break; \
	} \
	x##_f x = (x##_f)GetProcAddress(x##_ntdll, #x); \
	if (NULL == x) {\
		break; \
	}}while(0);

#define GetKernel32Func(x) do{HMODULE x##_ntdll = LoadLibrary(TEXT("Kernel32.dll")); \
	if (NULL == ##x##_ntdll) {\
		break; \
	} \
	x##_f x = (x##_f)GetProcAddress(x##_ntdll, #x); \
	if (NULL == x) {\
		break; \
	}}while(0);
#define GetOle32Func(x) do{HMODULE x##_ntdll = LoadLibrary(TEXT("Ole32.dll")); \
	if (NULL == ##x##_ntdll) {\
		break; \
	} \
	x##_f x = (x##_f)GetProcAddress(x##_ntdll, #x); \
	if (NULL == x) {\
		break; \
	}}while(0);
#define GetWinFunc(dll, x) do{ \
	x##_f x = NULL; \
	HMODULE x##_ntdll = LoadLibraryA(dll); \
	if (NULL == ##x##_ntdll) {\
		break; \
	} \
	x = (x##_f)GetProcAddress(x##_ntdll, #x); \
	if (NULL == x) {\
		break; \
	}}while(0);
#endif

#if defined(__cplusplus)
#define __enum enum class
#else
#define __enum enum
#endif


__enum ProcessIntegrityLevel
#if defined(__cplusplus)
	: unsigned long long
#endif
{
	Unknown,
	Untrusted,
	Low,
	Medium,
	High,
	System,
	Protected,
};


#if defined(__cplusplus)
extern "C" {
#endif
	int _strcmp_a(const char* s1, const char* s2);
	ProcessIntegrityLevel GetProcessIntegrityLevel(HANDLE hProcess);
	DWORD GetProcessByIntegrityLevel(ProcessIntegrityLevel);
#if defined(__cplusplus)
}
#endif
