#include "pch.h"
#include "SettingsPage.xaml.h"
#if __has_include("SettingsPage.g.cpp")
#include "SettingsPage.g.cpp"
#endif

namespace winrt::hyzjkz::implementation {
	/*  关于页面  */

	// 初始化
	static void InitializeAboutSubPage(SubPageBase* subPage) noexcept {
		auto sp{ subPage->to<SettingsPage::AboutSubPage>() };
		Application::LoadComponent(*sp, Windows::Foundation::Uri{ L"ms-appx:///Xaml/Settings/AboutSubPage.xaml" });
		sp->image_icon = sp->content().FindName(L"Image_Icon").as<Controls::Image>();
		sp->image_icon.Source(util::BinToBMP(Global.res_icon, { 
			static_cast<mqui32>(util::RDValue<mqf64>(L"About.Icon.Width")),
			static_cast<mqui32>(util::RDValue<mqf64>(L"About.Icon.Height")) }, true));
	}

	/*  路径页面  */

	// 工具链删除
	static Windows::Foundation::IAsyncAction ToolsDelete(IInspectable const& sender, RoutedEventArgs const&) noexcept {
		auto grid{ sender.as<FrameworkElement>().Parent().as<Controls::Grid>() };
		auto tool_name{ grid.FindName(L"Button_Name").as<Controls::Button>().Content().as<hstring>() };
		auto list_tools{ grid.Parent().as<Controls::ListView>() };
		auto result{ co_await Global.ui_window->as<hyzjkz::MainWindow>()
			.ShowConfirmDialog(util::RDString<hstring>(L"PathSubPage.Tip.ConfirmDeleleLink") + tool_name) };
		if (result) {
			// 更新变量与文件
			auto tools{ Global.cfg.get_config(GlobalConfig::TOOLS) };
			tools.remove(tool_name);
			Global.cfg.set(GlobalConfig::TOOLS, tools);
			Global.c_configFilePath.Write(Global.cfg.save());

			// 更新UI
			auto tool_items{ list_tools.Items() };
			for (mqui32 index{ }, len{ tool_items.Size() }; index < len; ++index) {
				if (tool_items.GetAt(index) == grid) {
					tool_items.RemoveAt(index);
					break;
				}
			}

			// 更新回调
			Global.ui_window->as<hyzjkz::MainWindow>().UpdateFlags(UpdateFlag::HISTORY, UpdateFlag::LINK_MENU);
		}
	}

	// 工具链修改名称
	static Windows::Foundation::IAsyncAction ToolsSetName(IInspectable const& sender, RoutedEventArgs const&) noexcept {
		auto button_name{ sender.as<Controls::Button>() };
		auto old_tool_name{ button_name.Content().as<hstring>() };
		hstring tool_name{ co_await Global.ui_window->as<hyzjkz::MainWindow>()
			.ShowInputDialog(util::RDString<hstring>(L"PathSubPage.Tip.InputLinkName")) };
		if (!tool_name.empty() && old_tool_name != tool_name) {
			auto tools{ Global.cfg.get_config(GlobalConfig::TOOLS) };
			if (!tools.contains(tool_name)) {
				// 更新变量与文件
				auto tool_path{ tools.get(old_tool_name) };
				tools.set(tool_name, tool_path);
				tools.remove(old_tool_name);
				Global.cfg.set(GlobalConfig::TOOLS, tools);
				Global.c_configFilePath.Write(Global.cfg.save());
				// 更新UI
				button_name.Content(box_value(tool_name));
				// 更新回调
				Global.ui_window->as<hyzjkz::MainWindow>().UpdateFlags(UpdateFlag::HISTORY, UpdateFlag::LINK_MENU);
			}
			else {
				co_await Global.ui_window->as<hyzjkz::MainWindow>()
					.ShowTipDialog(util::RDString<hstring>(L"PathSubPage.Tip.LinkExists") + tool_name);
			}
		}
	}

