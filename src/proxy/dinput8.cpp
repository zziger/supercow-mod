#include <string>
#include <windows.h>

#include "main.h"

struct dinput8_dll { 
	HMODULE dll;
	FARPROC OrignalDirectInput8Create;
	FARPROC OrignalDllCanUnloadNow;
	FARPROC OrignalDllGetClassObject;
	FARPROC OrignalDllRegisterServer;
	FARPROC OrignalDllUnregisterServer;
	FARPROC OrignalGetdfDIJoystick;
} dinput8;

extern "C"
{
	__declspec(naked) void __stdcall FakeDirectInput8Create() { _asm { jmp[dinput8.OrignalDirectInput8Create] } }
	__declspec(naked) void __stdcall FakeDllCanUnloadNow() { _asm { jmp[dinput8.OrignalDllCanUnloadNow] } }
	__declspec(naked) void __stdcall FakeDllGetClassObject() { _asm { jmp[dinput8.OrignalDllGetClassObject] } }
	__declspec(naked) void __stdcall FakeDllRegisterServer() { _asm { jmp[dinput8.OrignalDllRegisterServer] } }
	__declspec(naked) void __stdcall FakeDllUnregisterServer() { _asm { jmp[dinput8.OrignalDllUnregisterServer] } }
	__declspec(naked) void __stdcall FakeGetdfDIJoystick() { _asm { jmp[dinput8.OrignalGetdfDIJoystick] } }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	char path[MAX_PATH];
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
				wchar_t path[MAX_PATH];
				GetSystemDirectory(path, MAX_PATH);
				std::wstring pathname = path + std::wstring(L"\\dinput8.dll");
				dinput8.dll = LoadLibrary(pathname.c_str());
				if (dinput8.dll == nullptr)
				{
					MessageBox(0, L"Cannot load original dinput8.dll library", L"Proxy", MB_ICONERROR);
					ExitProcess(0);
				}
		dinput8.OrignalDirectInput8Create = GetProcAddress(dinput8.dll, "DirectInput8Create");
		dinput8.OrignalDllCanUnloadNow = GetProcAddress(dinput8.dll, "DllCanUnloadNow");
		dinput8.OrignalDllGetClassObject = GetProcAddress(dinput8.dll, "DllGetClassObject");
		dinput8.OrignalDllRegisterServer = GetProcAddress(dinput8.dll, "DllRegisterServer");
		dinput8.OrignalDllUnregisterServer = GetProcAddress(dinput8.dll, "DllUnregisterServer");
		dinput8.OrignalGetdfDIJoystick = GetProcAddress(dinput8.dll, "GetdfDIJoystick");

		break;
	}
	case DLL_PROCESS_DETACH:
	{
		FreeLibrary(dinput8.dll);
	}
	break;
	}
	return main(hModule, ul_reason_for_call, lpReserved);
}
