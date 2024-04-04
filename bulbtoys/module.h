#pragma once
#include "my_imgui.h"

/*
	This header contains the module framework. This is the main way you will be creating options and giving functionality to your trainer.

	You can have three types of modules:
		1. One which has an Init(), a Draw(), and an End() - in which case, use the MODULE(name) macro
		2. One which only has an Init() and an End() - in which case, use the MODULE_NO_DRAW(name) macro
		3. One which only has a Draw() - in which case, use the MODULE_DRAW_ONLY(name) macro

	Pick one and go wild! Scroll down for an example.
*/

class Module
{
public:
	// The drawing order is always according to the enum, from top to bottom
	enum struct DrawType
	{
		// Inside the main window.
		MainWindow,

		// Inside the overlay.
		Overlay,
	};

	using InitFn = void();
	using PanelFn = IPanel*(DrawType);
	using EndFn = void();
private:
	static inline Module* first = nullptr;
	static inline Module* last = nullptr;

	Module* next;
	Module* prev;

	const char* name;
	InitFn* Init;
	PanelFn* Panel;
	EndFn* End;
public:
	Module(const char* name, InitFn* Init, PanelFn* Panel, EndFn* End);
	Module(const Module&) = delete;
	Module(Module&&) = delete;

	inline static Module* First() { return first; }
	inline static Module* Last() { return last; }

	inline Module* Next() { return next; }
	inline Module* Prev() { return prev; }

	inline const char* Name() { return name; }
	inline InitFn* InitFunc() { return Init; }
	inline PanelFn* PanelFunc() { return Panel; }
	inline EndFn* EndFunc() { return End; }
};

#define MODULE(name)			static Module name##_##(#name, &name::Init, &name::Panel, &name::End)
#define MODULE_NO_PANEL(name)	static Module name##_##(#name, &name::Init, nullptr, &name::End)
#define MODULE_PANEL_ONLY(name)	static Module name##_##(#name, nullptr, &name::Panel, nullptr)

// Here is an example of the first type of module, complete with its own hook, window, main window menu and overlay
// These are designed to be headerless, all you need is a CPP (and optionally a folder to put your modules in) and you're good to go
#if 0
namespace my_cool_module
{
	/*
		Insert MyWindow from my_imgui.h here
	*/

	// void __thiscall CSomeClass::SetSomething(int something)
	HOOK(0x543210, void, __fastcall, CSomeClass_SetSomething, uintptr_t c_some_class, uintptr_t edx, int something);

	void Init()
	{
		CREATE_HOOK(CSomeClass_SetSomething);
	}

	// Similarly to custom windows, we must return true/false here as well. Check out my_imgui.h for more info
	bool Draw(Module::DrawType dt)
	{
		static int cool_number = 69;

		if (dt == Module::DrawType::MainWindow)
		{
			if (ImGui::BulbToys_Menu("My cool menu"))
			{
				ImGui::BulbToys_InputInt("Cool number", "##CoolNumber", &cool_number, 0, 100);

				if (ImGui::Button("Create a cool window"))
				{
					new MyWindow(cool_number);
				}
			}
		}
		else if (dt == Module::DrawType::Overlay)
		{
			ImGui::Text("The cool number: %d", cool_number);
		}

		return true;
	}

	void End()
	{
		// All hooks will be automatically destroyed in Hooks::End(), but it doesn't hurt to clean your own mess up
		DESTROY_HOOK(CSomeClass_SetSomething);
	}

	// We must use __fastcall, because we can't create __thiscall without a class as it implies a member function
	// __fastcall is identical to __thiscall, except the 2nd argument is the "edx" register, which usually isn't necessary
	void __fastcall CSomeClass_SetSomething_(uintptr_t c_some_class, uintptr_t edx, int something)
	{
		// We want our function to intercept SetSomething so "something" gets set to 69 when it tries to set 101
		if (something == 101)
		{
			something = 69;
		}

		// Call the original function
		CSomeClass_SetSomething(c_some_class, edx, something);
	}
}
#endif