	// 工具链修改路径
	static Windows::Foundation::IAsyncAction ToolsSetPath(IInspectable const& sender, RoutedEventArgs const&) noexcept {
		auto button_path{ sender.as<Controls::Button>() };
		auto button_name{ button_path.Parent().as<Controls::Grid>().FindName(L"Button_Name").as<Controls::Button>() };

		Windows::Storage::Pickers::FileOpenPicker openPicker;
		openPicker.SuggestedStartLocation(Windows::Storage::Pickers::PickerLocationId::Desktop);
		openPicker.FileTypeFilter().Append(L".exe");
		util::InitializeDialog(openPicker, Global.ui_hwnd);

		if (Windows::Storage::StorageFile file{ co_await openPicker.PickSingleFileAsync() }; file != nullptr) {
			// 更新变量与文件
			std::wstring tool_name{ button_name.Content().as<hstring>() };
			std::wstring tool_path{ file.Path() };
			auto tools{ Global.cfg.get_config(GlobalConfig::TOOLS) };
			tools.set(tool_name, tool_path);
			Global.cfg.set(GlobalConfig::TOOLS, tools);
			Global.c_configFilePath.Write(Global.cfg.save());
			// 更新UI
			button_path.Content(box_value(tool_path));
			// 更新回调
			Global.ui_window->as<hyzjkz::MainWindow>().UpdateFlags(UpdateFlag::HISTORY, UpdateFlag::LINK_MENU);
		}
	}

	// 生成外链项
	static Controls::Grid MakeToolItemGrid(std::wstring_view tool_name, std::wstring_view tool_path) noexcept {
		Controls::Grid grid;
		Application::LoadComponent(grid, Windows::Foundation::Uri{ L"ms-appx:///Xaml/Settings/PathSubPagePathItem.xaml" });
		auto button_delete{ grid.FindName(L"Button_Delete").as<Controls::Button>() };
		button_delete.Click(&ToolsDelete);
		auto button_name{ grid.FindName(L"Button_Name").as<Controls::Button>() };
		button_name.Content(box_value(tool_name));
		button_name.Click(&ToolsSetName);
		auto button_path{ grid.FindName(L"Button_Path").as<Controls::Button>() };
		button_path.Content(box_value(tool_path));
		button_path.Click(&ToolsSetPath);
		return grid;
	}

	// 工具链添加
	static Windows::Foundation::IAsyncAction ToolsAdd(SettingsPage::PathSubPage* sp) noexcept {
		Windows::Storage::Pickers::FileOpenPicker openPicker;
		openPicker.SuggestedStartLocation(Windows::Storage::Pickers::PickerLocationId::Desktop);
		openPicker.FileTypeFilter().Append(L".exe");
		util::InitializeDialog(openPicker, Global.ui_hwnd);

		if (Windows::Storage::StorageFile file{ co_await openPicker.PickSingleFileAsync() }; file != nullptr) {
			std::wstring tool_name{ file.Name() };
			std::wstring tool_path{ file.Path() };

			auto tools{ Global.cfg.get_config(GlobalConfig::TOOLS) };
			if (!tools.contains(tool_name)) {
				// 更新变量与文件
				tools.set(tool_name, tool_path);
				Global.cfg.set(GlobalConfig::TOOLS, tools);
				Global.c_configFilePath.Write(Global.cfg.save());
				// 更新UI
				sp->list_tools.Items().Append(MakeToolItemGrid(tool_name, tool_path));
				// 更新回调
				Global.ui_window->as<hyzjkz::MainWindow>().UpdateFlags(UpdateFlag::HISTORY, UpdateFlag::LINK_MENU);
			}
			else {
				co_await Global.ui_window->as<hyzjkz::MainWindow>()
					.ShowTipDialog(util::RDString<hstring>(L"PathSubPage.Tip.LinkExists") + tool_name);
			}
		}
	}

