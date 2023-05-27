#include "pch.h"
#include "SmartEvent.h"

SmartEvent::SmartEvent(const std::wstring& name, bool manual, bool signaled)
	: event_handle(CreateEventW(NULL, (manual) ? TRUE : FALSE, (signaled) ? TRUE : FALSE, name.c_str()))
{
}
void SmartEvent::wait(DWORD timeout)
{
	DWORD reason = WAIT_FAILED;
	if (WAIT_FAILED == (reason = WaitForSingleObject(event_handle.getRawHandle(), timeout)))
	{
		throw SmartEventException("Failed to wait on event!");
	}
	else if (WAIT_TIMEOUT == reason)
	{
		throw SmartEventException("Timed out waiting for event!");
	}
}

void SmartEvent::signal() noexcept
{
	SetEvent(event_handle.getRawHandle());
}
void SmartEvent::unsignal() noexcept
{
	ResetEvent(event_handle.getRawHandle());
}