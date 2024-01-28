#include "pch.h"
#include "app.h"

#include <shlwapi.h>
#include <shcore.h>
#include <shobjidl_core.h>

#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "shlwapi.lib")

using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::Foundation;

namespace util {
	constexpr GUID GuidIRandomAccessStream{ 0x905a0fe1, 0xbc53, 0x11df, { 0x8c, 0x49, 0x00, 0x1e, 0x4f, 0xc6, 0x86, 0xda } };

	Media::Imaging::BitmapImage BinToBMP(MasterQian::BinView bin, MasterQian::Media::ImageSize size, bool hasCache) noexcept {
		IStream* stream{ SHCreateMemStream(bin.data(), bin.size32()) };
		Streams::IRandomAccessStream ras;
		CreateRandomAccessStreamOverStream(stream, BSOS_OPTIONS::BSOS_DEFAULT, GuidIRandomAccessStream, reinterpret_cast<void**>(&ras));
		stream->Release();
		Media::Imaging::BitmapImage bmp;
		if (!hasCache) bmp.CreateOptions(Media::Imaging::BitmapCreateOptions::IgnoreImageCache);
		if (size.width) bmp.DecodePixelWidth(size.width);
		if (size.height) bmp.DecodePixelHeight(size.height);
		bmp.SetSource(ras);
		return bmp;
	}

	Media::Imaging::BitmapImage FileToBMP(std::wstring_view fn, MasterQian::Media::ImageSize size, bool hasCache) noexcept {
		Media::Imaging::BitmapImage bmp;
		if (!hasCache) bmp.CreateOptions(Media::Imaging::BitmapCreateOptions::IgnoreImageCache);
		if (size.width) bmp.DecodePixelWidth(size.width);
		if (size.height) bmp.DecodePixelHeight(size.height);
		bmp.UriSource(Uri{ fn });
		return bmp;
	}

	winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage
		StreamToBMP(mqhandle handle, MasterQian::Media::ImageSize size, bool hasCache) noexcept {
		auto stream{ static_cast<IStream*>(handle) };
		Streams::IRandomAccessStream ras;
		CreateRandomAccessStreamOverStream(stream, BSOS_OPTIONS::BSOS_DEFAULT, GuidIRandomAccessStream, reinterpret_cast<void**>(&ras));
		stream->Release(); // Ω”π‹UnsafeStream Õ∑≈
		Media::Imaging::BitmapImage bmp;
		if (!hasCache) bmp.CreateOptions(Media::Imaging::BitmapCreateOptions::IgnoreImageCache);
		if (size.width) bmp.DecodePixelWidth(size.width);
		if (size.height) bmp.DecodePixelHeight(size.height);
		bmp.SetSource(ras);
		return bmp;
	}

	void InitializeDialog(winrt::Windows::Foundation::IInspectable const& dialog, HWND hwnd) noexcept {
		dialog.as<IInitializeWithWindow>()->Initialize(hwnd);
	}

	mqhandle drop_hook_handle{ };
	void (*drop_hook_callback)(mqhandle, std::vector<std::wstring> const&) noexcept {};

	static LRESULT CALLBACK DropFileHookCallBack(INT code, WPARAM wParam, LPARAM lParam) {
		PMSG pcs{ reinterpret_cast<PMSG>(lParam) };
		if (pcs->message == WM_DROPFILES) {
			HDROP hDropInfo{ reinterpret_cast<HDROP>(pcs->wParam) };
			mqui32 nFileCount{ DragQueryFileW(hDropInfo, static_cast<mqui32>(-1), nullptr, 0U) };
			mqchar szFileName[MAX_PATH]{ };
			std::vector<std::wstring> files(nFileCount);
			for (mqui32 i{ }; i < nFileCount; ++i) {
				DragQueryFileW(hDropInfo, i, szFileName, MAX_PATH);
				files.emplace_back(szFileName);
			}
			DragFinish(hDropInfo);
			drop_hook_callback(drop_hook_handle, files);
		}
		return CallNextHookEx(nullptr, code, wParam, lParam);
	}

	void SetDropHook(void (*callback)(mqhandle, std::vector<std::wstring> const&) noexcept, mqhandle arg) noexcept {
		drop_hook_handle = arg;
		drop_hook_callback = callback;
		SetWindowLongW(Global.ui_hwnd, GWL_EXSTYLE, GetWindowLongW(Global.ui_hwnd, GWL_EXSTYLE) | WS_EX_ACCEPTFILES);
		SetWindowsHookExW(WH_GETMESSAGE, &DropFileHookCallBack, nullptr, GetCurrentThreadId());;
	}
}