	// 初始化
	static void InitializePathSubPage(SubPageBase* subPage) noexcept {
		auto sp{ subPage->to<SettingsPage::PathSubPage>() };
		Application::LoadComponent(*sp, Windows::Foundation::Uri{ L"ms-appx:///Xaml/Settings/PathSubPage.xaml" });
		auto panel{ sp->content() };
		// EOS路径
		sp->button_eospath_set = panel.FindName(L"Button_EOSPath_Set").as<Controls::Button>();
		sp->button_eospath_clear = panel.FindName(L"Button_EOSPath_Clear").as<Controls::Button>();
		sp->infobar_eospath = panel.FindName(L"InfoBar_EOSPath").as<Controls::InfoBar>();
		sp->input_eospath = panel.FindName(L"Input_EOSPath").as<Controls::TextBox>();
		sp->button_eospath_set.Click([sp] (auto, auto) -> Windows::Foundation::IAsyncAction {
			Windows::Storage::Pickers::FileOpenPicker openPicker;
			openPicker.SuggestedStartLocation(Windows::Storage::Pickers::PickerLocationId::Desktop);
			openPicker.FileTypeFilter().Append(L".exe");
			util::InitializeDialog(openPicker, Global.ui_hwnd);

			if (Windows::Storage::StorageFile file{ co_await openPicker.PickSingleFileAsync() }; file != nullptr) {
				sp->input_eospath.Text(file.Path());
				sp->infobar_eospath.Severity(Controls::InfoBarSeverity::Warning);
				Global.cfg.set(GlobalConfig::EOS_PATH, file.Path());
				Global.c_configFilePath.Write(Global.cfg.save());
			}
			});
		sp->button_eospath_clear.Click([sp] (auto, auto) {
			if (!sp->input_eospath.Text().empty()) {
				sp->input_eospath.Text(L"");
				sp->infobar_eospath.Severity(Controls::InfoBarSeverity::Error);
				Global.cfg.set(GlobalConfig::EOS_PATH, L"");
				Global.c_configFilePath.Write(Global.cfg.save());
			}
			});
		// 工具链
		sp->button_tools_add = panel.FindName(L"Button_Tools_Add").as<Controls::Button>();
		sp->list_tools = panel.FindName(L"List_Tools").as<Controls::ListView>();
		sp->button_tools_add.Click([sp] (auto, auto) -> Windows::Foundation::IAsyncAction {
			co_await ToolsAdd(sp);
			});
		auto tool_items{ sp->list_tools.Items() };
		auto tools{ Global.cfg.get_config(GlobalConfig::TOOLS) };
		for (auto& [tool_name, _] : tools) {
			tool_items.Append(MakeToolItemGrid(tool_name, tools.get(tool_name)));
		}
	}

	// 更新
	static void UpdatePathSubPage(SubPageBase* subPage) noexcept {
		auto sp{ subPage->to<SettingsPage::PathSubPage>() };
		// EOS路径
		auto eos_path{ Global.cfg.get(GlobalConfig::EOS_PATH, GlobalDefault::EOS_PATH) };
		sp->input_eospath.Text(eos_path);
		sp->infobar_eospath.Severity(eos_path.empty() ? Controls::InfoBarSeverity::Error : Controls::InfoBarSeverity::Warning);
	}

	/*  营业页面  */

