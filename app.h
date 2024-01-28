#pragma once

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Media.Imaging.h>

#include <MasterQian.Media.GDI.h>
#include <MasterQian.Storage.Path.h>
#include <MasterQian.Parser.Config.h>
#ifdef _DEBUG
#include <MasterQian.System.Log.h>
#endif
#include <MasterQian.System.Info.h>
#include <MasterQian.System.Process.h>
#include <MasterQian.WinRT.h>

#include "resource.h"

// ����ʱ����
#define F_RT
// �¼�����
#define F_EVENT

// ������־�����
#ifdef _DEBUG
inline MasterQian::System::DebugLogger logger;
#endif

// ��ҳ��
struct SubPageBase : winrt::Windows::Foundation::IInspectable {
	template<std::derived_from<SubPageBase> T>
	T* to() noexcept {
		return reinterpret_cast<T*>(this);
	}
};

template<typename T>
struct SubPage : SubPageBase {
	SubPage() : SubPageBase{ T{ } } {}

	T content() const noexcept {
		return winrt::Windows::Foundation::IInspectable::as<T>();
	}
};

using UpdateSubPageFunc = void(*)(SubPageBase*) noexcept;

// �������ݽṹ
struct RecordDayData {
	mqui16 num;
	mqui16 value;
};
using RecordMonthData = RecordDayData[31];
using RecordYearData = RecordMonthData[12];

// ��ӡģ��
struct PrintTemplate {
	mqui32 size; // �����ƴ�С
	mqui32 count; // ��Ƭ����
	mqchar name[32]; // ģ������
	struct FLAG {
		bool canDelete; // �Ƿ�����ɾ��
		bool unused2; // δ��
		bool unused3; // δ��
		bool unused4; // δ��
		bool isRotate; // �Ƿ���ת
		bool isAutoCut; // �Ƿ��Զ��ü�
		bool unused7; // δ��
		bool unused8; // δ��
	}flag;
	mqui32 order; // ģ����
	mqbyte unused[28]; // δ��
	struct DATA {
		mqui32 left, top, width, height; // ��ߣ����ߣ���ȣ��߶�
	}data[1];

	static constexpr mqui32 CalcSize(mqui32 count) noexcept {
		if (count < 2U) {
			return sizeof(PrintTemplate);
		}
		else {
			return static_cast<mqui32>(sizeof(PrintTemplate) + static_cast<mqui64>(
				count - 1U) * sizeof(PrintTemplate::DATA));
		}
	}

	static constexpr bool IsCorrect(mqui32 size) noexcept {
		if (static_cast<mqui64>(size) < sizeof(PrintTemplate)) return false;
		return (static_cast<mqui64>(size) - sizeof(PrintTemplate)) % sizeof(PrintTemplate::DATA) == 0ULL;
	}
};

// ģ���б�
struct TemplateList : private std::unordered_map<std::wstring, MasterQian::Bin, MasterQian::Meta::isomerism_hash, MasterQian::Meta::isomerism_equal> {
	using BaseT = std::unordered_map<std::wstring, MasterQian::Bin, MasterQian::Meta::isomerism_hash, MasterQian::Meta::isomerism_equal>;
	
	TemplateList() = default;

	using BaseT::empty;
	using BaseT::size;
	using BaseT::begin;
	using BaseT::end;
	using BaseT::contains;
	using BaseT::erase;

	void add(std::wstring_view name, MasterQian::Bin&& bin) noexcept {
		BaseT::emplace(name, std::move(bin));
	}

	void set(std::wstring_view name, MasterQian::Bin&& bin) noexcept {
		if (auto iter{ BaseT::find(name) }; iter != BaseT::cend()) {
			iter->second = std::move(bin);
		}
		else {
			BaseT::emplace(name, std::move(bin));
		}
	}

	PrintTemplate& operator [] (std::wstring_view name) noexcept {
		return *reinterpret_cast<PrintTemplate*>(BaseT::find(name)->second.data());
	}

	mqbytes at(std::wstring_view name) noexcept {
		return BaseT::find(name)->second.data();
	}
};

// ��־ת����
union FlagConverter {
	mqui64 value;
	bool flags[8ULL]{ };
};

// ���±�־
struct UpdateFlag {
	// ˽��ҳ
	static constexpr auto PRIVATE_PRINT{ static_cast<mqui64>(-1) }; // ��ӡ

	// ����ҳ
	static constexpr auto SETTINGS{ 0ULL }; // ����


	static constexpr auto HISTORY{ 1ULL }; // ��ʷ��¼
	static constexpr auto SHOW_TURNOVER{ 0ULL };
	static constexpr auto LINK_MENU{ 1ULL };



	static constexpr auto TEMPLATE{ 2ULL }; // ģ��



	static constexpr auto TOOLS{ 3ULL }; // ����



	static constexpr auto RECORD{ 4ULL }; // ����


