#include "pch.h"
#include <queue>
#include "PrintPage.xaml.h"
#if __has_include("PrintPage.g.cpp")
#include "PrintPage.g.cpp"
#endif

namespace winrt::hyzjkz::implementation {
	// ����ͬ��ͬ�ߵ����Ϊһ��, ����λ�����鼰��Ӧ����ͼ����
	struct ImageItem {
		std::vector<mqpoint> points;
		Media::GDI::Image img;

		static Media::GDI::Image MakeImage(bool isResample, Media::GDI::Image const& src,
			mqsize actual, bool isRotate, bool isAutoCut) noexcept {
			mqrect crop_rect{ }; // �ü�����
			if (isRotate) { // ��Ҫ��ת�Ƚ���߻���
				std::swap(actual.width, actual.height);
			}
			if (isAutoCut) {
				auto src_width{ src.Width() }; // Դ���
				auto src_height{ src.Height() }; // Դ�߶�
				auto src_scale{ static_cast<mqf64>(src_height) / src_width }; // Դ�߿��
				auto actual_scale{ static_cast<mqf64>(actual.height) / actual.width }; // ʵ�ʸ߿��
				auto eps{ Global.cfg.Get<GlobalConfig::AUTOCUT_EPS>() }; // ���̷�Χ
				if (src_scale + eps < actual_scale) { // ˵��Դ̫��, ѡ�����Ҳü�
					auto correct_width{ static_cast<mqui32>(src_height / actual_scale) }; // �����������
					crop_rect = { (src_width - correct_width) / 2U, 0U, correct_width, src_height }; // ������Ƭ���Ⱦ���
				}
				else if (src_scale - eps > actual_scale) { // ˵��Դ̫��, ѡ�����²ü�
					auto correct_height{ static_cast<mqui32>(src_width * actual_scale) }; // ���������߶�
					// һ�������϶С�����, ȡ��������֮һ����Ƭ����
					crop_rect = { 0, (src_height - correct_height) / 3U, src_width, correct_height };
				}
				else { // ����ü�
					isAutoCut = false;
				}
			}

			Media::GDI::Image image{ isResample ?
				(isAutoCut ?
					src.Crop(crop_rect).Resample({ actual.width, actual.height }, Media::GDI::QUALITY_MODE)
					: src.Resample({ actual.width, actual.height }, Media::GDI::QUALITY_MODE))
				:
				(isAutoCut ?
					src.Crop(crop_rect).Thumbnail({ actual.width, actual.height })
					: src.Thumbnail({ actual.width, actual.height })) };
			if (isRotate) { // ��Ҫ��ת
				image.RotateLeft();
			}
			return image;
		}

		ImageItem(bool isResample, Media::GDI::Image const& src,
			mqsize actual, bool isRotate, bool isAutoCut) noexcept :
			img{ MakeImage(isResample, src, actual, isRotate, isAutoCut) } {}

		void Add(mqpoint point) noexcept {
			points.emplace_back(point.x, point.y);
		}
	};

	// �ߴ�ӳ��
	using ImageItems = std::unordered_map<mqsize, ImageItem, freestanding::isomerism_hash>;

	// Ԥ��ģ��
	static void PreviewTemplate(PrintPage* page, std::wstring_view template_name) noexcept {
		auto& pt{ Global.templateList[template_name] };
		auto border{ page->Border_Main() };
		auto canvas_width{ border.ActualWidth() };
		auto canvas_height{ border.ActualHeight() };
		auto canvas_scale{ Global.c_printCanvasSize.height / canvas_height };

		// ��Ƭ
		Media::GDI::Image photo{ page->photo_path };
		// ����
		Media::GDI::Image canvas({ static_cast<mqui32>(canvas_width), static_cast<mqui32>(canvas_height) }, Media::Colors::Pink);
		canvas.DPI(photo.DPI());

		// ͬ��������
		ImageItems photoItems;
		for (mqui32 i{ }; i < pt.count; ++i) {
			auto actual{ pt.data[i] / canvas_scale };
			mqsize size{ actual.width, actual.height };
			auto iter{ photoItems.find(size) };
			if (iter == photoItems.cend()) {
				iter = photoItems.try_emplace(size, false, photo, size, pt.flag.isRotate, pt.flag.isAutoCut).first;
			}
			iter->second.Add({ actual.left, actual.top });
		}

		// ����Ƭ
		for (auto& [size, pre] : photoItems) {
			for (auto& [left, top] : pre.points) {
				canvas.DrawImage(pre.img, { left, top }, { }, Media::GDI::FAST_MODE);
			}
		}
		page->CanvasImage().Source(util::StreamToBMP(canvas.SaveToUnsafeStream(Media::GDI::ImageFormat::BMP)));
	}

