//#define _CRT_SECURE_NO_WARNINGS
#include "pch.h"
#include "Windows.h"
#include <objbase.h>
#include <assert.h>
#include <fstream>
#include <shlwapi.h> 
#pragma comment(lib, "Shlwapi.lib")

const int CLSID_STRING_SIZE = 39;

static void LogMsg(const WCHAR* fmt, ...)
{
	WCHAR tempPath[MAX_PATH];
	DWORD n = GetTempPathW(MAX_PATH, tempPath);
	if (n == 0 || n >= MAX_PATH) return;
	wcscat_s(tempPath, L"os12_reg.log");

	std::wofstream f(tempPath, std::ios::app);
	if (!f.is_open()) return;

	va_list args;
	va_start(args, fmt);
	WCHAR buf[2048];
	_vsnwprintf_s(buf, _countof(buf), _TRUNCATE, fmt, args);
	va_end(args);
	f << buf << std::endl;
	f.close();
}


HRESULT RegisterServer(HMODULE hModule,            // DLL module handle
	const CLSID& clsid,         // Class ID
	const WCHAR* szFriendlyName, // Friendly Name
	const WCHAR* szVerIndProgID, // Programmatic
	const WCHAR* szProgID)       //   IDs
{

	WCHAR szModule[512];
	DWORD dwResult = GetModuleFileName(hModule, szModule, sizeof(szModule) / sizeof(WCHAR));
	assert(dwResult != 0);

	WCHAR szCLSID[CLSID_STRING_SIZE];
	CLSIDtochar(clsid, szCLSID, sizeof(szCLSID));

	WCHAR szKey[264];      //smw szKey[64]
	wcscpy_s(szKey, L"CLSID\\");
	wcscat_s(szKey, szCLSID);

	if (!setKeyAndValue(szKey, NULL, szFriendlyName)) return HRESULT_FROM_WIN32(GetLastError());
	if (!setKeyAndValue(szKey, L"InprocServer32", szModule)) return HRESULT_FROM_WIN32(GetLastError());
	if (!setKeyAndValue(szKey, L"InprocServer32\\ThreadingModel", L"Apartment")) return HRESULT_FROM_WIN32(GetLastError());
	if (!setKeyAndValue(szKey, L"ProgID", szProgID)) return HRESULT_FROM_WIN32(GetLastError());
	if (!setKeyAndValue(szKey, L"VersionIndependentProgID", szVerIndProgID)) return HRESULT_FROM_WIN32(GetLastError());
	if (!setKeyAndValue(szVerIndProgID, NULL, szFriendlyName)) return HRESULT_FROM_WIN32(GetLastError());
	if (!setKeyAndValue(szVerIndProgID, L"CLSID", szCLSID)) return HRESULT_FROM_WIN32(GetLastError());
	if (!setKeyAndValue(szVerIndProgID, L"CurVer", szProgID)) return HRESULT_FROM_WIN32(GetLastError());
	if (!setKeyAndValue(szProgID, NULL, szFriendlyName)) return HRESULT_FROM_WIN32(GetLastError());
	if (!setKeyAndValue(szProgID, L"CLSID", szCLSID)) return HRESULT_FROM_WIN32(GetLastError());
	LogMsg(L"RegisterServer: finished successfully for CLSID %s", szCLSID);

	return S_OK;
}

HRESULT UnregisterServer(const CLSID& clsid,
	const WCHAR* szVerIndProgID,
	const WCHAR*  szProgID)

