#include "pch.h"
#include "SmartRegistryKey.h"

SmartRegistryKey::SmartRegistryKey(const HKEY root, const std::wstring& key_path, REGSAM desired_access, bool temporary)
	: m_hkey(NULL)
{
	LSTATUS status = RegCreateKeyExW(root, key_path.c_str(), 0, NULL, (temporary) ? REG_OPTION_VOLATILE : REG_OPTION_NON_VOLATILE, desired_access, NULL, &m_hkey, NULL);
	if (ERROR_SUCCESS != status || NULL == m_hkey)
	{
		throw SmartRegistryKeyException("Failed to create / open requested registry key!");
	}
}

SmartRegistryKey::SmartRegistryKey(const SmartRegistryKey& root, const std::wstring& key_path, REGSAM desired_access, bool temporary)
	: SmartRegistryKey(root.getRawKey(), key_path, desired_access, temporary) 
{
}

SmartRegistryKey::~SmartRegistryKey()
{
	if (NULL != m_hkey)
	{
		// Best effort
		(VOID)RegCloseKey(m_hkey);
		m_hkey = NULL;
	}
}

std::vector<uint8_t> SmartRegistryKey::query() const
{
	// Empty string queries the default value
	return query(L"");
}

std::vector<uint8_t> SmartRegistryKey::query(const std::wstring& value_name) const
{
	std::vector<uint8_t> output;
	DWORD needed_size = 0;
	LSTATUS status = RegQueryValueExW(m_hkey, value_name.c_str(), NULL, NULL, NULL, &needed_size);
	if (ERROR_MORE_DATA != status && ERROR_SUCCESS != status)
	{
		throw SmartRegistryKeyException("Failed to query registry value size!");
	}

	output.resize(needed_size);

	status = RegQueryValueExW(m_hkey, value_name.c_str(), NULL, NULL, output.data(), &needed_size);
	if (ERROR_SUCCESS != status)
	{
		throw SmartRegistryKeyException("Failed to query registry value!");
	}

	return output;
}

void SmartRegistryKey::set(const std::vector<uint8_t>& data, RegistryValueType type)
{
	set(L"", data, type);
}

void SmartRegistryKey::set(const uint8_t* data, size_t data_size, RegistryValueType type)
{
	set(L"", data, data_size, type);
}

void SmartRegistryKey::set(const std::wstring& value_name, const std::vector<uint8_t>& data, RegistryValueType type)
{
	set(L"", value_name, data, type);
}

void SmartRegistryKey::set(const std::wstring& value_name, const uint8_t* data, size_t data_size, RegistryValueType type)
{
	set(L"", value_name, data, data_size, type);
}

void SmartRegistryKey::set(const std::wstring& sub_key, const std::wstring& value_name, const std::vector<uint8_t>& data, RegistryValueType type)
{
	set(sub_key, value_name, data.data(), data.size(), type);
}

void SmartRegistryKey::set(const std::wstring& sub_key, const std::wstring& value_name, const uint8_t* data, size_t data_size, RegistryValueType type)
{
	LSTATUS status = RegSetKeyValueW(m_hkey, sub_key.c_str(), value_name.c_str(), static_cast<DWORD>(type), data, static_cast<DWORD>(data_size));
	if (ERROR_SUCCESS != status)
	{
		throw SmartRegistryKeyException("Failed to set registry key value!");
	}
}

