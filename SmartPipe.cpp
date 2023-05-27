#include "pch.h"
#include "SmartPipe.h"

SmartPipe::SmartPipe()
{
	SECURITY_ATTRIBUTES sa = {};
	sa.bInheritHandle = TRUE;
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;

	if (FALSE == CreatePipe(&read_handle, &write_handle, &sa, 0) || CWA_INVALID_HANDLE(read_handle) || CWA_INVALID_HANDLE(write_handle))
	{
		throw SmartPipeException("Failed to create pipe!");
	}
}

SmartPipe::~SmartPipe() noexcept
{
	CloseHandle(read_handle);
	CloseHandle(write_handle);
}
