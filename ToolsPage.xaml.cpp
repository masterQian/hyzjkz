#include "pch.h"
#include "ToolsPage.xaml.h"
#if __has_include("ToolsPage.g.cpp")
#include "ToolsPage.g.cpp"
#endif

namespace winrt::hyzjkz::implementation {
	/*  ��ҳ  */

	// ��ʼ��
	static void InitializeHomeSubPage(IInspectable* subPage) noexcept {
		auto sp{ ToolsPage::HomeSubPage::From(subPage) };
		sp->Stretch(Microsoft::UI::Xaml::Media::Stretch::Fill);
		sp->Source(util::BinToBMP(System::Process::GetResource(R_SIZEINFORMATION), { }, true));
	}

	/*  ���֤ƴ��  */

	// ѡ��
	static IAsyncAction IDCard_Select_Click(ToolsPage::IDCardSubPage* sp, IInspectable const& sender) noexcept {
		auto button{ sender.as<Controls::Button>() };

		// δ������Ƭ״̬
		if (button.Tag().as<hstring>() == L"false") {
			// ׼���Ի���
			Windows::Storage::Pickers::FileOpenPicker openPicker;
			openPicker.SuggestedStartLocation(Windows::Storage::Pickers::PickerLocationId::Desktop);
			openPicker.ViewMode(Windows::Storage::Pickers::PickerViewMode::Thumbnail);
			auto filters{ openPicker.FileTypeFilter() };
			filters.Append(L".jpg");
			filters.Append(L".png");
			filters.Append(L".bmp");
			util::InitializeDialog(openPicker, Global.ui_hwnd);

			// ������Ƭ
			if (Windows::Storage::StorageFile file{ co_await openPicker.PickSingleFileAsync() }) {
				Controls::Image image;
				image.Width(Global.c_IDCardPreviewSize.width);
				image.Height(Global.c_IDCardPreviewSize.height);
				image.Stretch(Microsoft::UI::Xaml::Media::Stretch::Fill);
				image.Tag(box_value(file.Path()));
				image.Source(util::FileToBMP(file.Path(), Global.c_IDCardPreviewSize, true));
				button.Content(image);
				button.Tag(box_value(L"true"));

				// �����涼׼����
				if (sp->button_front.Tag().as<hstring>() == L"true" && sp->button_back.Tag().as<hstring>() == L"true") {
					auto front_path{ sp->button_front.Content().as<FrameworkElement>().Tag().as<hstring>() };
					auto back_path{ sp->button_back.Content().as<FrameworkElement>().Tag().as<hstring>() };

					sp->button_clear.IsEnabled(false);
					sp->button_work.IsEnabled(false);
					sp->preview_text.Text(util::RDString<hstring>(L"IDCardSubPage.Previewing.Text"));

					winrt::apartment_context ui_thread;
					co_await winrt::resume_background();

					Media::GDI::Image front_image{ front_path }, back_image{ back_path };
					Media::GDI::Image combine_image{ front_image.Combine<false>(back_image,
						{ 0U, 0U, front_image.Width(), front_image.Height() / 2U },
						{ 0U, back_image.Height() / 2U, back_image.Width(), back_image.Height() / 2U },
						Media::GDI::FAST_MODE).Thumbnail(Global.c_IDCardPreviewSize) };

					co_await ui_thread;

					sp->button_clear.IsEnabled(true);
					sp->button_work.IsEnabled(true);
					sp->preview_text.Text(util::RDString<hstring>(L"IDCardSubPage.Preview.Text"));

					sp->image_preview.Source(util::StreamToBMP(combine_image.SaveToUnsafeStream(Media::GDI::ImageFormat::BMP)));
				}
			}
		}
	}

	// ���ð�ť
	inline static void IDCard_Reset(Controls::Button const& button) noexcept {
		if (button.Tag().as<hstring>() == L"true") {
			Controls::SymbolIcon icon;
			icon.Symbol(Controls::Symbol::Add);
			button.Content(icon);
			button.Tag(box_value(L"false"));
		}
	}

	// ���
	static void IDCard_Clear_Click(ToolsPage::IDCardSubPage* sp) noexcept {
		IDCard_Reset(sp->button_front);
		IDCard_Reset(sp->button_back);
		sp->image_preview.Source(nullptr);
	}

