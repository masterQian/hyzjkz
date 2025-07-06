#pragma once

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Media.Imaging.h>
#include <winrt/hyzjkz.h>
#include "MQControls/MQControls.Main.h"

#include "resource.h"

import MasterQian.Media.GDI;
import MasterQian.Storage.Path;
import MasterQian.Storage.PDF;
import MasterQian.Parser.Config;
import MasterQian.System;

using namespace MasterQian;
namespace Controls = winrt::Microsoft::UI::Xaml::Controls;
namespace Input = winrt::Microsoft::UI::Xaml::Input;
using Application = winrt::Microsoft::UI::Xaml::Application;
using RoutedEventArgs = winrt::Microsoft::UI::Xaml::RoutedEventArgs;
using FrameworkElement = winrt::Microsoft::UI::Xaml::FrameworkElement;
using Visibility = winrt::Microsoft::UI::Xaml::Visibility;
using Uri = winrt::Windows::Foundation::Uri;
using IAsyncAction = winrt::Windows::Foundation::IAsyncAction;
template<typename T>
using IAsyncOperation = winrt::Windows::Foundation::IAsyncOperation<T>;

// 运行时函数
#define F_RT
// 事件函数
#define F_EVENT

// 报表数据结构
struct RecordData {
	// 照相数据
	struct PhotoDayData {
		mqui16 num;
		mqui16 value;
	};
	using PhotoMonthData = PhotoDayData[31];
	using PhotoYearData = PhotoMonthData[12];
	// 复印数据
	using CopyDayData = mqui16;
	using CopyMonthData = CopyDayData[31];
	using CopyYearData = CopyMonthData[12];

	PhotoYearData photoData;
	CopyYearData copyData;

	static bool CheckSize(BinView bv) noexcept {
		return bv.size() == sizeof(RecordData);
	}

