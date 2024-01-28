#include "pch.h"
#include "App.xaml.h"
#include "MainWindow.xaml.h"

#include "app.h"

namespace winrt::hyzjkz::implementation {
    // ȡĬ������
    static MasterQian::Bin GetDefaultConfig() noexcept {
        MasterQian::Parser::Config config;
        config.set(GlobalConfig::EOS_PATH, GlobalDefault::EOS_PATH);
        config.set(GlobalConfig::PRINTER_NAME, GlobalDefault::PRINTER_NAME);
        config.set(GlobalConfig::AUTOCUT_EPS, GlobalDefault::AUTOCUT_EPS);
        config.set(GlobalConfig::UNIT_PRICE, GlobalDefault::UNIT_PRICE);
        config.set(GlobalConfig::SHOW_TURNOVER, GlobalDefault::SHOW_TURNOVER);
        config.set(GlobalConfig::RESET_MONTH, GlobalDefault::RESET_MONTH);
        config.set(GlobalConfig::PASSWORD, GlobalDefault::PASSWORD);
        config.set(GlobalConfig::USE_PASSWORD, GlobalDefault::USE_PASSWORD);
        MasterQian::Parser::Config tools;
        tools.set(L"��ͼ����", L"");
        tools.set(L"��Ӱħ����", L"");
        tools.set(L"PhotoShop", L"");
        config.set(GlobalConfig::TOOLS, tools);
        return config.save();
    }

    // ȡĬ�ϱ�������
    static MasterQian::Bin GetDefaultRecordData() noexcept {
        RecordYearData data{ };
        return MasterQian::Bin{ reinterpret_cast<mqcbytes>(&data), sizeof(RecordYearData) };
    }

    // ȡһ��ģ��
    static MasterQian::Bin GetTemplate1() noexcept {
        mqui32 count{ 10U };
        MasterQian::Bin bin(PrintTemplate::CalcSize(count));
        auto& pt{ *reinterpret_cast<PrintTemplate*>(bin.data()) };
        pt.size = sizeof(PrintTemplate);
        pt.count = count;
        pt.flag.canDelete = false;
        pt.flag.isRotate = false;
        pt.flag.isAutoCut = true;
        lstrcpyW(pt.name, L"һ��");
        pt.data[0] = { 80, 95, 295, 413 };
        pt.data[1] = { 406, 95, 295, 413 };
        pt.data[2] = { 732, 95, 295, 413 };
        pt.data[3] = { 1058, 95, 295, 413 };
        pt.data[4] = { 1384, 95, 295, 413 };
        pt.data[5] = { 100, 652, 295, 413 };
        pt.data[6] = { 406, 652, 295, 413 };
        pt.data[7] = { 732, 652, 295, 413 };
        pt.data[8] = { 1058, 652, 295, 413 };
        pt.data[9] = { 1384, 652, 295, 413 };
        return bin;
    }

    // ȡ����ģ��
    static MasterQian::Bin GetTemplate2() noexcept {
        mqui32 count{ 8U };
        MasterQian::Bin bin(PrintTemplate::CalcSize(count));
        auto& pt{ *reinterpret_cast<PrintTemplate*>(bin.data()) };
        pt.size = sizeof(PrintTemplate);
        pt.count = count;
        pt.flag.canDelete = false;
        pt.flag.isRotate = false;
        pt.flag.isAutoCut = true;
        lstrcpyW(pt.name, L"����");
        pt.data[0] = { 90, 0, 390, 567 };
        pt.data[1] = { 500, 0, 390, 567 };
        pt.data[2] = { 910, 0, 390, 567 };
        pt.data[3] = { 1320, 0, 390, 567 };
        pt.data[4] = { 90, 570, 390, 567 };
        pt.data[5] = { 500, 570, 390, 567 };
        pt.data[6] = { 910, 570, 390, 567 };
        pt.data[7] = { 1320, 570, 390, 567 };
        return bin;
    }

    // ȡ���ģ��
    static MasterQian::Bin GetTemplate5() noexcept {
        mqui32 count{ 1U };
        MasterQian::Bin bin(PrintTemplate::CalcSize(count));
        auto& pt{ *reinterpret_cast<PrintTemplate*>(bin.data()) };
        pt.size = sizeof(PrintTemplate);
        pt.count = count;
        pt.flag.canDelete = false;
        pt.flag.isRotate = true;
        pt.flag.isAutoCut = false;
        lstrcpyW(pt.name, L"���");
        pt.data[0] = { 120, 50, 1500, 1050 };
        return bin;
    }

    // ȡ�����ģ��
    static MasterQian::Bin GetTemplateMarry() noexcept {
        mqui32 count{ 4U };
        MasterQian::Bin bin(PrintTemplate::CalcSize(count));
        auto& pt{ *reinterpret_cast<PrintTemplate*>(bin.data()) };
        pt.size = sizeof(PrintTemplate);
        pt.count = count;
        pt.flag.canDelete = false;
        pt.flag.isRotate = false;
        pt.flag.isAutoCut = false;
        lstrcpyW(pt.name, L"�����");
        pt.data[0] = { 190, 110, 673, 437 };
        pt.data[1] = { 917, 110, 673, 437 };
        pt.data[2] = { 190, 603, 673, 437 };
        pt.data[3] = { 917, 603, 673, 437 };
        return bin;
}

