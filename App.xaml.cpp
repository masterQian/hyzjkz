#include "pch.h"
#include "App.xaml.h"
#include "MainWindow.xaml.h"

#include "app.h"

namespace winrt::hyzjkz::implementation {
    // ȡĬ������
    static MasterQian::Bin GetDefaultConfig() noexcept {
        GlobalConfig config;
        config.Set<GlobalConfig::EOS_PATH>();
        config.Set<GlobalConfig::PRINTER_NAME>();
        config.Set<GlobalConfig::AUTOCUT_EPS>();
        config.Set<GlobalConfig::UNIT_PRICE>();
        config.Set<GlobalConfig::SHOW_TURNOVER>();
        config.Set<GlobalConfig::RESET_MONTH>();
        config.Set<GlobalConfig::PASSWORD>();
        config.Set<GlobalConfig::USE_PASSWORD>();
        MasterQian::Parser::Config tools;
        tools.set(L"��ͼ����", L"");
        tools.set(L"��Ӱħ����", L"");
        tools.set(L"PhotoShop", L"");
        config.Set(GlobalConfig::TOOLS, tools);
        return config.save();
    }

    // ȡĬ�ϱ�������
    static MasterQian::Bin GetDefaultRecordData() noexcept {
        return MasterQian::Bin(sizeof(RecordData));
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
        pt.data[0] = { 100, 160, 295, 413 };
        pt.data[1] = { 415, 160, 295, 413 };
        pt.data[2] = { 730, 160, 295, 413 };
        pt.data[3] = { 1045, 160, 295, 413 };
        pt.data[4] = { 1360, 160, 295, 413 };
        pt.data[5] = { 100, 590, 295, 413 };
        pt.data[6] = { 415, 590, 295, 413 };
        pt.data[7] = { 730, 590, 295, 413 };
        pt.data[8] = { 1045, 590, 295, 413 };
        pt.data[9] = { 1360, 590, 295, 413 };
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
        pt.data[0] = { 205, 125, 673, 437 };
        pt.data[1] = { 895, 125, 673, 437 };
        pt.data[2] = { 205, 585, 673, 437 };
        pt.data[3] = { 895, 585, 673, 437 };
        return bin;
}

    App::App() {
        InitializeComponent();
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
        Global.res_icon = MasterQian::System::Process::GetResource(R_ICON);
        Global.res_default_img = MasterQian::System::Process::GetResource(R_DEFAULT_IMG);
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

        Global.c_printCanvasSize = {
            static_cast<mqui32>(util::RDValue<mqf64>(L"CanvasWidth")),
            static_cast<mqui32>(util::RDValue<mqf64>(L"CanvasHeight")) };

        Global.c_IDCardPreviewSize = {
            static_cast<mqui32>(util::RDValue<mqf64>(L"IDCardPreviewWidth")),
            static_cast<mqui32>(util::RDValue<mqf64>(L"IDCardPreviewHeight")) };
    }

    void App::OnLaunched(LaunchActivatedEventArgs const& e) {
        if (MasterQian::System::Process::SingleProcessLock()) { // ��ֹ�࿪
            // ��ʼ��
            InitializePath();
            InitializeResource();
            InitializeConfig();
            InitializeData();

            // ��ʾ������
            window = make<MainWindow>();
            Global.ui_window = &window;
            window.Activate();
        }
        else {
            Exit(); // ��������
        }
    }
}