{
	WCHAR szCLSID[CLSID_STRING_SIZE];
	CLSIDtochar(clsid, szCLSID, sizeof(szCLSID));

	WCHAR szKey[264];     //smw szKey[64]
	wcscpy_s(szKey, L"CLSID\\");
	wcscat_s(szKey, szCLSID);

	LONG lResult = recursiveDeleteKey(HKEY_CLASSES_ROOT, szKey);
	assert((lResult == ERROR_SUCCESS) || (lResult == ERROR_FILE_NOT_FOUND)); // Subkey may not exist.
	lResult = recursiveDeleteKey(HKEY_CLASSES_ROOT, szVerIndProgID);
	assert((lResult == ERROR_SUCCESS) || (lResult == ERROR_FILE_NOT_FOUND)); // Subkey may not exist.
	lResult = recursiveDeleteKey(HKEY_CLASSES_ROOT, szProgID);
	assert((lResult == ERROR_SUCCESS) || (lResult == ERROR_FILE_NOT_FOUND)); // Subkey may not exist.

	return S_OK;
}



void CLSIDtochar(const CLSID& clsid,      // Convert a CLSID to a char string.   
	WCHAR* szCLSID,          // smw:char* szCLSID,
	int length)
{
	assert(length >= CLSID_STRING_SIZE);

	LPOLESTR wszCLSID = NULL;
	HRESULT hr = StringFromCLSID(clsid, &wszCLSID);
	assert(SUCCEEDED(hr));
	wcscpy_s(szCLSID, CLSID_STRING_SIZE, wszCLSID);
	CoTaskMemFree(wszCLSID);

}


LONG recursiveDeleteKey(HKEY hKeyParent,           // Parent of key to delete
	const WCHAR* lpszKeyChild)  // Key to delete
{

	HKEY hKeyChild;
	LONG lRes = RegOpenKeyEx(hKeyParent, lpszKeyChild, 0,
		KEY_ALL_ACCESS, &hKeyChild);
	if (lRes != ERROR_SUCCESS)
	{
		return lRes;
	}

	FILETIME time;
	WCHAR szBuffer[256];
	DWORD dwSize = 256;
	while (RegEnumKeyEx(hKeyChild, 0, szBuffer, &dwSize, NULL,
		NULL, NULL, &time) == S_OK)
	{
		lRes = recursiveDeleteKey(hKeyChild, szBuffer);
		if (lRes != ERROR_SUCCESS)
		{
			RegCloseKey(hKeyChild);
			return lRes;
		}
		dwSize = 256;
	}

	RegCloseKey(hKeyChild);
	return RegDeleteKey(hKeyParent, lpszKeyChild);
}

BOOL setKeyAndValue(const WCHAR* szKey,         // smw:const char* szKey
	const WCHAR* szSubkey,      // smw:const char* szSubkey,
	const WCHAR* szValue)       // smw: const char* szValue
{
	HKEY hKey;
	WCHAR szKeyBuf[1024];

	wcscpy_s(szKeyBuf, szKey);

	if (szSubkey != NULL)
	{
		wcscat_s(szKeyBuf, L"\\");
		wcscat_s(szKeyBuf, szSubkey);
	}

	LogMsg(L"setKeyAndValue: creating/opening key: %s", szKeyBuf);

	long lResult = RegCreateKeyEx(HKEY_CLASSES_ROOT,
		szKeyBuf,
		0, NULL, REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS, NULL,
		&hKey, NULL);
	if (lResult != ERROR_SUCCESS)
	{
		LogMsg(L"RegCreateKeyExW failed for %s : error=%lu", szKeyBuf, (DWORD)lResult);
		return FALSE;
	}

	if (szValue != NULL)
	{
		
		DWORD cb = (DWORD)((wcslen(szValue) + 1) * sizeof(WCHAR));
		lResult = RegSetValueExW(hKey, NULL, 0, REG_SZ, (const BYTE*)szValue, cb);
		if (lResult != ERROR_SUCCESS)
		{
			LogMsg(L"RegSetValueExW failed for %s = %s : error=%lu", szKeyBuf, szValue, (DWORD)lResult);
			RegCloseKey(hKey);
			return FALSE;
		}
		else
		{
			LogMsg(L"RegSetValueExW success for %s = %s", szKeyBuf, szValue);
		}
	}

	RegCloseKey(hKey);
	return TRUE;
}