	FlagConverter flags[8ULL]{ };
};

// ȫ�����ýṹ
struct GlobalConfig : MasterQian::Parser::Config {
	static constexpr auto EOS_PATH{ L"eos_path" };
	static constexpr auto PRINTER_NAME{ L"printer_name" };
	static constexpr auto AUTOCUT_EPS{ L"autocut_eps" };
	static constexpr auto UNIT_PRICE{ L"unit_price" };
	static constexpr auto SHOW_TURNOVER{ L"show_turnover" };
	static constexpr auto RESET_MONTH{ L"reset_month" };
	static constexpr auto PASSWORD{ L"password" };
	static constexpr auto USE_PASSWORD{ L"use_password" };
	static constexpr auto TOOLS{ L"tools" };
};

// ȫ����������
struct GlobalType {
	using EOS_PATH = std::wstring_view;
	using PRINTER_NAME = std::wstring_view;
	using AUTOCUT_EPS = mqf64;
	using UNIT_PRICE = mqui32;
	using SHOW_TURNOVER = bool;
	using RESET_MONTH = mqui32;
	using PASSWORD = std::wstring_view;
	using USE_PASSWORD = bool;
	using TOOLS = MasterQian::Parser::Config;
};

// ȫ������Ĭ��ֵ
struct GlobalDefault {
	static constexpr auto EOS_PATH{ L"" };
	static constexpr auto PRINTER_NAME{ L"�����ӡ��" };
	static constexpr auto AUTOCUT_EPS{ 0.03 };
	static constexpr auto UNIT_PRICE{ 15U };
	static constexpr auto SHOW_TURNOVER{ false };
	static constexpr auto RESET_MONTH{ 1U };
	static constexpr auto PASSWORD{ L"011105" };
	static constexpr auto USE_PASSWORD{ false };
};

// ȫ�����ݽṹ
inline struct GlobalStruct {
	GlobalConfig cfg; // ȫ������
	TemplateList templateList; // ģ���б�

	// ȫ�ֳ���
	MasterQian::Storage::Path c_runPath; // ����·��
	MasterQian::Storage::Path c_photoPath; // ��Ƭ·��
	MasterQian::Storage::Path c_thumbPath; // ����·��
	MasterQian::Storage::Path c_templatePath; // ģ��·��
	MasterQian::Storage::Path c_dataPath; // ����·��
	MasterQian::Storage::Path c_configFilePath; // ����·��
	MasterQian::Storage::Path c_tempPhotoPath; // ��ʱ��Ƭ·��
	MasterQian::Storage::Path c_cameraPhotoPath; // �����Ƭ·��

	MasterQian::Media::ImageSize c_photoThumbSize; // ����ͼ�ߴ�
	MasterQian::Media::ImageSize c_photoPreviewSize; // Ԥ��ͼ�ߴ�
	MasterQian::Media::ImageSize c_IDCardPreviewSize; // ���֤ƴ��Ԥ���ߴ�

	// ȫ����Դ
	MasterQian::BinView res_icon; // ͼ��
	MasterQian::BinView res_default_img; // Ĭ����Ƭ

	// UI
	HWND ui_hwnd{ }; // ���ھ��
	winrt::Microsoft::UI::Xaml::Window* ui_window{ }; // ������

	HANDLE file_spy_event; // �ļ�����¼�
}Global;

namespace util {
	// �ֽڼ�תͼ��
	winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage
		BinToBMP(MasterQian::BinView, MasterQian::Media::ImageSize size = { }, bool hasCache = false) noexcept;

	// �ļ�תͼ��
	winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage
		FileToBMP(std::wstring_view, MasterQian::Media::ImageSize size = { }, bool hasCache = false) noexcept;

	// ��תͼ��
	winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage
		StreamToBMP(mqhandle, MasterQian::Media::ImageSize size = { }, bool hasCache = false) noexcept;

	// ��ʼ���Ի��򴰿�
	void InitializeDialog(winrt::Windows::Foundation::IInspectable const&, HWND) noexcept;

	// ע���ϷŹ���
	void SetDropHook(void (*callback)(mqhandle, std::vector<std::wstring> const&) noexcept, mqhandle) noexcept;

	// ��ȡ��Դ�ֵ�
	template<typename T, mqui32 N>
	inline T RD(winrt::hstring const& key) noexcept {
		auto rd{ winrt::Microsoft::UI::Xaml::Application::Current().Resources().MergedDictionaries().GetAt(N) };
		return rd.Lookup(winrt::box_value(key)).as<T>();
	}

	template<typename T>
	inline T RDString(winrt::hstring const& key) noexcept {
		return RD<T, 1U>(key);
	}

	template<typename T>
	inline T RDValue(winrt::hstring const& key) noexcept {
		return RD<T, 2U>(key);
	}

	template<typename T>
	inline T RDObject(winrt::hstring const& key) noexcept {
		return RD<T, 3U>(key);
	}
}
