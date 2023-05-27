#pragma once
#include "SmartWinApiException.h"
class SmartHandle
{
public:
	SmartHandle(HANDLE handle);
	~SmartHandle();
	HANDLE getRawHandle() { return raw_handle; }
	const HANDLE getRawHandle() const { return raw_handle; }
private:
	const HANDLE raw_handle;
};

class SmartHandleException : public SmartWinApiException
{
	using SmartWinApiException::SmartWinApiException;
};
