#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

namespace winrt::hyzjkz::implementation {
    // [异步] 照片清理
    static Windows::Foundation::IAsyncAction ClearPhotos() noexcept {
        using namespace MasterQian::Storage;

        co_await winrt::resume_background();

        wchar_t clear_month[3]{ L'0', L'0', L'\0' };
        auto month{ static_cast<mqui32>(MasterQian::Timestamp().local().month) };
        auto resetMonth{ Global.cfg.get<GlobalType::RESET_MONTH>(GlobalConfig::RESET_MONTH, 1U) };

        if (resetMonth == 0U || resetMonth >= 12U) resetMonth = 1U;
        if (month <= resetMonth) {
            month = month + 12U - resetMonth;
        }
        else {
            month = month - resetMonth;
        }
        if (month < 10) {
            clear_month[1] = L'0' + month;
        }
        else {
            clear_month[0] = L'1';
            clear_month[1] = L'0' + month - 10U;
        }

        for (auto& folder : Global.c_photoPath.EnumFolder()) {
            auto name{ folder.Name() };
            if (name.size() == 8ULL && name.substr(4ULL, 2ULL) == clear_month) { // 如20231231 -> 12
                // 删除照片文件夹及对应缩略图文件夹
                folder.Delete();
                Global.c_thumbPath.Concat(name).Delete();
            }
        }
    }

    // 初始化全局
    static void InitializeGlobal() noexcept {
        // 开启GDI
        MasterQian::Media::GDI::StartupGDI();
        // 创建文件监控事件
        Global.file_spy_event = CreateEventW(nullptr, true, false, nullptr);
        // 照片转储
        auto date_str{ MasterQian::Timestamp{ }.local().formatDate() };
        auto photo_path{ Global.c_photoPath / date_str };
        auto thumb_path{ Global.c_thumbPath / date_str };
        for (auto& file : Global.c_cameraPhotoPath.EnumFolder(L"*.jpg")) {
            MasterQian::Media::GDI::Image thumb_image(file);
            thumb_image.Thumbnail(Global.c_photoThumbSize).Save(thumb_path / file.Name(), MasterQian::Media::ImageFormat::JPG);
            file.Move(photo_path);
        }
    }

    // 清理全局
    static void UninitializeGlobal(IInspectable const&, WindowEventArgs const&) noexcept {
        // 发送停止文件监控事件
        SetEvent(Global.file_spy_event);
        // 关闭GDI
        MasterQian::Media::GDI::ShutdownGDI();
    }
}

namespace winrt::hyzjkz::implementation {
    MainWindow::MainWindow() {
        InitializeComponent();

        // 获取窗口句柄
        auto appWindow{ AppWindow() };
        Global.ui_hwnd = reinterpret_cast<HWND>(appWindow.Id().Value);
        
        // 初始化全局
        InitializeGlobal();
        // 关闭事件 --- 清理全局
        Closed(&UninitializeGlobal);

        // 设置自定义标题栏
        ExtendsContentIntoTitleBar(true);
        SetTitleBar(SP_TitleBar());

        // 调整窗口大小
        auto width{ static_cast<mqi32>(MasterQian::System::Info::GetScreenWidth()) };
        auto height{ static_cast<mqi32>(MasterQian::System::Info::GetScreenHeight()) };
        auto scale{ util::RDValue<mqf64>(L"MainWindow.Scale.Value") };
        auto actualWidth{ static_cast<mqi32>(width * scale) };
        auto actualHeight{ static_cast<mqi32>(height * scale) };
        auto actualLeft{ static_cast<mqi32>((1.0 - scale) * width / 2) };
        auto actualTop{ static_cast<mqi32>((1.0 - scale) * height / 2) };
        appWindow.MoveAndResize({ actualLeft, actualTop, actualWidth, actualHeight });

        // 加载图标
        Image_Icon().Source(util::BinToBMP(Global.res_icon, { }, true));
        
        // 异步任务
        ClearPhotos();

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

    // 检查更新
    F_EVENT void MainWindow::CheckUpdate_Click(IInspectable const&, RoutedEventArgs const&) {
        auto CurrentVersion{ util::RDString<hstring>(L"App.Version") };
        // 结束程序
        Close();
        // 唤起更新程序
        MasterQian::System::Process::Execute(L"AutoUpdate.exe", CurrentVersion, false, true);
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