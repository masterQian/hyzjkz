#include "pch.h"
#include "PrintPage.xaml.h"
#if __has_include("PrintPage.g.cpp")
#include "PrintPage.g.cpp"
#endif

#include <queue>

namespace winrt::hyzjkz::implementation {
	// 所有同宽同高的项归为一类, 由其位置数组及对应缩略图构成
	struct ImageItem {
		std::vector<MasterQian::Media::ImagePoint> points;
		MasterQian::Media::GDI::Image img;

		static MasterQian::Media::GDI::Image MakeImage(bool isResample, MasterQian::Media::GDI::Image const& src,
			mqui32 actual_width, mqui32 actual_height, bool isRotate, bool isAutoCut) noexcept {
			MasterQian::Media::ImageRect crop_rect{ }; // 裁剪参数
			if (isRotate) { // 若要旋转先将宽高互换
				std::swap(actual_width, actual_height);
			}
			if (isAutoCut) {
				auto src_width{ src.Width() }; // 源宽度
				auto src_height{ src.Height() }; // 源高度
				auto src_scale{ static_cast<mqf64>(src_height) / src_width }; // 源高宽比
				auto actual_scale{ static_cast<mqf64>(actual_height) / actual_width }; // 实际高宽比
				auto eps{ Global.cfg.get<GlobalType::AUTOCUT_EPS>(GlobalConfig::AUTOCUT_EPS) }; // 容忍范围
				if (src_scale + eps < actual_scale) { // 说明源太宽, 选择左右裁剪
					auto correct_width{ static_cast<mqui32>(src_height / actual_scale) }; // 计算修正宽度
					crop_rect = { (src_width - correct_width) / 2U, 0U, correct_width, src_height }; // 两侧碎片长度均分
				}
				else if (src_scale - eps > actual_scale) { // 说明源太高, 选择上下裁剪
					auto correct_height{ static_cast<mqui32>(src_width * actual_scale) }; // 计算修正高度
					// 一般上面空隙小下面大, 取上面三分之一的碎片长度
					crop_rect = { 0, (src_height - correct_height) / 3U, src_width, correct_height };
				}
				else { // 无需裁剪
					isAutoCut = false;
				}
			}

			MasterQian::Media::GDI::Image image{ isResample ?
				(isAutoCut ?
					src.Crop(crop_rect).Resample({ actual_width, actual_height }, MasterQian::Media::QUALITY_MODE)
					: src.Resample({ actual_width, actual_height }, MasterQian::Media::QUALITY_MODE))
				:
				(isAutoCut ?
					src.Crop(crop_rect).Thumbnail({ actual_width, actual_height })
					: src.Thumbnail({ actual_width, actual_height })) };
			if (isRotate) { // 需要旋转
				image.RotateLeft();
			}
			return image;
		}

		ImageItem(bool isResample, MasterQian::Media::GDI::Image const& src,
			mqui32 actual_width, mqui32 actual_height, bool isRotate, bool isAutoCut) noexcept :
			img{ MakeImage(isResample, src, actual_width, actual_height, isRotate, isAutoCut) } {}

		void Add(MasterQian::Media::ImagePoint point) noexcept {
			points.emplace_back(point.x, point.y);
		}
	};

	struct ImageSizeHash {
		auto operator () (MasterQian::Media::ImageSize const& size) const noexcept {
			return std::hash<mqui32>()(size.width) ^ std::hash<mqui32>()(size.height);
		}
	};

	// 尺寸映射
	using ImageItems = std::unordered_map<MasterQian::Media::ImageSize, ImageItem, ImageSizeHash>;