	// �ϳ�
	static IAsyncAction IDCard_Work_Click(ToolsPage::IDCardSubPage* sp) noexcept {
		if (sp->button_front.Tag().as<hstring>() == L"true" && sp->button_back.Tag().as<hstring>() == L"true") {
			Windows::Storage::Pickers::FileSavePicker savePicker;
			savePicker.SuggestedStartLocation(Windows::Storage::Pickers::PickerLocationId::Desktop);
			auto filters{ savePicker.FileTypeChoices() };
			filters.Insert(L"JPGͼƬ", single_threaded_observable_vector<hstring>({ L".jpg" }));
			savePicker.SuggestedFileName(util::RDString<hstring>(L"IDCardSubPage.SaveName"));
			util::InitializeDialog(savePicker, Global.ui_hwnd);

			auto front_path{ sp->button_front.Content().as<FrameworkElement>().Tag().as<hstring>() };
			auto back_path{ sp->button_back.Content().as<FrameworkElement>().Tag().as<hstring>() };

			if (Windows::Storage::StorageFile file{ co_await savePicker.PickSaveFileAsync() }) {
				IDCard_Clear_Click(sp);

				winrt::apartment_context ui_thread;
				co_await winrt::resume_background();

				Media::GDI::Image front_image{ front_path }, back_image{ back_path };
				front_image.Combine<false>(back_image,
					{ 0U, 0U, front_image.Width(), front_image.Height() / 2U },
					{ 0U, back_image.Height() / 2U, back_image.Width(), back_image.Height() / 2U },
					Media::GDI::QUALITY_MODE).Save(file.Path(), Media::GDI::ImageFormat::JPG);

				co_await ui_thread;
			}
		}
		else {
			co_await Global.ui_window->as<hyzjkz::MainWindow>().ShowTipDialog(util::RDString<hstring>(L"IDCardSubPage.Tip.AddPhoto"));
		}
	}

	// ��ʼ��
	static void InitializeIDCardSubPage(IInspectable* subPage) noexcept {
		auto sp{ ToolsPage::IDCardSubPage::From(subPage) };
		Application::LoadComponent(*sp, Uri{ L"ms-appx:///Xaml/Tools/IDCardSubPage.xaml" });
		sp->Bind(sp->button_front, L"Button_Front");
		sp->Bind(sp->button_back, L"Button_Back");
		sp->Bind(sp->button_clear, L"Button_Clear");
		sp->Bind(sp->button_work, L"Button_Work");
		sp->Bind(sp->preview_text, L"Preview_Text");
		sp->Bind(sp->image_preview, L"Image_Preview");

		sp->button_front.Click([sp] (IInspectable const& sender, auto) -> IAsyncAction {
			co_await IDCard_Select_Click(sp, sender);
			});
		sp->button_back.Click([sp] (IInspectable const& sender, auto) -> IAsyncAction {
			co_await IDCard_Select_Click(sp, sender);
			});
		sp->button_clear.Click([sp] (auto, auto) {
			IDCard_Clear_Click(sp);
			});
		sp->button_work.Click([sp] (auto, auto) -> IAsyncAction {
			co_await IDCard_Work_Click(sp);
			});
	}

	// ����
	static void UpdateIDCardSubPage(IInspectable* subPage) noexcept {
		IDCard_Clear_Click(ToolsPage::IDCardSubPage::From(subPage));
	}

	/*  ƴͼ  */

	// ��ջ�ͼ���
	static void Canvas_Jigsaw_Clear(ToolsPage::JigsawSubPage* sp) noexcept {
		sp->canvas.Children().Clear();
	}

	// �޸ĳߴ�
	static void Canvas_Jigsaw_Resize(ToolsPage::JigsawSubPage* sp) noexcept {
		auto canvas{ sp->canvas };
		mqf64 scale{ sp->switch_mode.IsOn() ?
			static_cast<mqf64>(Global.c_printCanvasSize.width) / Global.c_printCanvasSize.height // A6
			: static_cast<mqf64>(Global.c_A4Size.width) / Global.c_A4Size.height }; // A4
		canvas.Width(canvas.ActualHeight() * scale);
		Microsoft::UI::Xaml::Media::RectangleGeometry rg;
		rg.Rect({ 0.0f, 0.0f, static_cast<mqf32>(canvas.ActualWidth()), static_cast<mqf32>(canvas.ActualHeight()) });
		canvas.Clip(rg);
	}

