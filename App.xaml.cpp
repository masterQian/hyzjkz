#include "pch.h"
#include "App.xaml.h"
#include "MainWindow.xaml.h"

#include "app.h"

namespace winrt::hyzjkz::implementation {
    // [异步] 照片清理
    static IAsyncAction ClearPhotos() noexcept {
        co_await winrt::resume_background();

        mqchar clear_month[3]{ L'0', L'0', L'\0' };
        auto month{ static_cast<mqui32>(Timestamp{ }.local().month) };
        auto resetMonth{ Global.cfg.Get<GlobalConfig::RESET_MONTH>() };

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

    // 照片转储
    static void RestorePhotos() noexcept {
        auto date_str{ Timestamp{ }.local().formatDate() };
        auto photo_path{ Global.c_photoPath / date_str };
        auto thumb_path{ Global.c_thumbPath / date_str };
        for (auto& file : Global.c_cameraPhotoPath.EnumFolder(L"*.jpg")) {
            Media::GDI::Image thumb_image(file);
            thumb_image.Thumbnail(Global.c_photoThumbSize).Save(thumb_path / file.Name(), Media::GDI::ImageFormat::JPG);
            file.Copy(photo_path);
            file.Delete();
        }
    }

    // 取默认配置
    static Bin GetDefaultConfig() noexcept {
        GlobalConfig config;
        config.Set<GlobalConfig::EOS_PATH>();
        config.Set<GlobalConfig::PRINTER_NAME>();
        config.Set<GlobalConfig::AUTOCUT_EPS>();
        config.Set<GlobalConfig::UNIT_PRICE>();
        config.Set<GlobalConfig::SHOW_TURNOVER>();
        config.Set<GlobalConfig::RESET_MONTH>();
        config.Set<GlobalConfig::PASSWORD>();
        config.Set<GlobalConfig::USE_PASSWORD>();
        Parser::Config tools;
        tools.set(L"美图秀秀", L"");
        tools.set(L"光影魔术手", L"");
        tools.set(L"PhotoShop", L"");
        config.Set(GlobalConfig::TOOLS, tools);
        return config.save();
    }

    // 取默认报表数据
    inline Bin GetDefaultRecordData() noexcept {
        return Bin(sizeof(RecordData));
    }

    // 取一寸模板
    static Bin GetTemplate1() noexcept {
        mqui32 count{ 10U };
        Bin bin(PrintTemplate::CalcSize(count));
        auto& pt{ *reinterpret_cast<PrintTemplate*>(bin.data()) };
        pt.size = sizeof(PrintTemplate);
        pt.count = count;
        pt.flag.canDelete = false;
        pt.flag.isRotate = false;
        pt.flag.isAutoCut = true;
        lstrcpyW(pt.name, L"一寸");
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

    // 取二寸模板
    static Bin GetTemplate2() noexcept {
        mqui32 count{ 8U };
        Bin bin(PrintTemplate::CalcSize(count));
        auto& pt{ *reinterpret_cast<PrintTemplate*>(bin.data()) };
        pt.size = sizeof(PrintTemplate);
        pt.count = count;
        pt.flag.canDelete = false;
        pt.flag.isRotate = false;
        pt.flag.isAutoCut = true;
        lstrcpyW(pt.name, L"二寸");
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

    // 取五寸模板
    static Bin GetTemplate5() noexcept {
        mqui32 count{ 1U };
        Bin bin(PrintTemplate::CalcSize(count));
        auto& pt{ *reinterpret_cast<PrintTemplate*>(bin.data()) };
        pt.size = sizeof(PrintTemplate);
        pt.count = count;
        pt.flag.canDelete = false;
        pt.flag.isRotate = true;
        pt.flag.isAutoCut = false;
        lstrcpyW(pt.name, L"五寸");
        pt.data[0] = { 120, 50, 1500, 1050 };
        return bin;
    }

    // 取结婚照模板
    static Bin GetTemplateMarry() noexcept {
        mqui32 count{ 4U };
        Bin bin(PrintTemplate::CalcSize(count));
        auto& pt{ *reinterpret_cast<PrintTemplate*>(bin.data()) };
        pt.size = sizeof(PrintTemplate);
        pt.count = count;
        pt.flag.canDelete = false;
        pt.flag.isRotate = false;
        pt.flag.isAutoCut = false;
        lstrcpyW(pt.name, L"结婚照");
        pt.data[0] = { 205, 125, 673, 437 };
        pt.data[1] = { 895, 125, 673, 437 };
        pt.data[2] = { 205, 585, 673, 437 };
        pt.data[3] = { 895, 585, 673, 437 };
        return bin;
}

    App::App() {
        InitializeComponent();
#ifdef _DEBUG
        UnhandledException([](IInspectable const&, Microsoft::UI::Xaml::UnhandledExceptionEventArgs const& args) {
            if (IsDebuggerPresent()) {
                auto errorMessage = args.Message();
                __debugbreak();
            }
        });
#endif
    }

    // 初始化路径
    static void InitializePath() noexcept {
        // 获取运行路径
        Global.c_runPath = Storage::Path::Running();

        // 当前日期文本
        auto lt{ Timestamp{ }.local() };
        auto currentDate{ lt.formatDate() };

        // 照片路径
        Global.c_photoPath = Global.c_runPath / L"photo";
        Global.c_photoPath.Create();
        Global.c_photoPath.Concat(currentDate).Create();

        // 缓存路径
        Global.c_thumbPath = Global.c_runPath / L"thumb";
        Global.c_thumbPath.Create();
        Global.c_thumbPath.Concat(currentDate).Create();

        // 模板路径
        Global.c_templatePath = Global.c_runPath / L"template";
        Global.c_templatePath.Create();
        Global.c_templatePath.Concat(L"一寸.template").Create(GetTemplate1());
        Global.c_templatePath.Concat(L"二寸.template").Create(GetTemplate2());
        Global.c_templatePath.Concat(L"五寸.template").Create(GetTemplate5());
        Global.c_templatePath.Concat(L"结婚照.template").Create(GetTemplateMarry());

        // 数据路径
        Global.c_dataPath = Global.c_runPath / L"data";
        Global.c_dataPath.Create();
        Global.c_dataPath.Concat(std::to_wstring(lt.year) + L".bin")
            .Create(GetDefaultRecordData());

        // 配置文件
        Global.c_configFilePath = Global.c_runPath / L"app.config";
        Global.c_configFilePath.Create(GetDefaultConfig());
    
        // 临时照片路径
        Global.c_tempPhotoPath = Global.c_runPath / L"temp.jpg";

        // 相机照片路径
        Global.c_cameraPhotoPath = Global.c_runPath / L"camera_photo";
        Global.c_cameraPhotoPath.Create();
    }

    // 初始化资源
    static void InitializeResource() noexcept {
        Global.res_icon = System::Process::GetResource(R_ICON);
        Global.res_default_img = System::Process::GetResource(R_DEFAULT_IMG);
    }

    // 初始化配置
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
    
    // 初始化数据
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

        Global.c_A4Size = {
            static_cast<mqui32>(util::RDValue<mqf64>(L"A4PrinterCanvasWidth")),
            static_cast<mqui32>(util::RDValue<mqf64>(L"A4PrinterCanvasHeight"))
        };

        Global.c_ToPDFImageSize = {
            static_cast<mqui32>(util::RDValue<mqf64>(L"ToPDFImageWidth")),
            static_cast<mqui32>(util::RDValue<mqf64>(L"ToPDFImageHeight"))
        };
    }

    // 初始化全局
    static void InitializeGlobal() noexcept {
        // 开启GDI
        Media::GDI::StartupGDI();
        // 创建文件监控事件
        Global.file_spy_event = CreateEventW(nullptr, true, false, nullptr);
    }

    void App::OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const& e) {
        if (System::Process::SingleProcessLock()) { // 防止多开
            // 初始化
            InitializePath();
            InitializeResource();
            InitializeConfig();
            InitializeData();
            InitializeGlobal();

            // 照片转储
            RestorePhotos();

            // 异步任务
            ClearPhotos();

            // 显示主窗口
            Global.ui_window = make<MainWindow>();
            window = Global.ui_window;
            window.Closed([ ] (auto...) {
                // 取消引用
                Global.ui_window = nullptr;
                // 发送停止文件监控事件
                SetEvent(Global.file_spy_event);
                // 关闭GDI
                Media::GDI::ShutdownGDI();
            });
            window.Activate();
        }
        else {
            Exit(); // 结束程序
        }
    }
}