	// ˢ��ģ���б�
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

	// ���ɱ���ģ��
	static Media::GDI::Image MakeSaveImage(PrintTemplate& pt, hstring const& photo_path, mqsize canvas_size) noexcept {
		// ��Ƭ
		Media::GDI::Image photo{ photo_path };
		photo.DPI({ 300U, 300U });
		// ����
		Media::GDI::Image canvas(canvas_size, Media::Colors::White);
		canvas.DPI({ 300U, 300U });

		// ͬ��������
		ImageItems photoItems;
		for (mqui32 i{ }; i < pt.count; ++i) {
			auto& data{ pt.data[i] };
			mqsize size{ data.width, data.height };
			auto iter{ photoItems.find(size) };
			if (iter == photoItems.cend()) {
				iter = photoItems.try_emplace(size, true, photo, size, pt.flag.isRotate, pt.flag.isAutoCut).first;
			}
			iter->second.Add({ data.left, data.top });
		}

		// ����Ƭ
		for (auto& [size, pre] : photoItems) {
			for (auto& [left, top] : pre.points) {
				canvas.DrawImage(pre.img, { left, top }, { }, Media::GDI::QUALITY_MODE);
			}
		}
		return canvas;
	}

	// ��ӡ��Ƭ
	static IAsyncAction PrintPhoto(PrintPage* page, hstring template_name, hstring photo_path) noexcept {
		auto printer_name{ Global.cfg.Get<GlobalConfig::PRINTER_NAME>() };
		bool failed{ true };
		if (!printer_name.empty()) {
			Media::GDI::Printer printer(printer_name);
			if (printer.OK()) {
				winrt::apartment_context ui_thread;
				co_await winrt::resume_background();

				printer.DrawImage(MakeSaveImage(Global.templateList[template_name],
					photo_path, Global.c_printCanvasSize), Media::GDI::QUALITY_MODE);

				co_await ui_thread;

				failed = false;
			}
		}
		if (failed) {
			co_await Global.ui_window->as<hyzjkz::MainWindow>()
				.ShowTipDialog(util::RDString<hstring>(L"PrintPage.Tip.NoPrinter"));
		}
	}

	// ������Ƭ
	static IAsyncAction ExportPhoto(hstring template_name, hstring photo_path) noexcept {
		Windows::Storage::Pickers::FileSavePicker savePicker;
		savePicker.SuggestedStartLocation(Windows::Storage::Pickers::PickerLocationId::Desktop);
		auto filters{ savePicker.FileTypeChoices() };
		filters.Insert(L"JPGͼƬ", single_threaded_observable_vector<hstring>({ L".jpg" }));
		savePicker.SuggestedFileName(template_name);
		util::InitializeDialog(savePicker, Global.ui_hwnd);

		if (Windows::Storage::StorageFile file{ co_await savePicker.PickSaveFileAsync() }) {
			winrt::apartment_context ui_thread;
			co_await winrt::resume_background();

			MakeSaveImage(Global.templateList[template_name], photo_path, Global.c_printCanvasSize)
				.Save(file.Path(), Media::GDI::ImageFormat::JPG);

			co_await ui_thread;
		}
	}
}

namespace winrt::hyzjkz::implementation {
	PrintPage::PrintPage() {
		InitializeComponent();

		Loaded([this] (auto, auto) {
			// ��Ϊ�����ߴ����л�����ǰδȷ��, ����ȵ���ɼ��غ����Ԥ��ģ��
			RefreshTemplates(this);
			});
	}

	// �л�ҳ��
	F_EVENT void PrintPage::OnNavigatedTo(Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& args) {
		photo_path = unbox_value<hstring>(args.Parameter());
	}

	// ����������С
	F_EVENT void PrintPage::Canvas_SizeChanged(IInspectable const& sender, Microsoft::UI::Xaml::SizeChangedEventArgs const&) {
		auto canvas{ sender.as<FrameworkElement>() };
		canvas.Width(canvas.ActualHeight() * util::RDValue<mqf64>(L"CanvasWidth") / util::RDValue<mqf64>(L"CanvasHeight"));
	}

	// ģ���б�ѡ��
	F_EVENT void PrintPage::LV_Templates_SelectionChanged(IInspectable const&, Controls::SelectionChangedEventArgs const& args) {
		auto items{ args.AddedItems() };
		if (items.Size() == 1U) {
			PreviewTemplate(this, items.GetAt(0U).as<hstring>());
		}
	}

	// ��ť�鵥��
	F_EVENT IAsyncAction PrintPage::AppBarButton_Click(IInspectable const& sender, RoutedEventArgs const&) {
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