	// ����
	static IAsyncAction Canvas_Jigsaw_Import(ToolsPage::JigsawSubPage* sp) noexcept {
		Windows::Storage::Pickers::FileOpenPicker openPicker;
		openPicker.SuggestedStartLocation(Windows::Storage::Pickers::PickerLocationId::Desktop);
		openPicker.ViewMode(Windows::Storage::Pickers::PickerViewMode::Thumbnail);
		auto filters{ openPicker.FileTypeFilter() };
		filters.Append(L".jpg");
		filters.Append(L".png");
		filters.Append(L".bmp");
		util::InitializeDialog(openPicker, Global.ui_hwnd);

		if (Windows::Foundation::Collections::IVectorView<Windows::Storage::StorageFile>
			files_view{ co_await openPicker.PickMultipleFilesAsync() }; files_view.Size() > 0U) {
			std::vector<std::wstring> files;
			for (auto file_view : files_view) {
				files.emplace_back(file_view.Path());
			}

			auto items{ sp->canvas.Children() };
			auto canvas_height{ sp->canvas.ActualHeight() }; // ����ʵ�ʸ߶�

			winrt::apartment_context ui_thread;

			for (auto& file : files) {
				co_await winrt::resume_background();

				Media::GDI::Image image(file);
				auto image_size{ image.Size() };
				if (image_size != mqsize{ }) {
					auto thumb_height{ static_cast<mqui32>(canvas_height / 3) }; // �߶����ŵ������߶ȵ�1/3
					auto thumb_width{ thumb_height * image_size.width / image_size.height }; // ��Ȱ���������
					image = image.Resample({ thumb_width, thumb_height }, Media::GDI::FAST_MODE); // ���²���
					auto stream{ image.SaveToUnsafeStream(Media::GDI::ImageFormat::BMP) }; // ���浽��
					// ����UI
					co_await ui_thread;

					Controls::Image drag_image;
					drag_image.Width(thumb_width);
					drag_image.Height(thumb_height);
					drag_image.Stretch(Microsoft::UI::Xaml::Media::Stretch::Fill);
					drag_image.Source(util::StreamToBMP(stream));
					drag_image.Tag(box_value(file)); // ��¼��Ƭ·��
					// �ƶ��¼�
					drag_image.PointerMoved([ ] (IInspectable const& sender, Input::PointerRoutedEventArgs const& e) {
						auto drag_image{ sender.as<Controls::Image>() };
						auto cp{ e.GetCurrentPoint(drag_image) };
						if (cp.Properties().IsLeftButtonPressed()) {
							auto pos{ cp.Position() };
							auto old_left{ Controls::Canvas::GetLeft(drag_image) };
							auto old_top{ Controls::Canvas::GetTop(drag_image) };
							auto new_left{ old_left + pos.X - drag_image.ActualWidth() / 2 };
							auto new_top{ old_top + pos.Y - drag_image.ActualHeight() / 2 };
							Controls::Canvas::SetLeft(drag_image, static_cast<mqf32>(new_left));
							Controls::Canvas::SetTop(drag_image, static_cast<mqf32>(new_top));
						}
						});
					// �����¼�
					drag_image.PointerWheelChanged([ ] (IInspectable const& sender, Input::PointerRoutedEventArgs const& e) {
						auto drag_image{ sender.as<Controls::Image>() };
						auto cp{ e.GetCurrentPoint(drag_image) };
						auto prop{ cp.Properties() };
						if (!prop.IsHorizontalMouseWheel()) {
							auto delta{ prop.MouseWheelDelta() };
							if (delta > 0) { // ÿ�η�1.1��
								drag_image.Width(drag_image.ActualWidth() * 1.1);
								drag_image.Height(drag_image.ActualHeight() * 1.1);
							}
							else { // ÿ����0.9��
								drag_image.Width(drag_image.ActualWidth() * 0.9);
								drag_image.Height(drag_image.ActualHeight() * 0.9);
							}
						}
						});
					// �Ҽ��¼�
					drag_image.RightTapped([ ] (IInspectable const& sender, auto) {
						auto drag_image{ sender.as<Controls::Image>() };
						auto items{ drag_image.Parent().as<Controls::Canvas>().Children() };
						mqui32 index{ };
						items.IndexOf(drag_image, index);
						items.RemoveAt(index);
						});

					items.Append(drag_image);
				}
				else {
					co_await ui_thread;
				}
			}
		}
	}