    App::App() {
#ifdef _DEBUG
        UnhandledException([](IInspectable const&, UnhandledExceptionEventArgs const& args) {
            if (IsDebuggerPresent()) {
                auto errorMessage = args.Message();
                __debugbreak();
            }
        });
#endif
    }

    // ��ʼ��·��
    static void InitializePath() noexcept {
        using namespace MasterQian::System;

        // ��ȡ����·��
        Global.c_runPath = MasterQian::Storage::Path::Running();

        // ��ǰ�����ı�
        auto currentDate{ MasterQian::Timestamp().local().formatDate() };

        // ��Ƭ·��
        Global.c_photoPath = Global.c_runPath / L"photo";
        Global.c_photoPath.Create();
        Global.c_photoPath.Concat(currentDate).Create();

        // ����·��
        Global.c_thumbPath = Global.c_runPath / L"thumb";
        Global.c_thumbPath.Create();
        Global.c_thumbPath.Concat(currentDate).Create();

        // ģ��·��
        Global.c_templatePath = Global.c_runPath / L"template";
        Global.c_templatePath.Create();
        Global.c_templatePath.Concat(L"һ��.template").Create(GetTemplate1());
        Global.c_templatePath.Concat(L"����.template").Create(GetTemplate2());
        Global.c_templatePath.Concat(L"���.template").Create(GetTemplate5());
        Global.c_templatePath.Concat(L"�����.template").Create(GetTemplateMarry());

        // ����·��
        Global.c_dataPath = Global.c_runPath / L"data";
        Global.c_dataPath.Create();
        Global.c_dataPath.Concat(std::to_wstring(MasterQian::Timestamp{ }.local().year) + L".bin")
            .Create(GetDefaultRecordData());

        // �����ļ�
        Global.c_configFilePath = Global.c_runPath / L"app.config";
        Global.c_configFilePath.Create(GetDefaultConfig());
    
        // ��ʱ��Ƭ·��
        Global.c_tempPhotoPath = Global.c_runPath / L"temp.jpg";

        // �����Ƭ·��
        Global.c_cameraPhotoPath = Global.c_runPath / L"camera_photo";
        Global.c_cameraPhotoPath.Create();
    }

    // ��ʼ����Դ
    static void InitializeResource() noexcept {
        using namespace MasterQian::System;

        Global.res_icon = Process::GetResource(R_ICON);
        Global.res_default_img = Process::GetResource(R_DEFAULT_IMG);
    }

    // ��ʼ������
    static void InitializeConfig() noexcept {
        Global.cfg.load(Global.c_configFilePath.Read());

        for (auto& path : Global.c_templatePath.EnumFolder(L"*.template")) {
            auto bin{ path.Read() };
            auto& template_data{ *reinterpret_cast<PrintTemplate*>(bin.data()) };
            if (template_data.IsCorrect(bin.size32())) {
                Global.templateList.add(path.NameWithoutExt(), std::move(bin));
            }
        }
    }
    
    // ��ʼ������
    static void InitializeData() noexcept {
        Global.c_photoThumbSize = {
            static_cast<mqui32>(util::RDValue<mqf64>(L"PhotoThumbWidth")),
            static_cast<mqui32>(util::RDValue<mqf64>(L"PhotoThumbHeight")) };

        Global.c_photoPreviewSize = {
            static_cast<mqui32>(util::RDValue<mqf64>(L"PhotoPreviewWidth")),
            static_cast<mqui32>(util::RDValue<mqf64>(L"PhotoPreviewHeight")) };

        Global.c_IDCardPreviewSize = {
            static_cast<mqui32>(util::RDValue<mqf64>(L"IDCardPreviewWidth")),
            static_cast<mqui32>(util::RDValue<mqf64>(L"IDCardPreviewHeight")) };
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
    static void ClearGlobal() noexcept {
        // ����ֹͣ�ļ�����¼�
        SetEvent(Global.file_spy_event);
        // �ر�GDI
        MasterQian::Media::GDI::ShutdownGDI();
    }

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

    void App::OnLaunched(LaunchActivatedEventArgs const& e) {
        using namespace MasterQian::System;
        if (Process::SingleProcessLock()) { // ��ֹ�࿪
            // ��ʼ��
            InitializePath();
            InitializeResource();
            InitializeConfig();
            InitializeData();
            InitializeGlobal();

            // ��ʾ������
            window = make<MainWindow>();
            window.Activate();
            window.Closed([ ] (auto, auto) {
                ClearGlobal(); // ����ȫ��
                });

            // �������ڴ�С
            auto appWindow{ window.AppWindow() };
            auto width{ static_cast<mqi32>(Info::GetScreenWidth()) };
            auto height{ static_cast<mqi32>(Info::GetScreenHeight()) };
            auto scale{ util::RDValue<mqf64>(L"MainWindow.Scale.Value") };
            auto actualWidth{ static_cast<mqi32>(width * scale) };
            auto actualHeight{ static_cast<mqi32>(height * scale) };
            auto actualLeft{ static_cast<mqi32>((1.0 - scale) * width / 2) };
            auto actualTop{ static_cast<mqi32>((1.0 - scale) * height / 2) };
            appWindow.MoveAndResize({ actualLeft, actualTop, actualWidth, actualHeight });

            // ��ȡ���ھ��
            Global.ui_hwnd = reinterpret_cast<HWND>(appWindow.Id().Value);
            Global.ui_window = &window;

            // �첽����
            ClearPhotos();
        }
        else {
            Exit(); // ��������
        }
    }
}
