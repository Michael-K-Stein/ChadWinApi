#pragma once
#include "pch.h"
#include <Windows.h>
#include <stdexcept>
class SmartWinApiException : public std::exception
{
public:
	SmartWinApiException(const char* fuck)
		: std::exception(fuck), code(GetLastError())
	{
	}
	const int code = 0;
};
