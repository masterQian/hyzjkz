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

// 运行时函数
#define F_RT
// 事件函数
#define F_EVENT

// 调试日志输出器
#ifdef _DEBUG
inline MasterQian::System::DebugLogger logger;
#endif

// 子页面
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

// 报表数据结构
struct RecordDayData {
	mqui16 num;
	mqui16 value;
};
using RecordMonthData = RecordDayData[31];
using RecordYearData = RecordMonthData[12];

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
	struct DATA {
		mqui32 left, top, width, height; // 左边，顶边，宽度，高度
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

// 模板列表
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

// 全局配置结构
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

// 全局配置类型
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

// 全局配置默认值
struct GlobalDefault {
	static constexpr auto EOS_PATH{ L"" };
	static constexpr auto PRINTER_NAME{ L"照相打印机" };
	static constexpr auto AUTOCUT_EPS{ 0.03 };
	static constexpr auto UNIT_PRICE{ 15U };
	static constexpr auto SHOW_TURNOVER{ false };
	static constexpr auto RESET_MONTH{ 1U };
	static constexpr auto PASSWORD{ L"011105" };
	static constexpr auto USE_PASSWORD{ false };
};

// 全局数据结构
inline struct GlobalStruct {
	GlobalConfig cfg; // 全局配置
	TemplateList templateList; // 模板列表

	// 全局常量
	MasterQian::Storage::Path c_runPath; // 运行路径
	MasterQian::Storage::Path c_photoPath; // 照片路径
	MasterQian::Storage::Path c_thumbPath; // 缓存路径
	MasterQian::Storage::Path c_templatePath; // 模板路径
	MasterQian::Storage::Path c_dataPath; // 数据路径
	MasterQian::Storage::Path c_configFilePath; // 配置路径
	MasterQian::Storage::Path c_tempPhotoPath; // 临时照片路径
	MasterQian::Storage::Path c_cameraPhotoPath; // 相机照片路径

	MasterQian::Media::ImageSize c_photoThumbSize; // 缩略图尺寸
	MasterQian::Media::ImageSize c_photoPreviewSize; // 预览图尺寸
	MasterQian::Media::ImageSize c_IDCardPreviewSize; // 身份证拼接预览尺寸

	// 全局资源
	MasterQian::BinView res_icon; // 图标
	MasterQian::BinView res_default_img; // 默认照片

	// UI
	HWND ui_hwnd{ }; // 窗口句柄
	winrt::Microsoft::UI::Xaml::Window* ui_window{ }; // 主窗口

	HANDLE file_spy_event; // 文件监控事件
}Global;

namespace util {
	// 字节集转图像
	winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage
		BinToBMP(MasterQian::BinView, MasterQian::Media::ImageSize size = { }, bool hasCache = false) noexcept;

	// 文件转图像
	winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage
		FileToBMP(std::wstring_view, MasterQian::Media::ImageSize size = { }, bool hasCache = false) noexcept;

	// 流转图像
	winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage
		StreamToBMP(mqhandle, MasterQian::Media::ImageSize size = { }, bool hasCache = false) noexcept;

	// 初始化对话框窗口
	void InitializeDialog(winrt::Windows::Foundation::IInspectable const&, HWND) noexcept;

	// 注册拖放钩子
	void SetDropHook(void (*callback)(mqhandle, std::vector<std::wstring> const&) noexcept, mqhandle) noexcept;

	// 读取资源字典
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
