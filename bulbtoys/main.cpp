#include <Windows.h>
#include <d3d9.h>

#include "main.h"
#include "io.h"
#include "gui.h"
#include "modules.h"
#include "hooks.h"
#include "utils.h"
#include "settings.h"

DWORD WINAPI BulbToys_Main(LPVOID lpThreadParameter)
{
	constexpr DWORD time = 200;

	BulbToys::SetupParams params = *(BulbToys::SetupParams*)lpThreadParameter;
	HeapFree(GetProcessHeap(), 0, lpThreadParameter);

	/*
		Hooks -> Settings -> Modules -> IO -> GUI
	*/

	if (Hooks::Init() != MH_OK)
	{
		FreeLibraryAndExitThread(params.instance, 0);
		return 0;
	}

	auto settings = Settings::Get(params.settings_file);

	Modules::Init();

	if (!params.device)
	{
		if (!params.GetDevice)
		{
			Error("Setup unsuccessful: No device found and no GetDevice function was passed");
			FreeLibraryAndExitThread(params.instance, 0);
			return 0;
		}

		while (!(params.device = params.GetDevice()))
		{
			Sleep(time);
		}
	}

	D3DDEVICE_CREATION_PARAMETERS d3d_params;
	params.device->GetCreationParameters(&d3d_params);
	HWND window = d3d_params.hFocusWindow;
	
	auto io = IO::Get(window);

	Settings::Bool<"BulbToys", "UseGUI", true> use_gui;
	auto gui = use_gui.Get() ? GUI::Get(params.device, window) : nullptr;

	while (!io->Done())
	{
		Sleep(time);
	}

	if (gui)
	{
		gui->End();
	}

	io->End();

	Modules::End();

	if (settings)
	{
		settings->End();
	}

	Hooks::End(true);

	FreeLibraryAndExitThread(params.instance, 0);
	return 0;
}

void BulbToys::Setup(BulbToys::SetupParams& params)
{
	if (!params.instance)
	{
		return;
	}

	DisableThreadLibraryCalls(params.instance);

	auto heap_params = (BulbToys::SetupParams*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(BulbToys::SetupParams));
	if (!heap_params)
	{
		return;
	}
	*heap_params = params;

	auto thread = CreateThread(nullptr, 0, BulbToys_Main, heap_params, 0, nullptr);
	if (thread)
	{
		CloseHandle(thread);
	}
}