	// ƴͼ
	static IAsyncAction Canvas_Jigsaw_Save(ToolsPage::JigsawSubPage* sp) noexcept {
		Windows::Storage::Pickers::FileSavePicker savePicker;
		savePicker.SuggestedStartLocation(Windows::Storage::Pickers::PickerLocationId::Desktop);
		auto filters{ savePicker.FileTypeChoices() };
		filters.Insert(L"JPGͼƬ", single_threaded_observable_vector<hstring>({ L".jpg" }));
		savePicker.SuggestedFileName(util::RDString<hstring>(L"JigsawSubPage.SaveName"));
		util::InitializeDialog(savePicker, Global.ui_hwnd);

		if (Windows::Storage::StorageFile file{ co_await savePicker.PickSaveFileAsync() }) {
			auto print_canvas_size{ sp->switch_mode.IsOn() ? Global.c_printCanvasSize : Global.c_A4Size };
			auto scale{ print_canvas_size.width / sp->canvas.ActualWidth() }; // ���ű���
			std::vector<std::pair<std::wstring, mqrect>> rects;
			for (auto item : sp->canvas.Children()) {
				auto drag_image{ item.as<Controls::Image>() };
				rects.emplace_back(drag_image.Tag().as<hstring>(), mqrect{
					static_cast<mqui32>(Controls::Canvas::GetLeft(drag_image)),
					static_cast<mqui32>(Controls::Canvas::GetTop(drag_image)),
					static_cast<mqui32>(drag_image.ActualWidth()),
					static_cast<mqui32>(drag_image.ActualHeight()) } * scale);
			}

			winrt::apartment_context ui_thread;
			co_await winrt::resume_background();

			Media::GDI::Image print_canvas(print_canvas_size, Media::Colors::White); // ��ӡ����
			print_canvas.DPI({ 300U, 300U });
			for (auto& [file, rect] : rects) {
				Media::GDI::Image image{ file };
				image.DPI({ 300U, 300U });
				print_canvas.DrawImage(image, { rect.left, rect.top }, { rect.width, rect.height }, Media::GDI::QUALITY_MODE);
			}
			print_canvas.Save(file.Path(), Media::GDI::ImageFormat::JPG);

			co_await ui_thread;
		}
	}

	// ��ʼ��
	static void InitializeJigsawSubPage(IInspectable* subPage) noexcept {
		auto sp{ ToolsPage::JigsawSubPage::From(subPage) };
		Application::LoadComponent(*sp, Uri{ L"ms-appx:///Xaml/Tools/JigsawSubPage.xaml" });
		sp->Bind(sp->canvas, L"Canvas_Jigsaw");
		sp->Bind(sp->switch_mode, L"Switch_Mode");
		sp->Bind(sp->button_import, L"Button_Import");
		sp->Bind(sp->button_clear, L"Button_Clear");
		sp->Bind(sp->button_save, L"Button_Save");

		sp->switch_mode.Toggled([sp] (auto, auto) {
			Canvas_Jigsaw_Resize(sp);
			Canvas_Jigsaw_Clear(sp);
			});
		sp->canvas.SizeChanged([sp] (auto, auto) {
			Canvas_Jigsaw_Resize(sp);
			});
		sp->button_import.Click([sp] (auto, auto) -> IAsyncAction {
			co_await Canvas_Jigsaw_Import(sp);
			});
		sp->button_clear.Click([sp] (auto, auto) {
			Canvas_Jigsaw_Clear(sp);
			});
		sp->button_save.Click([sp] (auto, auto) -> IAsyncAction {
			co_await Canvas_Jigsaw_Save(sp);
			});
	}

	// ����
	static void UpdateJigsawSubPage(IInspectable* subPage) noexcept {
		auto sp{ ToolsPage::JigsawSubPage::From(subPage) };
		if (sp->switch_mode.IsOn()) { // �����A6ģʽ���ó�A4, IsOn�ᴥ��Toggled�����
			sp->switch_mode.IsOn(false);
		}
		else { // ����Ѿ���A4ģʽֱ�����
			Canvas_Jigsaw_Clear(sp);
		}
	}

	/*  ����PDF  */

	// ����
	static IAsyncAction ToPDF_Import(ToolsPage::ToPDFSubPage* sp) noexcept {
		Windows::Storage::Pickers::FileOpenPicker openPicker;
		openPicker.SuggestedStartLocation(Windows::Storage::Pickers::PickerLocationId::Desktop);
		openPicker.ViewMode(Windows::Storage::Pickers::PickerViewMode::Thumbnail);
		auto filters{ openPicker.FileTypeFilter() };
		filters.Append(L".jpg");
		filters.Append(L".png");
		filters.Append(L".bmp");
		util::InitializeDialog(openPicker, Global.ui_hwnd);

		if (Windows::Foundation::Collections::IVectorView<Windows::Storage::StorageFile>
			files_view{ co_await openPicker.PickMultipleFilesAsync() }; files_view.Size() > 0U) {
			auto items{ sp->gv_pdf.Items() };
			for (auto file_view : files_view) {
				auto path{ file_view.Path() };
				Controls::Image image;
				image.Width(Global.c_ToPDFImageSize.width);
				image.Height(Global.c_ToPDFImageSize.height);
				image.Source(util::FileToBMP(path, Global.c_ToPDFImageSize));
				image.Tag(box_value(path));
				items.Append(image);
			}
		}
	}