	static RecordData& From(Bin& bin) noexcept {
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

// 打印模板
struct PrintTemplate {
	mqui32 size; // 二进制大小
	mqui32 count; // 照片数量
	mqchar name[32]; // 模板名称
	struct FLAG {
		bool canDelete; // 是否允许删除
		bool unused2; // 未用
		bool unused3; // 未用
		bool unused4; // 未用
		bool isRotate; // 是否旋转
		bool isAutoCut; // 是否自动裁剪
		bool unused7; // 未用
		bool unused8; // 未用
	}flag;
	mqui32 order; // 模板序
	mqbyte unused[28]; // 未用
	mqrect data[1]; // 尺寸

	static constexpr mqui32 CalcSize(mqui32 count) noexcept {
		if (count < 2U) {
			return sizeof(PrintTemplate);
		}
		else {
			return static_cast<mqui32>(sizeof(PrintTemplate) + static_cast<mqui64>(
				count - 1U) * sizeof(mqrect));
		}
	}

	static constexpr bool IsCorrect(mqui32 size) noexcept {
		if (static_cast<mqui64>(size) < sizeof(PrintTemplate)) return false;
		return (static_cast<mqui64>(size) - sizeof(PrintTemplate)) % sizeof(mqrect) == 0ULL;
	}
};

// 模板列表
struct TemplateList : private std::unordered_map<std::wstring, Bin, freestanding::isomerism_hash, freestanding::isomerism_equal> {
	using BaseT = std::unordered_map<std::wstring, Bin, freestanding::isomerism_hash, freestanding::isomerism_equal>;
	
	TemplateList() = default;

	using BaseT::empty;
	using BaseT::size;
	using BaseT::begin;
	using BaseT::end;
	using BaseT::contains;
	using BaseT::erase;

	void add(std::wstring_view name, Bin&& bin) noexcept {
		BaseT::emplace(name, std::move(bin));
	}

	void set(std::wstring_view name, Bin&& bin) noexcept {
		if (auto iter{ BaseT::find(name) }; iter != BaseT::cend()) {
			iter->second = std::move(bin);
		}
		else {
			BaseT::emplace(name, std::move(bin));
		}
	}

	Bin& get(std::wstring_view name) noexcept {
		return BaseT::find(name)->second;
	}

	PrintTemplate& operator [] (std::wstring_view name) noexcept {
		return *reinterpret_cast<PrintTemplate*>(BaseT::find(name)->second.data());
	}

	mqbytes at(std::wstring_view name) noexcept {
		return BaseT::find(name)->second.data();
	}
};

// 标志转换器
union FlagConverter {
	mqui64 value;
	bool flags[8ULL]{ };
};

// 更新标志
struct UpdateFlag {
	// 私有页
	static constexpr auto PRIVATE_PRINT{ static_cast<mqui64>(-1) }; // 打印

	// 公共页
	static constexpr auto SETTINGS{ 0ULL }; // 设置


	static constexpr auto HISTORY{ 1ULL }; // 历史记录
	static constexpr auto SHOW_TURNOVER{ 0ULL };
	static constexpr auto LINK_MENU{ 1ULL };



	static constexpr auto TEMPLATE{ 2ULL }; // 模板



	static constexpr auto TOOLS{ 3ULL }; // 工具



	static constexpr auto RECORD{ 4ULL }; // 报表


	FlagConverter flags[8ULL]{ };
};

template<typename T>
concept ConfigItem = requires() {
	typename T::Type;
	{ std::remove_cvref_t<decltype(T::name)>{} } -> std::same_as<std::wstring_view>;
	{ std::remove_cvref_t<decltype(T::def_value)>{} } -> std::same_as<typename T::Type>;
};

// 全局配置结构
struct GlobalConfig : private Parser::Config {
	template<ConfigItem T>
	auto Get() const noexcept { return get(T::name, T::def_value); }
	auto Get(std::wstring_view name) const noexcept { return get_config(name); }

	template<ConfigItem T>
	void Set(typename T::Type t = T::def_value) noexcept { set(T::name, t); }
	template<typename U, ConfigItem T>
	void Set(U u) noexcept { set(T::name, static_cast<T::Type>(u)); }
	void Set(std::wstring_view name, Parser::Config const& config) noexcept { set(name, config); }

	using Parser::Config::Config;
	using Parser::Config::load;
	using Parser::Config::save;

	struct EOS_PATH {
		using Type = std::wstring_view;
		static constexpr std::wstring_view name{ L"eos_path" };
		static constexpr Type def_value{ L"" };
	};

	struct PRINTER_NAME {
		using Type = std::wstring_view;
		static constexpr std::wstring_view name{ L"printer_name" };
		static constexpr Type def_value{ L"照相打印机" };
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

// 全局数据结构
inline struct GlobalStruct {
	GlobalConfig cfg; // 全局配置
	TemplateList templateList; // 模板列表

	// 全局常量
	Storage::Path c_runPath; // 运行路径
	Storage::Path c_photoPath; // 照片路径
	Storage::Path c_thumbPath; // 缓存路径
	Storage::Path c_templatePath; // 模板路径
	Storage::Path c_dataPath; // 数据路径
	Storage::Path c_configFilePath; // 配置路径
	Storage::Path c_tempPhotoPath; // 临时照片路径
	Storage::Path c_cameraPhotoPath; // 相机照片路径

	mqsize c_photoThumbSize; // 缩略图尺寸
	mqsize c_photoPreviewSize; // 预览图尺寸
	mqsize c_printCanvasSize; // 打印画布尺寸
	mqsize c_IDCardPreviewSize; // 身份证拼接预览尺寸
	mqsize c_A4Size; // A4尺寸
	mqsize c_ToPDFImageSize; // 导出PDF导入图像尺寸

	// 全局资源
	BinView res_icon; // 图标
	BinView res_default_img; // 默认照片

	// UI
	HWND ui_hwnd{ }; // 窗口句柄
	winrt::hyzjkz::MainWindow ui_window{ nullptr }; // 主窗口

	HANDLE file_spy_event; // 文件监控事件
}Global;

namespace util {
	// 字节集转图像
	winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage
		BinToBMP(BinView, mqsize size = { }, bool hasCache = false) noexcept;

	// 文件转图像
	winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage
		FileToBMP(std::wstring_view, mqsize size = { }, bool hasCache = false) noexcept;

	// 流转图像
	winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage
		StreamToBMP(mqhandle, mqsize size = { }, bool hasCache = false) noexcept;

	// 初始化对话框窗口
	void InitializeDialog(winrt::Windows::Foundation::IInspectable const&, HWND) noexcept;

	// DateTime转LocalTime
	inline Time DateTimeToLocal(winrt::Windows::Foundation::DateTime dt) noexcept {
		return Timestamp{ static_cast<mqui64>(dt.time_since_epoch().count()), Timestamp::Type::microsecond, true }.local();
	}

	// 注册拖放钩子
	void SetDropHook(void (*callback)(mqhandle, std::vector<std::wstring> const&) noexcept, mqhandle) noexcept;

	// 读取资源字典
	template<typename T, mqui32 N>
	inline T RD(winrt::hstring const& key) noexcept {
		auto rd{ Application::Current().Resources().MergedDictionaries().GetAt(N) };
		return rd.Lookup(winrt::box_value(key)).as<T>();
	}

	template<typename T>
	inline T RDString(winrt::hstring const& key) noexcept {
		return RD<T, 2U>(key);
	}

	template<typename T>
	inline T RDValue(winrt::hstring const& key) noexcept {
		return RD<T, 3U>(key);
	}

	template<typename T>
	inline T RDStyle(winrt::hstring const& key) noexcept {
		return RD<T, 4U>(key);
	}
}
