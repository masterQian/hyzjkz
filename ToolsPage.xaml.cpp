#include "pch.h"
#include "ToolsPage.xaml.h"
#if __has_include("ToolsPage.g.cpp")
#include "ToolsPage.g.cpp"
#endif

namespace winrt::hyzjkz::implementation {
	/*  首页  */

	// 初始化
	static void InitializeHomeSubPage(SubPageBase* subPage) noexcept {
		auto sp{ subPage->to<ToolsPage::HomeSubPage>() };
		auto image{ sp->content() };
		image.Stretch(Media::Stretch::Fill);
		image.Source(util::BinToBMP(MasterQian::System::Process::GetResource(R_SIZEINFORMATION), { }, true));
	}

	/*  身份证拼接  */

	// 选择
	static Windows::Foundation::IAsyncAction IDCard_Select_Click(ToolsPage::IDCardSubPage* sp, IInspectable const& sender) noexcept {
		auto button{ sender.as<Controls::Button>() };

		// 未导入照片状态
		if (button.Tag().as<hstring>() == L"false") {
			// 准备对话框
			Windows::Storage::Pickers::FileOpenPicker openPicker;
			openPicker.SuggestedStartLocation(Windows::Storage::Pickers::PickerLocationId::Desktop);
			openPicker.ViewMode(Windows::Storage::Pickers::PickerViewMode::Thumbnail);
			auto filters{ openPicker.FileTypeFilter() };
			filters.Append(L".jpg");
			filters.Append(L".png");
			filters.Append(L".bmp");
			util::InitializeDialog(openPicker, Global.ui_hwnd);

			// 接受照片
			if (Windows::Storage::StorageFile file{ co_await openPicker.PickSingleFileAsync() }) {
				Controls::Image image;
				image.Width(Global.c_IDCardPreviewSize.width);
				image.Height(Global.c_IDCardPreviewSize.height);
				image.Stretch(Media::Stretch::Fill);
				image.Tag(box_value(file.Path()));
				image.Source(util::FileToBMP(file.Path(), Global.c_IDCardPreviewSize, true));
				button.Content(image);
				button.Tag(box_value(L"true"));

				// 若两面都准备好
				if (sp->button_front.Tag().as<hstring>() == L"true" && sp->button_back.Tag().as<hstring>() == L"true") {
					auto front_path{ sp->button_front.Content().as<FrameworkElement>().Tag().as<hstring>() };
					auto back_path{ sp->button_back.Content().as<FrameworkElement>().Tag().as<hstring>() };

					sp->button_clear.IsEnabled(false);
					sp->button_work.IsEnabled(false);
					sp->preview_text.Text(util::RDString<hstring>(L"IDCardSubPage.Previewing.Text"));

					winrt::apartment_context ui_thread;
					co_await winrt::resume_background();

					MasterQian::Media::GDI::Image front_image{ front_path }, back_image{ back_path };
					MasterQian::Media::GDI::Image combine_image{ front_image.Combine<false>(back_image,
						{ 0U, 0U, front_image.Width(), front_image.Height() / 2U },
						{ 0U, back_image.Height() / 2U, back_image.Width(), back_image.Height() / 2U },
						MasterQian::Media::FAST_MODE).Thumbnail({ 280, 350 }) };

					co_await ui_thread;

					sp->button_clear.IsEnabled(true);
					sp->button_work.IsEnabled(true);
					sp->preview_text.Text(util::RDString<hstring>(L"IDCardSubPage.Preview.Text"));

					sp->image_preview.Source(util::StreamToBMP(combine_image.SaveToUnsafeStream(MasterQian::Media::ImageFormat::BMP)));
				}
			}
		}
	}

	// 重置按钮
	inline static void IDCard_Reset(Controls::Button const& button) noexcept {
		if (button.Tag().as<hstring>() == L"true") {
			Controls::SymbolIcon icon;
			icon.Symbol(Controls::Symbol::Add);
			button.Content(icon);
			button.Tag(box_value(L"false"));
		}
	}

	// 清除
	static void IDCard_Clear_Click(ToolsPage::IDCardSubPage* sp) {
		IDCard_Reset(sp->button_front);
		IDCard_Reset(sp->button_back);
		sp->image_preview.Source(nullptr);
	}