	// 预览模板
	static void PreviewTemplate(PrintPage* page, std::wstring_view template_name) noexcept {
		using namespace MasterQian::Media;
		auto& pt{ Global.templateList[template_name] };
		auto border{ page->Border_Main() };
		auto canvas_width{ border.ActualWidth() };
		auto canvas_height{ border.ActualHeight() };
		auto canvas_scale{ Global.c_printCanvasSize.height / canvas_height };

		// 照片
		GDI::Image photo{ page->photo_path };
		// 画布
		GDI::Image canvas({ static_cast<mqui32>(canvas_width), static_cast<mqui32>(canvas_height) }, Colors::Pink);
		canvas.DPI(photo.DPI());

		// 同宽高项分类
		ImageItems photoItems;
		for (mqui32 i{ }; i < pt.count; ++i) {
			auto& data{ pt.data[i] };
			auto actual_left{ static_cast<mqui32>(data.left / canvas_scale) };
			auto actual_top{ static_cast<mqui32>(data.top / canvas_scale) };
			auto actual_width{ static_cast<mqui32>(data.width / canvas_scale) };
			auto actual_height{ static_cast<mqui32>(data.height / canvas_scale) };
			ImageSize size{ actual_width, actual_height };
			auto iter{ photoItems.find(size) };
			if (iter == photoItems.cend()) {
				iter = photoItems.try_emplace(size, false, photo, actual_width,
					actual_height, pt.flag.isRotate, pt.flag.isAutoCut).first;
			}
			iter->second.Add({ actual_left, actual_top });
		}

		// 画照片
		for (auto& [size, pre] : photoItems) {
			for (auto& [left, top] : pre.points) {
				canvas.DrawImage(pre.img, { left, top }, { }, FAST_MODE);
			}
		}
		page->CanvasImage().Source(util::StreamToBMP(canvas.SaveToUnsafeStream(ImageFormat::BMP)));
	}

	// 刷新模板列表
	static void RefreshTemplates(PrintPage* page) noexcept {
		auto lv_templates{ page->LV_Templates() };
		auto items{ lv_templates.Items() };
		items.Clear();

		struct TemplateItem {
			mqui32 order;
			std::wstring_view name;

			[[nodiscard]] bool operator < (TemplateItem const& item) const noexcept {
				return order < item.order;
			}
		};

		std::priority_queue<TemplateItem> queue;
		for (auto& [name, bin] : Global.templateList) {
			auto& pt{ *reinterpret_cast<PrintTemplate*>(bin.data()) };
			queue.emplace(pt.order, name);
		}
		while (!queue.empty()) {
			auto& [_, name] { queue.top() };
			items.Append(box_value(name));
			queue.pop();
		}
		if (items.Size() > 0U) {
			lv_templates.SelectedItem(items.GetAt(0U));
		}
	}

	// 生成保存模板
	static MasterQian::Media::GDI::Image MakeSaveImage(PrintTemplate& pt, hstring const& photo_path, MasterQian::Media::ImageSize canvas_size) noexcept {
		// 照片
		MasterQian::Media::GDI::Image photo{ photo_path };
		photo.DPI({ 300U, 300U });
		// 画布
		MasterQian::Media::GDI::Image canvas(canvas_size, MasterQian::Media::Colors::White);
		canvas.DPI({ 300U, 300U });

		// 同宽高项分类
		ImageItems photoItems;
		for (mqui32 i{ }; i < pt.count; ++i) {
			auto& data{ pt.data[i] };
			MasterQian::Media::ImageSize size{ data.width, data.height };
			auto iter{ photoItems.find(size) };
			if (iter == photoItems.cend()) {
				iter = photoItems.try_emplace(size, true, photo, data.width,
					data.height, pt.flag.isRotate, pt.flag.isAutoCut).first;
			}
			iter->second.Add({ data.left, data.top });
		}

		// 画照片
		for (auto& [size, pre] : photoItems) {
			for (auto& [left, top] : pre.points) {
				canvas.DrawImage(pre.img, { left, top }, { }, MasterQian::Media::QUALITY_MODE);
			}
		}
		return canvas;
	}

