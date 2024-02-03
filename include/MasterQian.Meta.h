#pragma once

// ���뻷��
#if !defined(__cplusplus) || !defined(__cpp_concepts) || _MSVC_LANG < 202002L
#error "Module requires at least C++20 standard compiler."
#endif

#ifndef _WIN64
#error "Module requires Windows x64."
#endif

// ����Win32API
#define META_WINAPI(type, name, ...) extern __declspec(dllimport) type __stdcall name (__VA_ARGS__) noexcept

// ģ���ʼ��(������.h)
#define META_MODULE_BEGIN \
auto MasterQianModuleName(ModuleHandle) = []() noexcept { \
auto handle { api::LoadLibraryW(L"" MasterQianLibString) }; \
mqcstr errMsg { }; \
if (handle) { \
	using VerFuncType = mqui64(__stdcall*)(); \
	if(auto VerFunc { (VerFuncType)api::GetProcAddress(handle, "MasterQianVersion") }) { \
		if(auto ver{ VerFunc() }; ver != MasterQianModuleVersion) { \
			errMsg = L"Header version mismatched library \"" MasterQianLibString "\" version !"; \
		} \
	} \
	else { \
		errMsg = L"The library file \"" MasterQianLibString "\" is damaged!"; \
	} \
} \
else { \
	errMsg = L"Missing \"" MasterQianLibString "\" !"; \
} \
if (errMsg) { \
	api::MessageBoxW(nullptr, errMsg, L"MasterQian", 16U); \
	api::ExitProcess(static_cast<mqui32>(-1)); \
}

#define META_MODULE_END \
return handle; \
}();

// ģ�鵼��API����(������.h)
#define META_IMPORT_API(type, name, ...) type(__stdcall* MasterQianModuleName(name)) (__VA_ARGS__);
// ģ��API��ȡ(������.h)
#define META_PROC_API(name) MasterQianModuleName(name) = (decltype(MasterQianModuleName(name)))api::GetProcAddress(handle, MasterQianModuleNameString(name))
// ����API(������.cpp)
#define META_EXPORT_API(type, name, ...) extern "C" __declspec(dllexport) type __stdcall MasterQianModuleName(name) (__VA_ARGS__)
// �����汾API(������.cpp)
#define META_EXPORT_API_VERSION(ver) extern "C" __declspec(dllexport) mqui64 __stdcall MasterQianVersion() { return ver; }