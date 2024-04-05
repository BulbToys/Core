#pragma once
#include <d3d9.h>

/*
	This header contains the necessary procedures to initialize the BulbToys trainer framework.

	NOTE: Currently, BulbToys is an internal trainer, which supports DirectX 9 only, and does not come with an injector.
	You will need to make the necessary changes yourself, as well as use your own injector (ie. Xenos or ASI Loader).
*/

namespace BulbToys
{
	// IDirect3DDevice9* GetDevice() { return the_device; }
	using GetDeviceFn = IDirect3DDevice9*();

	struct SetupParams
	{
		// Handle to the module instance, given by DllMain
		HMODULE instance = 0;

		// Pointer to the Direct3D device used by the game. Can be nullptr, in which case GetDevice will be used instead
		IDirect3DDevice9* device = nullptr;

		// Pointer to the function that gets called indefinitely by the created thread until a device is returned
		// In case the device is NOT nullptr, this function is unused
		// Can be nullptr, but if the device is also nullptr, the setup will fail
		BulbToys::GetDeviceFn* GetDevice = nullptr;

		// Path to the settings file, absolute or relative to the attached-to executable (NOT the DLL)
		// If nullptr, the settings functionality will not be used
		const char* settings_file = nullptr;
	};

	void Setup(SetupParams& params);

	bool Init(SetupParams& params);
	void End();
}

// Here is an example on how to initialize BulbToys for your DLL:
#if 0
int GetVersion()
{
	// For the sake of the example, let's say we have two versions of a game: v1.0 and v1.1
	// Through reverse-engineering we have concluded that:
	//   1. v1.0 has the string ":3" at address 0xAAA000, and the device is located at 0xAAA100
	//   2. v1.1 has the string ":3" at address 0xBBB000, and the device is located at 0xBBB100
	if (!strncmp(reinterpret_cast<char*>(0xAAA000), ":3", 2))
	{
		return 1;
	}
	else if (!strncmp(reinterpret_cast<char*>(0xBBB000), ":3", 2))
	{
		return 2;
	}
	return 0;
}

BOOL APIENTRY DllMain(HMODULE instance, DWORD reason, LPVOID)
{
	// Only if we're attaching to the process
	if (reason == DLL_PROCESS_ATTACH)
	{
		// You can call BulbToys::Setup here directly, but it's best to verify that we're attaching to the right process
		// Especially if said process has multiple versions you would like to support
		// You can use a function or a global, which you will likely need later down the line for version-specific memory offsets
		int version = GetVersion();

		// The third parameter of BulbToys::Setup is a lambda function which just returns the device you've passed
		// It will get called indefinitely 5 times per second until it returns a valid pointer
		// It's probably the simplest, "csgo paster :^)" way to wait for the device to start, but you're free to use a hook as well

		// If we're on version 1.0
		if (version == 1)
		{
			BulbToys::Setup(instance, Read<IDirect3DDevice9*>(0xAAA100), +[]()
			{
				return Read<IDirect3DDevice9*>(0xAAA100);
			});
		}

		// If we're on version 2.0
		else if (version == 2)
		{
			BulbToys::Setup(instance, Read<IDirect3DDevice9*>(0xBBB100), +[]()
			{
				return Read<IDirect3DDevice9*>(0xBBB100);
			});
		}

		// If we're not on either version, do not call setup, and skip attaching the DLL (by returing FALSE)

		// Some games (eg. OMSI 2) may behave weirdly if you skip attaching the DLL
		// In which case, you're free to return TRUE instead, the DLL will simply remain dormant
		else
		{
			Error("This trainer does not support your version of the game.");
			return FALSE;
		}
	}

	// Attach the DLL and do whatever else we're told to do
	return TRUE;
}
#endif