#include "pch.h"
#include "SmartHandle.h"
SmartHandle::SmartHandle(HANDLE handle)
	: raw_handle(handle)
{
	if (0 == raw_handle || INVALID_HANDLE_VALUE == raw_handle)
	{
		throw SmartHandleException("Invalid handle passed to ctor!");
	}
}
SmartHandle::~SmartHandle()
{
	CloseHandle(raw_handle);
}