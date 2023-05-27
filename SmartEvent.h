#pragma once
#include "SmartWinApiException.h"
#include "SmartHandle.h"
#define ANONYMOUS_EVENT (L"")
class SmartEvent
{
public:
	SmartEvent(const std::wstring& name = ANONYMOUS_EVENT, bool manual = false, bool signaled = false);
	void wait(DWORD timeout = INFINITE);
	void signal() noexcept;
	void unsignal() noexcept;
private:
	const SmartHandle event_handle;
};

class SmartEventException : public SmartWinApiException
{
	using SmartWinApiException::SmartWinApiException;
};
