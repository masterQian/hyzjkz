#pragma once

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Media.Imaging.h>

#include "resource.h"

import MasterQian.Media.GDI;
import MasterQian.Storage.Path;
import MasterQian.Storage.PDF;
import MasterQian.Parser.Config;
import MasterQian.System;
import MasterQian.WinRT;

// ����ʱ����
#define F_RT
// �¼�����
#define F_EVENT

// �������ݽṹ
struct RecordData {
	// ��������
	struct PhotoDayData {
		mqui16 num;
		mqui16 value;
	};
	using PhotoMonthData = PhotoDayData[31];
	using PhotoYearData = PhotoMonthData[12];
	// ��ӡ����
	using CopyDayData = mqui16;
	using CopyMonthData = CopyDayData[31];
	using CopyYearData = CopyMonthData[12];

	PhotoYearData photoData;
	CopyYearData copyData;

	static bool CheckSize(MasterQian::BinView bv) noexcept {
		return bv.size() == sizeof(RecordData);
	}

	static RecordData& From(MasterQian::Bin& bin) noexcept {
		return *reinterpret_cast<RecordData*>(bin.data());
	}

	void Set(std::integral auto month, std::integral auto day,
		std::integral auto num, std::integral auto value) noexcept {
		photoData[static_cast<mqui32>(month) - 1U][static_cast<mqui32>(day) - 1U]
			= { static_cast<mqui16>(num), static_cast<mqui16>(value) };
	}

	void Add(std::integral auto month, std::integral auto day, std::integral auto value) noexcept {
		copyData[static_cast<mqui32>(month) - 1U][static_cast<mqui32>(day) - 1U] += value;
	}
};

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
struct TemplateList : private std::unordered_map<std::wstring, MasterQian::Bin, MasterQian::freestanding::isomerism_hash, MasterQian::freestanding::isomerism_equal> {
	using BaseT = std::unordered_map<std::wstring, MasterQian::Bin, MasterQian::freestanding::isomerism_hash, MasterQian::freestanding::isomerism_equal>;
	
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

	MasterQian::Bin& get(std::wstring_view name) noexcept {
		return BaseT::find(name)->second;
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

template<typename T>
concept ConfigItem = requires() {
	typename T::Type;
	{ std::remove_cvref_t<decltype(T::name)>{} } -> std::same_as<std::wstring_view>;
	{ std::remove_cvref_t<decltype(T::def_value)>{} } -> std::same_as<typename T::Type>;
};

// ȫ�����ýṹ
struct GlobalConfig : private MasterQian::Parser::Config {
	template<ConfigItem T>
	auto Get() const noexcept { return get(T::name, T::def_value); }
	auto Get(std::wstring_view name) const noexcept { return get_config(name); }

	template<ConfigItem T>
	void Set(typename T::Type t = T::def_value) noexcept { set(T::name, t); }
	template<typename U, ConfigItem T>
	void Set(U u) noexcept { set(T::name, static_cast<T::Type>(u)); }
	void Set(std::wstring_view name, MasterQian::Parser::Config const& config) noexcept { set(name, config); }

	using MasterQian::Parser::Config::Config;
	using MasterQian::Parser::Config::load;
	using MasterQian::Parser::Config::save;

	struct EOS_PATH {
		using Type = std::wstring_view;
		static constexpr std::wstring_view name{ L"eos_path" };
		static constexpr Type def_value{ L"" };
	};

	struct PRINTER_NAME {
		using Type = std::wstring_view;
		static constexpr std::wstring_view name{ L"printer_name" };
		static constexpr Type def_value{ L"�����ӡ��" };
	};

	struct AUTOCUT_EPS {
		using Type = mqf64;
		static constexpr std::wstring_view name{ L"autocut_eps" };
		static constexpr Type def_value{ 0.03 };
	};

	struct UNIT_PRICE {
		using Type = mqui32;
		static constexpr std::wstring_view name{ L"unit_price" };
		static constexpr Type def_value{ 15U };
	};

	struct SHOW_TURNOVER {
		using Type = bool;
		static constexpr std::wstring_view name{ L"show_turnover" };
		static constexpr Type def_value{ false };
	};

	struct RESET_MONTH {
		using Type = mqui32;
		static constexpr std::wstring_view name{ L"reset_month" };
		static constexpr Type def_value{ 1U };
	};

	struct PASSWORD {
		using Type = std::wstring_view;
		static constexpr std::wstring_view name{ L"password" };
		static constexpr Type def_value{ L"011105" };
	};

	struct USE_PASSWORD {
		using Type = bool;
		static constexpr std::wstring_view name{ L"use_password" };
		static constexpr Type def_value{ false };
	};

	static constexpr std::wstring_view TOOLS{ L"tools" };
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

	mqsize c_photoThumbSize; // ����ͼ�ߴ�
	mqsize c_photoPreviewSize; // Ԥ��ͼ�ߴ�
	mqsize c_printCanvasSize; // ��ӡ�����ߴ�
	mqsize c_IDCardPreviewSize; // ���֤ƴ��Ԥ���ߴ�
	mqsize c_A4Size; // A4�ߴ�
	mqsize c_ToPDFImageSize; // ����PDF����ͼ��ߴ�

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
		BinToBMP(MasterQian::BinView, mqsize size = { }, bool hasCache = false) noexcept;

	// �ļ�תͼ��
	winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage
		FileToBMP(std::wstring_view, mqsize size = { }, bool hasCache = false) noexcept;

	// ��תͼ��
	winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage
		StreamToBMP(mqhandle, mqsize size = { }, bool hasCache = false) noexcept;

	// ��ʼ���Ի��򴰿�
	void InitializeDialog(winrt::Windows::Foundation::IInspectable const&, HWND) noexcept;

	// DateTimeתLocalTime
	inline MasterQian::Time DateTimeToLocal(winrt::Windows::Foundation::DateTime dt) noexcept {
		return MasterQian::Timestamp{ static_cast<mqui64>(dt.time_since_epoch().count()), MasterQian::Timestamp::Type::microsecond, true }.local();
	}

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
	inline T RDStyle(winrt::hstring const& key) noexcept {
		return RD<T, 3U>(key);
	}
}