	// ����
	static IAsyncAction ToPDF_Save(ToolsPage::ToPDFSubPage* sp) noexcept {
		Windows::Storage::Pickers::FileSavePicker savePicker;
		savePicker.SuggestedStartLocation(Windows::Storage::Pickers::PickerLocationId::Desktop);
		auto filters{ savePicker.FileTypeChoices() };
		filters.Insert(L"PDF�ĵ�", single_threaded_observable_vector<hstring>({ L".pdf" }));
		savePicker.SuggestedFileName(util::RDString<hstring>(L"ToPDFSubPage.SaveName"));
		util::InitializeDialog(savePicker, Global.ui_hwnd);

		if (Windows::Storage::StorageFile file{ co_await savePicker.PickSaveFileAsync() }) {
			std::vector<std::wstring> files;
			for (auto item : sp->gv_pdf.Items()) {
				files.emplace_back(item.as<FrameworkElement>().Tag().as<hstring>());
			}
			winrt::apartment_context ui_thread;
			co_await winrt::resume_background();

			Storage::PDF pdf;
			for (auto& file : files) {
				Media::GDI::Image image{ file };
				auto scale{ image.DPI().width / 72.0 };
				auto size{ image.Size() / scale };

				auto page{ pdf.AddPage(MasterQian::Storage::PDF::PageConfig{ .page_size = { size.width, size.height } }) };
				auto page_image{ pdf.LoadPNGImage(image.Save(MasterQian::Media::GDI::ImageFormat::PNG)) };
				page.AddImage(page_image, { 0U, 0U }, size);
			}
			pdf.Save(file.Path());

			co_await ui_thread;
		}
	}

	// ��ʼ��
	static void InitializeToPDFSubPage(IInspectable* subPage) noexcept {
		auto sp{ ToolsPage::ToPDFSubPage::From(subPage) };
		Application::LoadComponent(*sp, Uri{ L"ms-appx:///Xaml/Tools/ToPDFSubPage.xaml" });
		sp->Bind(sp->button_import, L"Button_Import");
		sp->Bind(sp->button_delete, L"Button_Delete");
		sp->Bind(sp->button_clear, L"Button_Clear");
		sp->Bind(sp->button_save, L"Button_Save");
		sp->Bind(sp->gv_pdf, L"GV_PDF");
		sp->button_import.Click([sp] (auto, auto) -> IAsyncAction {
			co_await ToPDF_Import(sp);
			});
		sp->button_delete.Click([sp] (auto, auto) {
			if (auto index{ sp->gv_pdf.SelectedIndex() }; index != -1) {
				sp->gv_pdf.Items().RemoveAt(index);
			}
			});
		sp->button_clear.Click([sp] (auto, auto) {
			sp->gv_pdf.Items().Clear();
			});
		sp->button_save.Click([sp] (auto, auto) -> IAsyncAction {
			co_await ToPDF_Save(sp);
			});
	}

	// ����
	static void UpdateToPDFSubPage(IInspectable* subPage) noexcept {
		auto sp{ ToolsPage::ToPDFSubPage::From(subPage) };
		sp->gv_pdf.Items().Clear();
	}
}

namespace winrt::hyzjkz::implementation {
	ToolsPage::ToolsPage() {
		InitializeComponent();
		NVI_Home().Tag(WinRT::Args::box(&Home, nullptr));
		InitializeHomeSubPage(&Home);
		NVI_IDCard().Tag(WinRT::Args::box(&IDCard, &UpdateIDCardSubPage));
		InitializeIDCardSubPage(&IDCard);
		NVI_Jigsaw().Tag(WinRT::Args::box(&Jigsaw, &UpdateJigsawSubPage));
		InitializeJigsawSubPage(&Jigsaw);
		NVI_ToPDF().Tag(WinRT::Args::box(&ToPDF, &UpdateToPDFSubPage));
		InitializeToPDFSubPage(&ToPDF);

		Loaded([this] (auto, auto) {
			NV_Main().SelectedItem(NVI_Home());
			});
	}

	// ��������
	F_EVENT void ToolsPage::NV_Main_SelectionChanged(Controls::NavigationView const&, Controls::NavigationViewSelectionChangedEventArgs const& args) {
		auto tag{ args.SelectedItem().as<Controls::NavigationViewItem>().Tag() };
		if (tag != nullptr) {
			auto args{ tag.as<WinRT::Args>() };
			auto [subPage, func] = args.unbox<IInspectable*, WinRT::SubPageFunc>();
			Frame_Main().Content(*subPage);
			if (func) {
				func(subPage);
			}
		}
	}
}