	// 打印照片
	static Windows::Foundation::IAsyncAction PrintPhoto(PrintPage* page, hstring template_name, hstring photo_path) noexcept {
		auto printer_name{ Global.cfg.get(GlobalConfig::PRINTER_NAME, GlobalDefault::PRINTER_NAME) };
		bool failed{ true };
		if (!printer_name.empty()) {
			MasterQian::Media::GDI::Printer printer(printer_name);
			if (printer.OK()) {
				winrt::apartment_context ui_thread;
				co_await winrt::resume_background();

				printer.DrawImage(MakeSaveImage(Global.templateList[template_name],
					photo_path, Global.c_printCanvasSize), { }, { }, MasterQian::Media::QUALITY_MODE);

				co_await ui_thread;

				failed = false;
			}
		}
		if (failed) {
			co_await Global.ui_window->as<hyzjkz::MainWindow>()
				.ShowTipDialog(util::RDString<hstring>(L"PrintPage.Tip.NoPrinter"));
		}
	}

	// 导出照片
	static Windows::Foundation::IAsyncAction ExportPhoto(hstring template_name, hstring photo_path) noexcept {
		Windows::Storage::Pickers::FileSavePicker savePicker;
		savePicker.SuggestedStartLocation(Windows::Storage::Pickers::PickerLocationId::Desktop);
		auto filters{ savePicker.FileTypeChoices() };
		filters.Insert(L"JPG图片", single_threaded_observable_vector<hstring>({ L".jpg" }));
		savePicker.SuggestedFileName(template_name);
		util::InitializeDialog(savePicker, Global.ui_hwnd);

		if (Windows::Storage::StorageFile file{ co_await savePicker.PickSaveFileAsync() }) {
			winrt::apartment_context ui_thread;
			co_await winrt::resume_background();

			MakeSaveImage(Global.templateList[template_name], photo_path, Global.c_printCanvasSize)
				.Save(file.Path(), MasterQian::Media::ImageFormat::JPG);

			co_await ui_thread;
		}
	}
}

namespace winrt::hyzjkz::implementation {
	PrintPage::PrintPage() {
		InitializeComponent();

		Loaded([this] (auto, auto) {
			// 因为画布尺寸在切换导航前未确定, 必须等到完成加载后才能预览模板
			RefreshTemplates(this);
			});
	}

	// 切换页面
	F_EVENT void PrintPage::OnNavigatedTo(Navigation::NavigationEventArgs const& args) {
		photo_path = unbox_value<hstring>(args.Parameter());
	}

	// 画布调整大小
	F_EVENT void PrintPage::Canvas_SizeChanged(IInspectable const& sender, SizeChangedEventArgs const&) {
		auto canvas{ sender.as<FrameworkElement>() };
		canvas.Width(canvas.ActualHeight() * util::RDValue<mqf64>(L"CanvasWidth") / util::RDValue<mqf64>(L"CanvasHeight"));
	}

	// 模板列表选择
	F_EVENT void PrintPage::LV_Templates_SelectionChanged(IInspectable const&, Controls::SelectionChangedEventArgs const& args) {
		auto items{ args.AddedItems() };
		if (items.Size() == 1U) {
			PreviewTemplate(this, items.GetAt(0U).as<hstring>());
		}
	}

	// 按钮组单击
	F_EVENT Windows::Foundation::IAsyncAction PrintPage::AppBarButton_Click(IInspectable const& sender, RoutedEventArgs const&) {
		auto label{ sender.as<Controls::AppBarButton>().Label() };
		auto template_name{ LV_Templates().SelectedItem().as<hstring>() };
		if (label == util::RDString<hstring>(L"PrintPage.Bar.Print")) {
			co_await PrintPhoto(this, template_name, photo_path);
		}
		else if (label == util::RDString<hstring>(L"PrintPage.Bar.Save")) {
			co_await ExportPhoto(template_name, photo_path);
		}
	}
}