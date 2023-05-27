#include "pch.h"
#include "SmartMutex.h"
SmartMutex::SmartMutex(const std::wstring& mutex_name)
	: raw_mutex_handle(CreateMutexW(NULL, FALSE, mutex_name.c_str())), mutex_held(false)
{
	if (0 == raw_mutex_handle || INVALID_HANDLE_VALUE == raw_mutex_handle)
	{
		throw SmartMutexException("Invalid handle passed to mutex ctor!");
	}
}
SmartMutex::~SmartMutex()
{
	if (mutex_held) { ReleaseMutex(raw_mutex_handle); }
	CloseHandle(raw_mutex_handle);
}

void SmartMutex::acquire(DWORD timeout)
{
	DWORD wait_end_reason = WAIT_FAILED;
	if (WAIT_FAILED == (wait_end_reason = WaitForSingleObject(raw_mutex_handle, timeout)))
	{
		throw SmartMutexException("Failed to acquire mutex!");
	}
	if (WAIT_TIMEOUT == wait_end_reason)
	{
		throw SmartMutexLockedException("Acquire mutex timed out!");
	}
	mutex_held = true;
}
void SmartMutex::release()
{
	if (FALSE == ReleaseMutex(raw_mutex_handle))
	{
		throw SmartMutexException("Failed to release mutex!");
	}
	mutex_held = false;
}