	// 合成
	static Windows::Foundation::IAsyncAction IDCard_Work_Click(ToolsPage::IDCardSubPage* sp) {
		if (sp->button_front.Tag().as<hstring>() == L"true" && sp->button_back.Tag().as<hstring>() == L"true") {
			Windows::Storage::Pickers::FileSavePicker savePicker;
			savePicker.SuggestedStartLocation(Windows::Storage::Pickers::PickerLocationId::Desktop);
			auto filters{ savePicker.FileTypeChoices() };
			filters.Insert(L"JPG图片", single_threaded_observable_vector<hstring>({ L".jpg" }));
			savePicker.SuggestedFileName(util::RDString<hstring>(L"IDCardSubPage.SaveName"));
			util::InitializeDialog(savePicker, Global.ui_hwnd);

			auto front_path{ sp->button_front.Content().as<FrameworkElement>().Tag().as<hstring>() };
			auto back_path{ sp->button_back.Content().as<FrameworkElement>().Tag().as<hstring>() };

			if (Windows::Storage::StorageFile file{ co_await savePicker.PickSaveFileAsync() }) {
				IDCard_Clear_Click(sp);

				winrt::apartment_context ui_thread;
				co_await winrt::resume_background();

				MasterQian::Media::GDI::Image front_image{ front_path }, back_image{ back_path };
				front_image.Combine<false>(back_image,
					{ 0U, 0U, front_image.Width(), front_image.Height() / 2U },
					{ 0U, back_image.Height() / 2U, back_image.Width(), back_image.Height() / 2U },
					MasterQian::Media::QUALITY_MODE).Save(file.Path(), MasterQian::Media::ImageFormat::JPG);

				co_await ui_thread;
			}
		}
		else {
			co_await Global.ui_window->as<hyzjkz::MainWindow>().ShowTipDialog(util::RDString<hstring>(L"IDCardSubPage.Tip.AddPhoto"));
		}
	}

	// 初始化
	static void InitializeIDCardSubPage(SubPageBase* subPage) noexcept {
		auto sp{ subPage->to<ToolsPage::IDCardSubPage>() };
		Application::LoadComponent(*sp, Windows::Foundation::Uri{ L"ms-appx:///Xaml/Tools/IDCardSubPage.xaml" });
		auto grid{ sp->content() };
		sp->button_front = grid.FindName(L"Button_Front").as<Controls::Button>();
		sp->button_back = grid.FindName(L"Button_Back").as<Controls::Button>();
		sp->button_clear = grid.FindName(L"Button_Clear").as<Controls::Button>();
		sp->button_work = grid.FindName(L"Button_Work").as<Controls::Button>();
		sp->preview_text = grid.FindName(L"Preview_Text").as<Controls::TextBlock>();
		sp->image_preview = grid.FindName(L"Image_Preview").as<Controls::Image>();

		sp->button_front.Click([sp] (IInspectable const& sender, auto) -> Windows::Foundation::IAsyncAction {
			co_await IDCard_Select_Click(sp, sender);
			});
		sp->button_back.Click([sp] (IInspectable const& sender, auto) -> Windows::Foundation::IAsyncAction {
			co_await IDCard_Select_Click(sp, sender);
			});
		sp->button_clear.Click([sp] (auto, auto) {
			IDCard_Clear_Click(sp);
			});
		sp->button_work.Click([sp] (auto, auto) -> Windows::Foundation::IAsyncAction {
			co_await IDCard_Work_Click(sp);
			});
	}

	// 更新
	static void UpdateIDCardSubPage(SubPageBase* subPage) noexcept {
		IDCard_Clear_Click(subPage->to<ToolsPage::IDCardSubPage>());
	}
}

namespace winrt::hyzjkz::implementation {
	ToolsPage::ToolsPage() {
		using Args = MasterQian::WinRT::Args;
		InitializeComponent();
		NVI_Home().Tag(Args::box(&Home, nullptr));
		InitializeHomeSubPage(&Home);
		NVI_IDCard().Tag(Args::box(&IDCard, &UpdateIDCardSubPage));
		InitializeIDCardSubPage(&IDCard);

		Loaded([this] (auto, auto) {
			NV_Main().SelectedItem(NVI_Home());
			});
	}

	// 更换导航
	F_EVENT void ToolsPage::NV_Main_SelectionChanged(Controls::NavigationView const&, Controls::NavigationViewSelectionChangedEventArgs const& args) {
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