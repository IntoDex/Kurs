#include <windows.h>
#include <iostream>

void registry_read(LPCWSTR subkey, LPCWSTR name, DWORD type)
{
	HKEY key;
	WCHAR value[255];
	DWORD value_length = 255;
	RegOpenKey(HKEY_CURRENT_USER, subkey, &key);
	RegQueryValueEx(key, name, NULL, &type, (LPBYTE)&value, &value_length);
	RegCloseKey(key);
	std::cout << value << std::endl;
}

void registry_write(LPCWSTR subkey, LPCWSTR name, DWORD type, const wchar_t* value)

{
	HKEY key;
	RegOpenKey(HKEY_CURRENT_USER, subkey, &key);
	RegSetValueEx(key, name, 0, type, (LPBYTE)value, wcslen(value) * sizeof(wchar_t));
	RegCloseKey(key);
}


void registry_read(LPCWSTR subkey, LPCWSTR name, DWORD type)
{
	HKEY rKey;
	unsigned char Reget[512];
	DWORD RegetPath = sizeof(Reget);
	RegOpenKey(HKEY_CURRENT_USER, subkey, &rKey);
	RegQueryValueEx(rKey, name, NULL, &type, (LPBYTE)&Reget, &RegetPath);
	RegCloseKey(rKey);
	wprintf(L"%s\n", Reget);

}




int main()
{
	registry_read(L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", L"Discord", REG_SZ);

	//  registry_write(L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", L"NewValue", REG_SZ, L"Content");
	return 0;
}