#pragma once
#include <Windows.h>
#include <unordered_map>

/*
	This header contains various utility and helper functions/structures, such as:
		1. ASSERT(condition) and various other macros
		2. Error("Message") and various other helper funcs
		3. Read<T> and Write<T> for very basic memory access
		4. IFile for saving and loading structs to and from files
		5. Patch<T>, PatchNOP, PatchJMP and Unpatch for patching functionality (memory writes that you can undo)
*/

#define PROJECT_NAME "BulbToys"

#define DIE() *((int*)0xDEAD) = 0
#define ASSERT(cond) do { if (!(cond)) { Error("Assertion failed: " #cond); DIE(); } } while (false)

#define MYPRINTF(dest, size, fmt, ...) _snprintf_s(dest, size, size - 1, fmt, __VA_ARGS__)

/* ===== Structs ===== */

// Base interface for all struct-files. Use IFile instead, which automatically implements Size() for you
struct IFileBase
{
	// Size of the data being saved
	virtual size_t Size() const = 0;

	// Return false if struct/class data is bogus, true otherwise - returning false prompts a warning when saving/loading
	// Use this as a sanity check for when you're about to save or load a struct to/from a file
	virtual bool Validate() = 0;

	// Save the struct to a file named filename
	virtual void Save(const char* filename) final;

	// Prompt the user where to save the file
	virtual void SaveDialog(const char* title = "Save File", const char* filter = "All Files (*.*)\0*.*\0", const char* default_extension = nullptr) final;

	// Load the struct from a file named filename. Object remains unchanged if this returns false!
	virtual bool Load(const char* filename, bool allow_undersize = false) final;

	// Prompt the user which file to load. Object remains unchanged if this returns false!
	virtual bool LoadDialog(const char* title = "Load File", const char* filter = "All Files (*.*)\0*.*\0", const char* default_extension = nullptr,
		bool allow_undersize = false) final;
};

// Interface for structs/classes that can be saved to files in a binary format
// If you wish to save game structs/classes, it's best to make an adapter class first, as this will append its own vtable and fuck up your member offsets
template <typename T>
struct IFile : IFileBase
{
	virtual size_t Size() const override final { return sizeof(T); }
};

// Here is a very basic example of a game struct and its adapter class
// From here you can do your Save/Load(Dialog)ing and manually copy the member variables from the adapter to the game struct
#if 0
struct CSomeStruct
{
	// Must be between 0 and 10
	int data_1;

	// Must be exactly 5.0
	float data_2;

	// Can be anything
	char data_3[8];
};

struct CSomeStructFile : public IFile
{
	CSomeStruct some_struct;

	virtual bool Validate() override final
	{
		if (some_struct.data_1 < 0 || some_struct.data_1 > 10)
		{
			return false;
		}

		if (some_struct.data_2 != 5.0)
		{
			return false;
		}

		return true;
	}
};
#endif

class PatchInfo
{
	static inline std::unordered_map<uintptr_t, PatchInfo*> map;

	char* bytes = nullptr;

	uintptr_t address = 0;
	size_t len = 0;
	bool vprot = false;
public:
	PatchInfo(uintptr_t address, size_t len, bool use_vprot = false);
	~PatchInfo();

	inline char* Bytes() { return bytes; }
	inline size_t Len() { return len; }

	static PatchInfo* Find(uintptr_t address);
};

// Wrapper for string literals, primarily used for template arguments
template <size_t N>
struct StringLiteral
{
	char str[N];
	constexpr StringLiteral(const char(&str)[N])
	{
		for (int i = 0; i < N; i++)
		{
			this->str[i] = str[i];
		}
	}
};

// Simple RAII implementation of VirtualProtect
class Unprotect
{
	void* address;
	size_t size;
	DWORD protection_flags;
public:
	Unprotect(void* address, size_t size) : address(address), size(size) { VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &protection_flags); }
	Unprotect(uintptr_t address, size_t size) : Unprotect(reinterpret_cast<void*>(address), size) {}
	~Unprotect() { VirtualProtect(address, size, protection_flags, &protection_flags); }
};

/* ===== Templates ===== */

template <typename T>
inline T Read(uintptr_t address)
{
	return *reinterpret_cast<T*>(address);
}

template <typename T>
inline void Write(uintptr_t address, T value)
{
	*reinterpret_cast<T*>(address) = value;
}

// Returns THE ADDRESS to the virtual function at <index> in the virtual table of this_
template <int index>
inline uintptr_t Virtual(uintptr_t this_)
{
	return Read<uintptr_t>(Read<uintptr_t>(this_) + index * 4);
}

// Returns A POINTER TO THE ADDRESS of the virtual function at <index> in the virtual table of this_
template <int index>
inline uintptr_t PtrVirtual(uintptr_t this_)
{
	return Read<uintptr_t>(this_) + index * 4;
}

// Identical to Write, except it gets added to the patch map (for later unpatching)
template <typename T>
inline void Patch(uintptr_t address, T value)
{
	auto patch = new PatchInfo(address, sizeof(T));

	T* memory = reinterpret_cast<T*>(address);
	*memory = value;
}

/* ===== Functions ===== */

void Error(const char* message, ...);

void PatchNOP(uintptr_t address, int count = 1);

// !!! ASM FUNCTIONS ONLY !!!
// NOTE: The jump instruction is 5 bytes
void PatchJMP(uintptr_t address, void* asm_func, size_t patch_len = 5);

void Unpatch(uintptr_t address, bool low_priority = false);