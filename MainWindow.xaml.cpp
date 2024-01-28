#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

namespace winrt::hyzjkz::implementation {
    // [�첽] ��Ƭ����
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
            if (name.size() == 8ULL && name.substr(4ULL, 2ULL) == clear_month) { // ��20231231 -> 12
                // ɾ����Ƭ�ļ��м���Ӧ����ͼ�ļ���
                folder.Delete();
                Global.c_thumbPath.Concat(name).Delete();
            }
        }
    }

    // ��ʼ��ȫ��
    static void InitializeGlobal() noexcept {
        // ����GDI
        MasterQian::Media::GDI::StartupGDI();
        // �����ļ�����¼�
        Global.file_spy_event = CreateEventW(nullptr, true, false, nullptr);
        // ��Ƭת��
        auto date_str{ MasterQian::Timestamp{ }.local().formatDate() };
        auto photo_path{ Global.c_photoPath / date_str };
        auto thumb_path{ Global.c_thumbPath / date_str };
        for (auto& file : Global.c_cameraPhotoPath.EnumFolder(L"*.jpg")) {
            MasterQian::Media::GDI::Image thumb_image(file);
            thumb_image.Thumbnail(Global.c_photoThumbSize).Save(thumb_path / file.Name(), MasterQian::Media::ImageFormat::JPG);
            file.Move(photo_path);
        }
    }

    // ����ȫ��
    static void UninitializeGlobal(IInspectable const&, WindowEventArgs const&) noexcept {
        // ����ֹͣ�ļ�����¼�
        SetEvent(Global.file_spy_event);
        // �ر�GDI
        MasterQian::Media::GDI::ShutdownGDI();
    }
}

namespace winrt::hyzjkz::implementation {
    MainWindow::MainWindow() {
        InitializeComponent();

        // ��ȡ���ھ��
        auto appWindow{ AppWindow() };
        Global.ui_hwnd = reinterpret_cast<HWND>(appWindow.Id().Value);
        
        // ��ʼ��ȫ��
        InitializeGlobal();
        // �ر��¼� --- ����ȫ��
        Closed(&UninitializeGlobal);

        // �����Զ��������
        ExtendsContentIntoTitleBar(true);
        SetTitleBar(SP_TitleBar());

        // �������ڴ�С
        auto width{ static_cast<mqi32>(MasterQian::System::Info::GetScreenWidth()) };
        auto height{ static_cast<mqi32>(MasterQian::System::Info::GetScreenHeight()) };
        auto scale{ util::RDValue<mqf64>(L"MainWindow.Scale.Value") };
        auto actualWidth{ static_cast<mqi32>(width * scale) };
        auto actualHeight{ static_cast<mqi32>(height * scale) };
        auto actualLeft{ static_cast<mqi32>((1.0 - scale) * width / 2) };
        auto actualTop{ static_cast<mqi32>((1.0 - scale) * height / 2) };
        appWindow.MoveAndResize({ actualLeft, actualTop, actualWidth, actualHeight });

        // ����ͼ��
        Image_Icon().Source(util::BinToBMP(Global.res_icon, { }, true));
        
        // �첽����
        ClearPhotos();

        // ����ҳ���ǩ
        auto nv_main{ NV_Main() };
        NVI_Settings().Tag(box_value(UpdateFlag::SETTINGS));
        NVI_History().Tag(box_value(UpdateFlag::HISTORY));
        NVI_Template().Tag(box_value(UpdateFlag::TEMPLATE));
        NVI_Tools().Tag(box_value(UpdateFlag::TOOLS));
        NVI_Record().Tag(box_value(UpdateFlag::RECORD));

        // ��ҳΪ��ʷ��¼
        nv_main.SelectedItem(NVI_History());
        NavigateToPublicPage(UpdateFlag::HISTORY);
    }

    // �������л�
    F_EVENT void MainWindow::NV_Main_ItemInvoked(Controls::NavigationView const&, Controls::NavigationViewItemInvokedEventArgs const& args) {
        // �л���Ӧ��ҳ��
        auto flagID{ args.InvokedItemContainer().Tag().as<mqui64>() };
        if (flagID != NV_Main().Tag().as<mqui64>()) {
            NavigateToPublicPage(flagID);
        }
    }

    // �������
    F_EVENT Windows::Foundation::IAsyncAction MainWindow::AttachCamera_Click(IInspectable const&, RoutedEventArgs const&) {
        MasterQian::Storage::Path eos_path{ Global.cfg.get(GlobalConfig::EOS_PATH) };
        if (eos_path.IsFile()) {
            MasterQian::System::Process::Execute(eos_path, L"", false, true);
        }
        else {
            co_await ShowTipDialog(util::RDString<hstring>(L"MainWindow.Tip.NoEOSPath"));
        }
    }

    // ������
    F_EVENT void MainWindow::CheckUpdate_Click(IInspectable const&, RoutedEventArgs const&) {
        auto CurrentVersion{ util::RDString<hstring>(L"App.Version") };
        // ��������
        Close();
        // ������³���
        MasterQian::System::Process::Execute(L"AutoUpdate.exe", CurrentVersion, false, true);
    }
}

namespace winrt::hyzjkz::implementation {
    // ��ʾ�Ի���
    F_RT Windows::Foundation::IAsyncAction MainWindow::ShowTipDialog(hstring const& msg) {
        auto dialog{ TipDialog() };
        dialog.Title(box_value(msg));
        co_await dialog.ShowAsync();
    }

    // ȷ�϶Ի���
    F_RT  Windows::Foundation::IAsyncOperation<bool> MainWindow::ShowConfirmDialog(hstring const& msg) {
        auto dialog{ ConfirmDialog() };
        dialog.Title(box_value(msg));
        auto result{ co_await dialog.ShowAsync() };
        co_return result == Controls::ContentDialogResult::Primary;
    }

    // ����Ի���
    F_RT Windows::Foundation::IAsyncOperation<hstring> MainWindow::ShowInputDialog(hstring const& msg) {
        auto dialog{ InputDialog() };
        dialog.Title(box_value(msg));
        auto textbox{ dialog.Content().as<Controls::TextBox>() };
        textbox.Text(L"");
        auto result{ co_await dialog.ShowAsync() };
        co_return result == Controls::ContentDialogResult::Primary ? textbox.Text() : L"";
    }

    // ����Ի���
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

    // �л�����ҳ
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

    // �л�˽��ҳ
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

    // ���±�־
    F_RT void MainWindow::UpdateFlags(mqui64 page_index, mqui64 flag_index) {
        mUpdateFlag.flags[page_index].flags[flag_index] = true;
    }
}