#pragma once
#include <cstdint>

/*
	This header contains basic function hooking functionality, check out the module.h header file for an example.
*/

#define HOOK(address, return_t, callconv, name, ...) return_t callconv name##_##(__VA_ARGS__); \
	static inline decltype(&name##_##) name = reinterpret_cast<decltype(&name##_##)>(address) \

#define VIRTUAL 0x0

namespace Hooks
{
	bool Create(void* address, void* hook, void* call);
	bool Create(uintptr_t address, void* hook, void* call);

	bool Destroy(void* address);
	bool Destroy(uintptr_t address);

	// Use Unpatch(vtbl_func_addr) to destroy
	void VTablePatch(uintptr_t vtbl_func_addr, void* hook, void* call);
}

#define CREATE_HOOK(name) Hooks::Create(name, &name##_##, &name)
#define CREATE_VTABLE_PATCH(addr, name) Hooks::VTablePatch(addr, &name##_##, &name)
#define DESTROY_HOOK(name) Hooks::Destroy(name)