	// 初始化
	static void InitializeBusinessSubPage(SubPageBase* subPage) noexcept {
		auto sp{ subPage->to<SettingsPage::BusinessSubPage>() };
		Application::LoadComponent(*sp, Windows::Foundation::Uri{ L"ms-appx:///Xaml/Settings/BusinessSubPage.xaml" });
		auto panel{ sp->content() };
		// 单价
		sp->button_unitprice = panel.FindName(L"Button_UnitPrice").as<Controls::Button>();
		sp->input_unitprice = panel.FindName(L"Input_UnitPrice").as<Controls::NumberBox>();
		sp->button_unitprice.Click([sp] (auto, auto) -> Windows::Foundation::IAsyncAction {
			Global.cfg.set(GlobalConfig::UNIT_PRICE, static_cast<GlobalType::UNIT_PRICE>(sp->input_unitprice.Value()));
			Global.c_configFilePath.Write(Global.cfg.save());
			co_await Global.ui_window->as<hyzjkz::MainWindow>()
				.ShowTipDialog(util::RDString<hstring>(L"PathSubPage.Tip.SaveSuccess"));
			});
		// 清理周期
		sp->button_resetmonth = panel.FindName(L"Button_ResetMonth").as<Controls::Button>();
		sp->input_resetmonth = panel.FindName(L"Input_ResetMonth").as<Controls::NumberBox>();
		sp->button_resetmonth.Click([sp] (auto, auto) -> Windows::Foundation::IAsyncAction {
			Global.cfg.set(GlobalConfig::RESET_MONTH, static_cast<GlobalType::RESET_MONTH>(sp->input_resetmonth.Value()));
			Global.c_configFilePath.Write(Global.cfg.save());
			co_await Global.ui_window->as<hyzjkz::MainWindow>()
				.ShowTipDialog(util::RDString<hstring>(L"PathSubPage.Tip.SaveSuccess"));
			});
		// 显示营业额
		sp->switch_showturnover = panel.FindName(L"Switch_ShowTurnover").as<Controls::ToggleSwitch>();
		sp->switch_showturnover.Toggled([sp] (auto, auto) {
			Global.cfg.set(GlobalConfig::SHOW_TURNOVER, sp->switch_showturnover.IsOn());
			Global.c_configFilePath.Write(Global.cfg.save());
			Global.ui_window->as<hyzjkz::MainWindow>().UpdateFlags(UpdateFlag::HISTORY, UpdateFlag::SHOW_TURNOVER);
			});
		// 管理员密码
		sp->button_password = panel.FindName(L"Button_Password").as<Controls::Button>();
		sp->input_password = panel.FindName(L"Input_Password").as<Controls::TextBox>();
		sp->switch_password = panel.FindName(L"Switch_Password").as<Controls::ToggleSwitch>();
		sp->switch_password.Toggled([sp] (auto, auto) {
			Global.cfg.set(GlobalConfig::USE_PASSWORD, sp->switch_password.IsOn());
			Global.c_configFilePath.Write(Global.cfg.save());
			});
		sp->button_password.Click([sp] (auto, auto) -> Windows::Foundation::IAsyncAction {
			auto pwd{ sp->input_password.Text() };
			if (pwd.empty()) {
				sp->input_password.Text(Global.cfg.get(GlobalConfig::PASSWORD, GlobalDefault::PASSWORD));
				co_await Global.ui_window->as<hyzjkz::MainWindow>()
					.ShowTipDialog(util::RDString<hstring>(L"PathSubPage.Tip.PasswordEmpty"));
			}
			else {
				Global.cfg.set(GlobalConfig::PASSWORD, pwd);
				Global.c_configFilePath.Write(Global.cfg.save());
				co_await Global.ui_window->as<hyzjkz::MainWindow>()
					.ShowTipDialog(util::RDString<hstring>(L"PathSubPage.Tip.SaveSuccess"));
			}
			});
	}

	// 更新
	static void UpdateBusinessSubPage(SubPageBase* subPage) noexcept {
		auto sp{ subPage->to<SettingsPage::BusinessSubPage>() };
		// 单价
		sp->input_unitprice.Value(static_cast<mqf64>(Global.cfg.get<GlobalType::UNIT_PRICE>(GlobalConfig::UNIT_PRICE, GlobalDefault::UNIT_PRICE)));
		// 清理周期
		sp->input_resetmonth.Value(static_cast<mqf64>(Global.cfg.get<GlobalType::RESET_MONTH>(GlobalConfig::RESET_MONTH, GlobalDefault::RESET_MONTH)));
		// 显示营业额
		sp->switch_showturnover.IsOn(Global.cfg.get<GlobalType::SHOW_TURNOVER>(GlobalConfig::SHOW_TURNOVER, GlobalDefault::SHOW_TURNOVER));
		// 管理员密码
		sp->switch_password.IsOn(Global.cfg.get<GlobalType::USE_PASSWORD>(GlobalConfig::USE_PASSWORD, GlobalDefault::USE_PASSWORD));
		sp->input_password.Text(Global.cfg.get(GlobalConfig::PASSWORD, GlobalDefault::PASSWORD));
	}

	/*  打印页面  */

