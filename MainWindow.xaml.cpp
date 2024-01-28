#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

namespace winrt::hyzjkz::implementation {
    MainWindow::MainWindow() {
        InitializeComponent();

        // 设置自定义标题栏
        ExtendsContentIntoTitleBar(true);
        SetTitleBar(SP_TitleBar());

        // 加载图标
        Image_Icon().Source(util::BinToBMP(Global.res_icon, { }, true));

        // 设置页面标签
        auto nv_main{ NV_Main() };
        NVI_Settings().Tag(box_value(UpdateFlag::SETTINGS));
        NVI_History().Tag(box_value(UpdateFlag::HISTORY));
        NVI_Template().Tag(box_value(UpdateFlag::TEMPLATE));
        NVI_Tools().Tag(box_value(UpdateFlag::TOOLS));
        NVI_Record().Tag(box_value(UpdateFlag::RECORD));

        // 首页为历史记录
        nv_main.SelectedItem(NVI_History());
        NavigateToPublicPage(UpdateFlag::HISTORY);
    }

    // 导航栏切换
    F_EVENT void MainWindow::NV_Main_ItemInvoked(Controls::NavigationView const&, Controls::NavigationViewItemInvokedEventArgs const& args) {
        // 切换对应的页面
        auto flagID{ args.InvokedItemContainer().Tag().as<mqui64>() };
        if (flagID != NV_Main().Tag().as<mqui64>()) {
            NavigateToPublicPage(flagID);
        }
    }

    // 连接相机
    F_EVENT Windows::Foundation::IAsyncAction MainWindow::AttachCamera_Click(IInspectable const&, RoutedEventArgs const&) {
        MasterQian::Storage::Path eos_path{ Global.cfg.get(GlobalConfig::EOS_PATH) };
        if (eos_path.IsFile()) {
            MasterQian::System::Process::Execute(eos_path, L"", false, true);
        }
        else {
            co_await ShowTipDialog(util::RDString<hstring>(L"MainWindow.Tip.NoEOSPath"));
        }
    }
}

namespace winrt::hyzjkz::implementation {
    // 提示对话框
    F_RT Windows::Foundation::IAsyncAction MainWindow::ShowTipDialog(hstring const& msg) {
        auto dialog{ TipDialog() };
        dialog.Title(box_value(msg));
        co_await dialog.ShowAsync();
    }

    // 确认对话框
    F_RT  Windows::Foundation::IAsyncOperation<bool> MainWindow::ShowConfirmDialog(hstring const& msg) {
        auto dialog{ ConfirmDialog() };
        dialog.Title(box_value(msg));
        auto result{ co_await dialog.ShowAsync() };
        co_return result == Controls::ContentDialogResult::Primary;
    }

    // 输入对话框
    F_RT Windows::Foundation::IAsyncOperation<hstring> MainWindow::ShowInputDialog(hstring const& msg) {
        auto dialog{ InputDialog() };
        dialog.Title(box_value(msg));
        auto textbox{ dialog.Content().as<Controls::TextBox>() };
        textbox.Text(L"");
        auto result{ co_await dialog.ShowAsync() };
        co_return result == Controls::ContentDialogResult::Primary ? textbox.Text() : L"";
    }

    // 密码对话框
    F_RT Windows::Foundation::IAsyncAction MainWindow::ShowPasswordDialog() {
        auto dialog{ PasswordDialog() };
        dialog.Content().as<Controls::PasswordBox>().Password(L"");
        dialog.PrimaryButtonClick([ ] (Controls::ContentDialog const& dialog, Controls::ContentDialogButtonClickEventArgs const& args) {
            if (dialog.Content().as<Controls::PasswordBox>().Password() !=
                Global.cfg.get(GlobalConfig::PASSWORD, GlobalDefault::PASSWORD)) {
                args.Cancel(true);
            }
            });
        auto result{ Controls::ContentDialogResult::None };
        while (result != Controls::ContentDialogResult::Primary) {
            result = co_await dialog.ShowAsync();
        }
    }

    // 切换公开页
    F_RT void MainWindow::NavigateToPublicPage(mqui64 flagID) {
        Windows::UI::Xaml::Interop::TypeName name;
        switch (flagID) {
        case UpdateFlag::HISTORY:
            name = xaml_typename<hyzjkz::HistoryPage>();
            break;
        case UpdateFlag::TEMPLATE:
            name = xaml_typename<hyzjkz::TemplatePage>();
            break;
        case UpdateFlag::TOOLS:
            name = xaml_typename<hyzjkz::ToolsPage>();
            break;
        case UpdateFlag::RECORD:
            name = xaml_typename<hyzjkz::RecordPage>();
            break;
        case UpdateFlag::SETTINGS:
            name = xaml_typename<hyzjkz::SettingsPage>();
            break;
        default:
            return;
        }
        NV_Main().Tag(box_value(flagID));
        auto flag{ mUpdateFlag.flags[flagID].value };
        mUpdateFlag.flags[flagID] = { };
        Frame_Main().Navigate(name, box_value(flag));
    }

    // 切换私有页
    F_RT void MainWindow::NavigateToPrivatePage(mqui64 flagID, IInspectable const& args) {
        Windows::UI::Xaml::Interop::TypeName name;
        switch (flagID) {
        case UpdateFlag::PRIVATE_PRINT:
            name = xaml_typename<hyzjkz::PrintPage>();
            break;
        default:
            return;
        }
        auto nv_main{ NV_Main() };
        nv_main.SelectedItem(nullptr);
        nv_main.Tag(box_value(flagID));
        Frame_Main().Navigate(name, args);
    }

    // 更新标志
    F_RT void MainWindow::UpdateFlags(mqui64 page_index, mqui64 flag_index) {
        mUpdateFlag.flags[page_index].flags[flag_index] = true;
    }
}