#pragma once
#include "SmartWinApiException.h"
#include <vector>

enum class RegistryValueType
	: DWORD
{
	Binary = REG_BINARY,
	Dword_le = REG_DWORD_LITTLE_ENDIAN,
	Dword = REG_DWORD,
	Dword_be = REG_DWORD_BIG_ENDIAN, // Are you fucking anti-semetic?
	Expand_sz = REG_EXPAND_SZ,
	Link = REG_LINK,
	Multi_sz = REG_MULTI_SZ,
	None = REG_NONE,
	Qword = REG_QWORD,
	Qword_le = REG_QWORD_LITTLE_ENDIAN,
	Sz = REG_SZ,
};

class SmartRegistryKey
{
public:
	SmartRegistryKey(const HKEY predefined_root, const std::wstring& key_path, REGSAM desired_access = KEY_ALL_ACCESS, bool temporary = false);
	SmartRegistryKey(const SmartRegistryKey& root, const std::wstring& key_path, REGSAM desired_access = KEY_ALL_ACCESS, bool temporary = false);
	~SmartRegistryKey() noexcept;

	std::vector<uint8_t> query() const;
	std::vector<uint8_t> query(const std::wstring& value_name) const;

	void set(const std::vector<uint8_t>& data, RegistryValueType type = RegistryValueType::None);
	void set(const uint8_t* data, size_t data_size, RegistryValueType type = RegistryValueType::None);
	void set(const std::wstring& value_name, const std::vector<uint8_t>& data, RegistryValueType type = RegistryValueType::None);
	void set(const std::wstring& value_name, const uint8_t* data, size_t data_size, RegistryValueType type = RegistryValueType::None);
	void set(const std::wstring& sub_key, const std::wstring& value_name, const std::vector<uint8_t>& data, RegistryValueType type = RegistryValueType::None);
	void set(const std::wstring& sub_key, const std::wstring& value_name, const uint8_t* data, size_t data_size, RegistryValueType type = RegistryValueType::None);

	HKEY getRawKey() { return m_hkey; }
	const HKEY getRawKey() const { return m_hkey; }
private:
	HKEY m_hkey;
};

class SmartRegistryKeyException
	: public SmartWinApiException
{
	using SmartWinApiException::SmartWinApiException;
};
