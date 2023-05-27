#pragma once
#include "SmartWinApiException.h"
class SmartMutex
{
public:
	SmartMutex(const std::wstring& mutex_name);
	~SmartMutex();
	const HANDLE getRawHandle() const { return raw_mutex_handle; }
	HANDLE getRawHandle() { return raw_mutex_handle; }
	void acquire(DWORD timeout = INFINITE);
	void release();
private:
	const HANDLE raw_mutex_handle;
	bool mutex_held;
};

class SmartMutexException : public SmartWinApiException
{
	using SmartWinApiException::SmartWinApiException;
};

class SmartMutexLockedException : public SmartMutexException
{
	using SmartMutexException::SmartMutexException;
};
