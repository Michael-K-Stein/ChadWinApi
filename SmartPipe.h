#pragma once
#include "ChadWinApi.h"
#include "SmartHandle.h"
class SmartPipe
{
public:
	SmartPipe();
	~SmartPipe() noexcept;
private:
	HANDLE read_handle;
	HANDLE write_handle;
};

class SmartPipeException : public SmartWinApiException
{
	using SmartWinApiException::SmartWinApiException;
};