	// 初始化
	static void InitializePrintSubPage(SubPageBase* subPage) noexcept {
		auto sp{ subPage->to<SettingsPage::PrintSubPage>() };
		Application::LoadComponent(*sp, Windows::Foundation::Uri{ L"ms-appx:///Xaml/Settings/PrintSubPage.xaml" });
		auto panel{ sp->content() };
		// 打印机名称
		sp->button_printername = panel.FindName(L"Button_PrinterName").as<Controls::Button>();
		sp->input_printername = panel.FindName(L"Input_PrinterName").as<Controls::TextBox>();
		sp->infobar_printername = panel.FindName(L"InfoBar_PrinterName").as<Controls::InfoBar>();
		sp->button_printername.Click([sp] (auto, auto) -> Windows::Foundation::IAsyncAction {
			Global.cfg.set(GlobalConfig::PRINTER_NAME, sp->input_printername.Text());
			Global.c_configFilePath.Write(Global.cfg.save());
			co_await Global.ui_window->as<hyzjkz::MainWindow>()
				.ShowTipDialog(util::RDString<hstring>(L"PathSubPage.Tip.SaveSuccess"));
			});
		// 自动裁剪容忍误差
		sp->button_autocuteps = panel.FindName(L"Button_AutoCutEPS").as<Controls::Button>();
		sp->input_autocuteps = panel.FindName(L"Input_AutoCutEPS").as<Controls::NumberBox>();
		sp->button_autocuteps.Click([sp] (auto, auto)-> Windows::Foundation::IAsyncAction {
			Global.cfg.set(GlobalConfig::AUTOCUT_EPS, sp->input_autocuteps.Value());
			Global.c_configFilePath.Write(Global.cfg.save());
			co_await Global.ui_window->as<hyzjkz::MainWindow>()
				.ShowTipDialog(util::RDString<hstring>(L"PathSubPage.Tip.SaveSuccess"));
			});
	}

	// 更新
	static void UpdatePrintSubPage(SubPageBase* subPage) noexcept {
		auto sp{ subPage->to<SettingsPage::PrintSubPage>() };
		// 打印机名称
		auto printer_name{ Global.cfg.get(GlobalConfig::PRINTER_NAME, GlobalDefault::PRINTER_NAME) };
		sp->input_printername.Text(printer_name);
		sp->infobar_printername.Severity(printer_name.empty() ? Controls::InfoBarSeverity::Error : Controls::InfoBarSeverity::Informational);
		// 自动裁剪容忍误差
		sp->input_autocuteps.Value(Global.cfg.get(GlobalConfig::AUTOCUT_EPS, GlobalDefault::AUTOCUT_EPS));
	}
}

namespace winrt::hyzjkz::implementation {
	SettingsPage::SettingsPage() {
		using Args = MasterQian::WinRT::Args;
		InitializeComponent();
		NVI_Home().Tag(Args::box(&About, nullptr));
		InitializeAboutSubPage(&About);
		NVI_Path().Tag(Args::box(&Path, &UpdatePathSubPage));
		InitializePathSubPage(&Path);
		NVI_Business().Tag(Args::box(&Business, &UpdateBusinessSubPage));
		InitializeBusinessSubPage(&Business);
		NVI_Print().Tag(Args::box(&Print, &UpdatePrintSubPage));
		InitializePrintSubPage(&Print);

		Loaded([this] (auto, auto) -> Windows::Foundation::IAsyncAction {
			auto nv_main{ NV_Main() };
			if (Global.cfg.get<GlobalType::USE_PASSWORD>(GlobalConfig::USE_PASSWORD, GlobalDefault::USE_PASSWORD)) {
				nv_main.Visibility(Visibility::Collapsed);
				co_await Global.ui_window->as<hyzjkz::MainWindow>().ShowPasswordDialog();
				nv_main.Visibility(Visibility::Visible);
			}
			nv_main.SelectedItem(NVI_Home());
			});
	}

	// 更换导航
	F_EVENT void SettingsPage::NV_Main_SelectionChanged(Controls::NavigationView const&, Controls::NavigationViewSelectionChangedEventArgs const& args) {
		auto tag{ args.SelectedItem().as<Controls::NavigationViewItem>().Tag() };
		if (tag != nullptr) {
			auto args{ tag.as<MasterQian::WinRT::Args>() };
			auto [subPage, func] = args.unbox<SubPageBase*, UpdateSubPageFunc>();
			Frame_Main().Content(*subPage);
			if (func) {
				func(subPage);
			}
		}
	}
}