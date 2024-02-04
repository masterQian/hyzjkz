#include "pch.h"
#include "HistoryPage.xaml.h"
#if __has_include("HistoryPage.g.cpp")
#include "HistoryPage.g.cpp"
#endif

namespace winrt::hyzjkz::implementation {
	// 更新工具链菜单
	static void UpdateToolChain(HistoryPage* page) noexcept {
		auto items{ page->GV_Photos_Menu_Link().Items() };
		items.Clear();
		auto tools{ Global.cfg.Get(GlobalConfig::TOOLS) };
		for (auto& [name, _] : tools) {
			Storage::Path path{ tools.get(name) };
			if (path.IsFile()) {
				Controls::MenuFlyoutItem menu;
				Controls::ImageIcon icon;
				icon.Source(util::StreamToBMP(Media::GDI::Image(path, 0U).SaveToUnsafeStream(Media::GDI::ImageFormat::PNG), { }, true));
				menu.Icon(icon);
				menu.Text(name);
				menu.Tag(box_value(L"Link|" + name));
				menu.Click({ page, &HistoryPage::MenuItem_Click });
				items.Append(menu);
			}
		}
	}

	// 更新照相报表数据
	static void UpdateRecordPhotoData(HistoryPage* page, mqui16 year, mqui8 month, mqui8 day) noexcept {
		std::wstring filename{ std::to_wstring(year) + L".bin" };
		auto data_path{ Global.c_dataPath / filename };
		Bin bin{ data_path.Read() };
		if (RecordData::CheckSize(bin)) {
			RecordData::From(bin).Set(month, day, page->mPhotoNum, page->mPhotoNum * Global.cfg.Get<GlobalConfig::UNIT_PRICE>());
			data_path.Write(bin);
		}
	}

	// 更新复印报表数据
	static void UpdateRecordCopyData(HistoryPage* page, mqui32 value) noexcept {
		auto cur_date{ util::DateTimeToLocal(page->CDP_Main().Date().as<Windows::Foundation::DateTime>()) };
		std::wstring filename{ std::to_wstring(cur_date.year) + L".bin" };
		auto data_path{ Global.c_dataPath / filename };
		MasterQian::Bin bin{ data_path.Read() };
		if (RecordData::CheckSize(bin)) {
			RecordData::From(bin).Add(cur_date.month, cur_date.day, value);
			data_path.Write(bin);
		}
	}

	// 刷新
	static void Refresh(HistoryPage* page, bool updateRecord) noexcept {
		// 先清空列表, 批量同时重绘
		auto photo_view{ page->GV_Photos() };
		photo_view.ItemsSource(nullptr);
		auto new_photos{ single_threaded_observable_vector<hyzjkz::ModelPhoto>() };

		auto cur_date{ util::DateTimeToLocal(page->CDP_Main().Date().as<Windows::Foundation::DateTime>()) };
		auto date_str{ cur_date.formatDate() };
		auto photo_path{ Global.c_photoPath / date_str }; // [照片路径]/20231231
		auto thumb_path{ Global.c_thumbPath / date_str }; // [缩略图路径]/20231231
		thumb_path.Create(); // 确保缩略图目录存在

		mqui32 photo_num{ }; // 照片数量

		// 遍历照片文件夹
		for (auto& photo_file : photo_path.EnumFolder(L"*.jpg")) {
			auto filename{ photo_file.Name() };
			auto thumb_file{ thumb_path / filename };
			if (!thumb_file.Exists()) { // 缩略图不存在
				// 创建缩略图
				Media::GDI::Image(photo_file).Thumbnail(Global.c_photoThumbSize).Save(thumb_file, Media::GDI::ImageFormat::JPG);
			}

			new_photos.Append(hyzjkz::ModelPhoto{ date_str, filename }); // 添加新照片

			++photo_num;
		}

		photo_view.ItemsSource(new_photos); // 更新照片数组同时刷新UI
		page->PhotoNum(photo_num); // 更新照片数量同时刷新UI

		// 更新报表数据
		if (updateRecord) {
			UpdateRecordPhotoData(page, cur_date.year, cur_date.month, cur_date.day);
		}
	}

	// 另存为照片
	static IAsyncAction Menu_Save(hyzjkz::ModelPhoto item) noexcept {
		Windows::Storage::Pickers::FileSavePicker savePicker;
		auto filters{ savePicker.FileTypeChoices() };
		filters.Insert(L"JPG图片", single_threaded_observable_vector<hstring>({ L".jpg" }));
		filters.Insert(L"PNG图片", single_threaded_observable_vector<hstring>({ L".png" }));
		filters.Insert(L"BMP图片", single_threaded_observable_vector<hstring>({ L".bmp" }));
		savePicker.SuggestedStartLocation(Windows::Storage::Pickers::PickerLocationId::Desktop);
		savePicker.SuggestedFileName(MasterQian::Timestamp{ }.local().formatDateTime());
		util::InitializeDialog(savePicker, Global.ui_hwnd);

		if (Windows::Storage::StorageFile file{ co_await savePicker.PickSaveFileAsync() }) {
			winrt::apartment_context ui_thread;
			co_await winrt::resume_background();

			Media::GDI::Image image{ item.PhotoPath() };
			Media::GDI::ImageFormat format{ Media::GDI::ImageFormat::JPG };
			image.DPI({ 300U, 300U });
			if (file.FileType() == L".png") {
				format = Media::GDI::ImageFormat::PNG;
			}
			else if (file.FileType() == L".bmp") {
				format = Media::GDI::ImageFormat::BMP;
			}
			image.Save(file.Path(), format);

			co_await ui_thread;
		}
	}

	// 复制照片
	static IAsyncAction Menu_Copy(HistoryPage* page, hyzjkz::ModelPhoto item) noexcept {
		winrt::apartment_context ui_thread;
		co_await winrt::resume_background();

		Storage::Path photo_path{ item.PhotoPath() };
		Storage::Path thumb_path{ item.ThumbPath() };
		auto filename{ Timestamp{ }.local().formatDateTime() + L"00.jpg" };
		photo_path.CopyRename(photo_path.Parent() / filename, true);
		thumb_path.CopyRename(thumb_path.Parent() / filename, true);

		co_await ui_thread;

		Refresh(page, true);
	}

	// 删除照片
	static IAsyncAction Menu_Delete(HistoryPage* page, hyzjkz::ModelPhoto item) noexcept {
		winrt::apartment_context ui_thread;
		co_await winrt::resume_background();
		
		Storage::Path(item.PhotoPath()).DeleteToRecycleBin();
		Storage::Path(item.ThumbPath()).Delete();

		co_await ui_thread;

		Refresh(page, true);
	}

	// 打印照片
	static void Menu_Print(hyzjkz::ModelPhoto item) noexcept {
		Global.ui_window->as<hyzjkz::MainWindow>().NavigateToPrivatePage(
			UpdateFlag::PRIVATE_PRINT, box_value(item.PhotoPath()));
	}

	// 旋转
	static IAsyncAction Menu_Rotate(HistoryPage* page, hyzjkz::ModelPhoto item, std::wstring_view tag) noexcept {
		std::wstring photo_path{ item.PhotoPath() };
		std::wstring thumb_path{ item.ThumbPath() };
		std::wstring type{ tag };

		Media::GDI::Image image{ photo_path };
		if (type == L"RotateLeft") image.RotateLeft();
		else if (type == L"RotateRight") image.RotateRight();
		else if (type == L"RotateFlipX") image.FlipX();
		else if (type == L"RotateFlipY") image.FlipY();
		Media::GDI::Image thumb_image{ image.Thumbnail(Global.c_photoThumbSize) };

		auto dialog{ page->PreviewDialog() };
		auto stream{ thumb_image.SaveToUnsafeStream(Media::GDI::ImageFormat::BMP) };
		dialog.Content().as<Controls::Image>().Source(util::StreamToBMP(stream));

		auto result{ co_await dialog.ShowAsync() };
		if (result == Controls::ContentDialogResult::Primary) {
			winrt::apartment_context ui_thread;
			co_await winrt::resume_background();

			image.Save(photo_path, Media::GDI::ImageFormat::JPG);
			thumb_image.Save(thumb_path, Media::GDI::ImageFormat::JPG);

			co_await ui_thread;

			Refresh(page, false);
		}
	}

	// DPI
	static IAsyncAction Menu_DPI(HistoryPage* page, hyzjkz::ModelPhoto item) noexcept {
		std::wstring photo_path{ item.PhotoPath() };
		hstring str{ co_await Global.ui_window->as<hyzjkz::MainWindow>()
			.ShowInputDialog(util::RDString<hstring>(L"HistoryPage.Tip.InputDPI")) };
		if (!str.empty()) {
			mqstr end{ };
			auto dpi{ wcstoul(str.data(), &end, 10) };
			if (dpi && !*end) {
				winrt::apartment_context ui_thread;
				co_await winrt::resume_background();

				Media::GDI::Image image{ photo_path };
				image.DPI({ static_cast<mqui32>(dpi), static_cast<mqui32>(dpi) });
				image.Save(photo_path, Media::GDI::ImageFormat::JPG);

				co_await ui_thread;
			}
			else {
				co_await Global.ui_window->as<hyzjkz::MainWindow>()
					.ShowTipDialog(util::RDString<hstring>(L"HistoryPage.Tip.InputInvalid"));
			}
		}
	}

	// 边框
	static IAsyncAction Menu_Border(HistoryPage* page, hyzjkz::ModelPhoto item) noexcept {
		static auto ImageBorderImpl = [ ] (Media::GDI::Image& image,
			mqui32 size, Windows::UI::Color color, Media::GDI::AlgorithmModes mode) {
				auto border_x{ (image.Width() >> 8) * size };
				auto border_y{ (image.Height() >> 8) * size };
				if (border_x == 0) border_x = 1;
				if (border_y == 0) border_y = 1;
				return image.Border({ border_x, border_y, border_x, border_y }, { color.R, color.G, color.B, color.A }, mode);
			};

		std::wstring photo_path{ item.PhotoPath() };
		std::wstring thumb_path{ item.ThumbPath() };

		auto dialog{ page->MenuBorderDialog() };
		auto border_image{ page->MBD_Image() };
		border_image.Source(util::FileToBMP(thumb_path));
		border_image.Tag(box_value(item.PhotoPath()));
		auto border_box{ page->MBD_BorderWidth() };
		border_box.Value(1.0);
		auto border_color{ page->MBD_BorderColor() };
		border_color.Color(Windows::UI::Colors::White());

		dialog.PrimaryButtonClick([page] (auto,
			Controls::ContentDialogButtonClickEventArgs const& args) {
				// 图像边框预览
				auto size{ static_cast<mqui32>(page->MBD_BorderWidth().Value()) };
				auto color{ page->MBD_BorderColor().Color() };

				Media::GDI::Image image{ unbox_value<hstring>(page->MBD_Image().Tag()) };
				auto stream{ ImageBorderImpl(image, size, color, Media::GDI::FAST_MODE).
					Thumbnail(Global.c_photoPreviewSize).SaveToUnsafeStream(Media::GDI::ImageFormat::BMP) };
				page->MBD_Image().Source(util::StreamToBMP(stream));
				args.Cancel(true);
			});

		auto result{ co_await dialog.ShowAsync() };
		if (result == Controls::ContentDialogResult::Secondary) {
			// 尺寸和颜色
			auto size{ static_cast<mqui32>(border_box.Value()) };
			auto color{ border_color.Color() };

			winrt::apartment_context ui_thread;
			co_await winrt::resume_background();

			// 图像边框保存
			Media::GDI::Image image{ photo_path };
			image = ImageBorderImpl(image, size, color, Media::GDI::QUALITY_MODE);
			image.Save(photo_path, Media::GDI::ImageFormat::JPG);
			image.Thumbnail(Global.c_photoThumbSize).Save(thumb_path, Media::GDI::ImageFormat::JPG);

			co_await ui_thread;

			Refresh(page, false);
		}
	}

	// 黑白照
	static IAsyncAction Menu_BWPhoto(HistoryPage* page, hyzjkz::ModelPhoto item) noexcept {
		std::wstring photo_path{ item.PhotoPath() };
		std::wstring thumb_path{ item.ThumbPath() };

		Media::GDI::Image image{ photo_path };
		Media::GDI::Image thumb_image{ image.Thumbnail(Global.c_photoThumbSize) };
		thumb_image.GrayScale(Media::GDI::FAST_MODE);

		auto dialog{ page->PreviewDialog() };
		auto stream{ thumb_image.SaveToUnsafeStream(Media::GDI::ImageFormat::BMP) };
		dialog.Content().as<Controls::Image>().Source(util::StreamToBMP(stream));

		auto result{ co_await dialog.ShowAsync() };
		if (result == Controls::ContentDialogResult::Primary) {
			winrt::apartment_context ui_thread;
			co_await winrt::resume_background();

			image.GrayScale(Media::GDI::QUALITY_MODE);
			image.Save(photo_path, Media::GDI::ImageFormat::JPG);
			thumb_image.Save(thumb_path, Media::GDI::ImageFormat::JPG);

			co_await ui_thread;

			Refresh(page, false);
		}
	}

	// 身份证
	static IAsyncAction Menu_IDCard(HistoryPage* page, hyzjkz::ModelPhoto item) noexcept {
		std::wstring photo_path{ item.PhotoPath() };
		std::wstring thumb_path{ item.ThumbPath() };

		Media::GDI::Image image{ photo_path };
		auto [width, height] { image.Size() };
		Media::GDI::GDIText textInfo{ L"", width * 7 / image.DPI().width, Media::Colors::White, L"黑体" };

		auto dialog{ page->MenuIDCardDialog() };
		auto idcard_image{ page->MIDD_Image() };
		idcard_image.Source(util::FileToBMP(thumb_path));
		idcard_image.Tag(box_value(item.PhotoPath()));
		auto idcard_box{ page->MIDD_IDCardNumber() };
		idcard_box.Text(L"");
		auto idcard_color{ page->MIDD_IDCardColor() };
		idcard_color.Color(Windows::UI::Colors::White());

		auto idcard_bottom{ page->MIDD_IDCardBottom() };
		idcard_bottom.Value(static_cast<mqui32>(height * 31.0 / 32));
		auto idcard_fontsize{ page->MIDD_IDCardFontSize() };
		idcard_fontsize.Value(textInfo.size);

		auto idcard_mode{ page->MIDD_IDCardMode() };
		idcard_mode.IsOn(false);

		page->MIDD_IDCardPlusX().Click([page] (auto, auto) {
			auto idcard_box{ page->MIDD_IDCardNumber() };
			auto id_number{ idcard_box.Text() };
			if (id_number.size() < 18U) {
				idcard_box.Text(id_number + L"X");
				idcard_box.Focus(Microsoft::UI::Xaml::FocusState::Keyboard);
				idcard_box.Select(id_number.size() + 1ULL, 0);
			}
			});

		dialog.PrimaryButtonClick([page] (auto,
			Controls::ContentDialogButtonClickEventArgs const& args) {
				// 图像身份证预览
				Media::GDI::Image image{ unbox_value<hstring>(page->MIDD_Image().Tag()) };
				auto color{ page->MIDD_IDCardColor().Color() };
				Media::GDI::GDIText textInfo{ page->MIDD_IDCardNumber().Text().data(),
					static_cast<mqui32>(page->MIDD_IDCardFontSize().Value()),
					Media::Color(color.R, color.G, color.B, color.A), L"黑体" };
				auto bottom{ static_cast<mqui32>(page->MIDD_IDCardBottom().Value()) };
				auto isExtend{ page->MIDD_IDCardMode().IsOn() };

				auto extend_height{ image.Height() * 5U / 64U };
				if (isExtend) {
					image = image.Border({ 0, 0, 0, extend_height }, Media::Colors::White, Media::GDI::FAST_MODE);
					bottom += extend_height;
				}
				mqpoint point{ image.Width() / 2, bottom };
				image.DrawString(point, textInfo, Media::GDI::FAST_MODE);
				auto stream{ image.Thumbnail(Global.c_photoPreviewSize).SaveToUnsafeStream(Media::GDI::ImageFormat::BMP) };
				page->MIDD_Image().Source(util::StreamToBMP(stream));
				args.Cancel(true);
			});

		auto result{ co_await dialog.ShowAsync() };
		if (result == Controls::ContentDialogResult::Secondary) {
			// 身份证号,颜色,底边距离,字体大小
			textInfo.content = idcard_box.Text().data();
			textInfo.size = static_cast<mqui32>(idcard_fontsize.Value());
			auto color{ idcard_color.Color() };
			textInfo.color = Media::Color(color.R, color.G, color.B, color.A);
			auto bottom{ static_cast<mqui32>(idcard_bottom.Value()) };
			auto isExtend{ idcard_mode.IsOn() };

			auto extend_height{ image.Height() * 5U / 64U };
			if (isExtend) {
				image = image.Border({ 0, 0, 0, extend_height }, Media::Colors::White, Media::GDI::QUALITY_MODE);
				bottom += extend_height;
			}
			mqpoint point{ image.Width() / 2, bottom };

			winrt::apartment_context ui_thread;
			co_await winrt::resume_background();

			// 图像身份证保存
			image.DrawString(point, textInfo, Media::GDI::QUALITY_MODE);
			image.Save(photo_path, Media::GDI::ImageFormat::JPG);
			image.Thumbnail(Global.c_photoThumbSize).Save(thumb_path, Media::GDI::ImageFormat::JPG);

			co_await ui_thread;

			Refresh(page, false);
		}
	}

	// [协程] 外链
	static IAsyncAction Menu_Link(HistoryPage* page, hyzjkz::ModelPhoto item, std::wstring_view name) noexcept {
		// 照片路径, 缩略图路径, 临时路径, 外链路径
		Storage::Path photo_path{ item.PhotoPath() };
		Storage::Path thumb_path{ item.ThumbPath() };
		Storage::Path link_path{ name };
		// 复制照片到临时路径
		photo_path.CopyRename(Global.c_tempPhotoPath, true);

		// 打开外链锁
		page->NotAtExternalLink(false);

		// 异步运行外链
		winrt::apartment_context ui_thread;
		co_await winrt::resume_background();

		auto ret{ System::Process::Execute(link_path, Global.c_tempPhotoPath, true, true) };

		co_await ui_thread;

		// 关闭外链锁
		page->NotAtExternalLink(true);

		// 确认结果对话框
		auto dialog{ page->PreviewDialog() };

		auto priview_bmp{ util::FileToBMP(Global.c_tempPhotoPath, Global.c_photoPreviewSize) };
		dialog.Content().as<Controls::Image>().Source(priview_bmp);

		auto result{ co_await dialog.ShowAsync() };
		if (result == Controls::ContentDialogResult::Primary) {
			// 复制临时照片到照片路径
			Global.c_tempPhotoPath.CopyRename(photo_path, true);
			// 生成缩略图
			Media::GDI::Image(photo_path).Thumbnail(Global.c_photoThumbSize).Save(thumb_path, Media::GDI::ImageFormat::JPG);

			Refresh(page, false);
		}
		// 删除临时照片
		Global.c_tempPhotoPath.Delete();
	}

	// 导入照片
	static void ImportPhotos(mqhandle arg, std::vector<std::wstring> const& files) noexcept {
		auto page{ static_cast<HistoryPage*>(arg) };
		auto date_str{ util::DateTimeToLocal(page->CDP_Main().Date().as<Windows::Foundation::DateTime>()).formatDate() };
		auto photo_path{ Global.c_photoPath / date_str };
		photo_path.Create();
		auto thumb_path{ Global.c_thumbPath / date_str };
		thumb_path.Create();

		mqui32 index{ 10U }; // 从10开始省的一位数转字符串还要添前置0
		for (auto& file : files) {
			std::wstring filename{ Timestamp{ }.local().formatDateTime() + std::to_wstring(index++) };
			filename += L".jpg";
			Media::GDI::Image image(file);
			image.Save(photo_path / filename, Media::GDI::ImageFormat::JPG); // 复制照片及导出缩略图
			image.Thumbnail(Global.c_photoThumbSize).Save(thumb_path / filename, Media::GDI::ImageFormat::JPG);
		}

		Refresh(page, true);
	}

	// 文件监控
	static IAsyncAction StartFileSpy(HistoryPage* page) noexcept {
		winrt::apartment_context ui_thread;
		HANDLE hDir{ CreateFileW(std::wstring_view{ Global.c_cameraPhotoPath }.data(),
			FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			nullptr, OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, nullptr) };
		OVERLAPPED overlapped{ .hEvent = CreateEventW(nullptr, true, false, nullptr) };
		HANDLE events[2]{ Global.file_spy_event, overlapped.hEvent };
		mqbyte buffer[1024]{ };
		while (true) {
			co_await winrt::resume_background();
			ReadDirectoryChangesW(hDir, buffer, 1024, false,
				FILE_NOTIFY_CHANGE_FILE_NAME, nullptr, &overlapped, nullptr);
			DWORD tag{ WaitForMultipleObjects(2U, events, false, INFINITE) };
			co_await ui_thread;
			if (tag == WAIT_OBJECT_0) {
				break;
			}
			else if (tag == WAIT_OBJECT_0 + 1) {
				auto now_date{ Timestamp{ }.local() };
				auto cur_date{ util::DateTimeToLocal(page->CDP_Main().Date().as<Windows::Foundation::DateTime>()) };
				auto date_str{ now_date.formatDate() };

				PFILE_NOTIFY_INFORMATION fni{ };
				DWORD offset{ };
				do {
					fni = reinterpret_cast<PFILE_NOTIFY_INFORMATION>(buffer + offset);
					if (fni->Action == FILE_ACTION_RENAMED_NEW_NAME) {
						co_await winrt::resume_background();

						std::wstring_view filename{ fni->FileName, fni->FileNameLength };
						auto photo_path{ Global.c_photoPath / date_str };
						auto thumb_path{ Global.c_thumbPath / date_str };
						auto file{ Global.c_cameraPhotoPath / filename };
						file.Move(photo_path);
						Media::GDI::Image thumb_image(photo_path / filename);
						thumb_image.Thumbnail(Global.c_photoThumbSize).Save(thumb_path / filename, Media::GDI::ImageFormat::JPG);

						co_await ui_thread;
						if (now_date.compare_date(cur_date)) {
							Refresh(page, true);
						}
					}
					offset += fni->NextEntryOffset;
				} while (fni->NextEntryOffset);
			}
		}
		CloseHandle(overlapped.hEvent);
		CloseHandle(hDir);
		CloseHandle(Global.file_spy_event);
	}
}

namespace winrt::hyzjkz::implementation {
	HistoryPage::HistoryPage() {
		mNotAtExternalLink = true;
		mCoverTurnover = !Global.cfg.Get<GlobalConfig::SHOW_TURNOVER>();
		mPhotoNum = 0U;

		for (mqui64 i{ }, len{ bmp_nums.size() }; i < len; ++i) {
			bmp_nums[i] = util::BinToBMP(System::Process::GetResource(static_cast<mqui32>(R_NUM0 + i)), { 18U, 24U }, true);
		}
		bmp_default_img = util::BinToBMP(Global.res_default_img, Global.c_photoThumbSize, true);
	
		InitializeComponent();

		// 更新工具链菜单
		UpdateToolChain(this);

		// 初始化复印数据对话框
		auto copy_grid{ AddCopyDataGrid() };
		Application::LoadComponent(copy_grid, Uri{ L"ms-appx:///Xaml/CopyDataDialog.xaml" });
		for (auto child : copy_grid.Children()) {
			if (child.as<FrameworkElement>().Tag() != nullptr) {
				child.as<Controls::Button>().Click({ this, &HistoryPage::AddCopyData_Plus_Click });
			}
		}
		copy_grid.FindName(L"Button_AddCopyData_Set").as<Controls::Button>().Click({ this, &HistoryPage::AddCopyData_Set_Click });

		// 设置日期为今天
		CDP_Main().Date(winrt::clock::now());

		// 注册拖放钩子
		util::SetDropHook(&ImportPhotos, this);
		
		// 注册文件监控
		StartFileSpy(this);
	}

	// 切换页面
	F_EVENT void HistoryPage::OnNavigatedTo(Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& args) {
		FlagConverter flag{ .value = args.Parameter().as<mqui64>() };
		if (flag.flags[UpdateFlag::SHOW_TURNOVER]) {
			CoverTurnover(!Global.cfg.Get<GlobalConfig::SHOW_TURNOVER>());
		}
		if (flag.flags[UpdateFlag::LINK_MENU]) {
			UpdateToolChain(this);
		}
	}

	// 日历框被选择
	F_EVENT void HistoryPage::CDP_Main_DateChanged(Controls::CalendarDatePicker const&, Controls::CalendarDatePickerDateChangedEventArgs const& args) {
		auto old_date{ args.OldDate() }, new_date{ args.NewDate() };
		if (new_date != nullptr) { // 点击重复的日期时old_date为该日期, 而new_date==nullptr
			Refresh(this, false);
		}
		else if (old_date != nullptr) {
			auto t{ util::DateTimeToLocal(old_date.Value()) };
			mqchar buf[32]{ };
			wsprintfW(buf, L"%hu年%u月%u日", t.year, t.month, t.day);
			CDP_Main().PlaceholderText(buf);
		}
	}

	// 刷新
	F_EVENT void HistoryPage::Refresh_Click(IInspectable const&, RoutedEventArgs const&) {
		CDP_Main().Date(winrt::clock::now()); // 更新日期到今天
	}

	// 补差
	F_EVENT IAsyncAction HistoryPage::AddDefault_Click(IInspectable const&, RoutedEventArgs const&) {
		auto date_str{ util::DateTimeToLocal(CDP_Main().Date().as<Windows::Foundation::DateTime>()).formatDate() };
		winrt::apartment_context ui_thread;
		co_await winrt::resume_background();

		auto photo_file{ Global.c_photoPath / date_str };
		photo_file.Create();
		auto thumb_file{ Global.c_thumbPath / date_str };
		thumb_file.Create();
		auto filename{ Timestamp{ }.local().formatDateTime() + L"00.jpg" };
		photo_file /= filename;
		thumb_file /= filename;
		photo_file.Write(Global.res_default_img);
		thumb_file.Write(Global.res_default_img);

		co_await ui_thread;

		Refresh(this, true);
	}

	// 导入
	F_EVENT IAsyncAction HistoryPage::Import_Click(IInspectable const&, RoutedEventArgs const&) {
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
			ImportPhotos(this, files);
		}
	}

	// 添加复印数据
	F_EVENT IAsyncAction HistoryPage::AddCopyData_Click(IInspectable const&, RoutedEventArgs const&) {
		AddCopyDataGrid().FindName(L"NB_AddCopyData").as<Controls::NumberBox>().Value(0.0);
		co_await AddCopyDataDialog().ShowAsync();
	}

	// 添加复印数据, 点击
	F_EVENT void HistoryPage::AddCopyData_Plus_Click(IInspectable const& sender, RoutedEventArgs const&) {
		std::wstring_view str{ sender.as<Controls::Button>().Content().as<hstring>() };
		auto value{ std::wcstoul(str.substr(1U).data(), nullptr, 10) };
		UpdateRecordCopyData(this, value);
		AddCopyDataDialog().Hide();
	}

	// 添加复印数据, 自定义点击
	F_EVENT void HistoryPage::AddCopyData_Set_Click(IInspectable const&, RoutedEventArgs const&) {
		auto value{ static_cast<mqui32>(AddCopyDataGrid().FindName(L"NB_AddCopyData").as<Controls::NumberBox>().Value()) };
		UpdateRecordCopyData(this, value);
		AddCopyDataDialog().Hide();
	}

	// 双击查看大图
	F_EVENT IAsyncAction HistoryPage::GV_Photos_DoubleClick(IInspectable const&, Input::DoubleTappedRoutedEventArgs const&) {
		auto dialog{ ShowPictureDialog() };
		Storage::Path photo_path{ GV_Photos().SelectedItem().as<hyzjkz::ModelPhoto>().PhotoPath() };
		Media::GDI::Image image(photo_path);
		mqchar image_information[32]{ };
		auto size{ image.Size() };
		wsprintfW(image_information, L"尺寸: %u × %u 像素", size.width, size.height);
		SPD_Text1().Text(image_information);
		auto dpi{ image.DPI() };
		wsprintfW(image_information, L"分辨率: %u × %u dpi", dpi.width, dpi.height);
		SPD_Text2().Text(image_information);
		wsprintfW(image_information, L"位深度: %u", image.BitDepth());
		SPD_Text3().Text(image_information);
		wsprintfW(image_information, L"大小: %u KB", static_cast<mqui32>(photo_path.Size()) / 1024U);
		SPD_Text4().Text(image_information);
		SPD_Image().Source(util::StreamToBMP(image.SaveToUnsafeStream(Media::GDI::ImageFormat::BMP)));
		co_await dialog.ShowAsync();
	}

	// 右键打开菜单
	F_EVENT void HistoryPage::ModelPhoto_RightClick(IInspectable const&, Input::RightTappedRoutedEventArgs const& args) {
		auto photo_view{ GV_Photos() };
		auto image{ args.OriginalSource().as<Controls::Image>() };
		auto tag{ image.Tag().as<hstring>() };
		for (auto record : photo_view.ItemsSource().as<Windows::Foundation::Collections::IVector<hyzjkz::ModelPhoto>>()) {
			if (record.PhotoPath() == tag) {
				photo_view.SelectedItem(record);
				GV_Photos_Menu().ShowAt(image, args.GetPosition(image));
				break;
			}
		}
	}

	// 右键菜单
	F_EVENT IAsyncAction HistoryPage::MenuItem_Click(IInspectable const&, RoutedEventArgs const& args) {
		std::wstring tag{ args.OriginalSource().as<FrameworkElement>().Tag().as<hstring>() };
		auto dom{ std::wstring_view{ tag }.substr(0ULL, 4ULL) };
		auto name{ std::wstring_view{ tag }.substr(5ULL) };

		auto item{ GV_Photos().SelectedItem().as<hyzjkz::ModelPhoto>() };

		if (dom == L"Main") {
			if (name == L"Save") co_await Menu_Save(item);
			else if (name == L"Copy") co_await Menu_Copy(this, item);
			else if (name == L"Delete") co_await Menu_Delete(this, item);
			else if (name == L"Print") Menu_Print(item);
		}
		else if (dom == L"Auto") {
			if (name.starts_with(L"Rotate")) co_await Menu_Rotate(this, item, name);
			else if (name == L"DPI") co_await Menu_DPI(this, item);
			else if (name == L"Border") co_await Menu_Border(this, item);
			else if (name == L"BWPhoto") co_await Menu_BWPhoto(this, item);
			else if (name == L"IDCard") co_await Menu_IDCard(this, item);
		}
		else if (dom == L"Link") {
			auto tools{ Global.cfg.Get(GlobalConfig::TOOLS) };
			if (tools.contains(name)) {
				co_await Menu_Link(this, item, tools.get(name));
			}
		}
	}
}

namespace winrt::hyzjkz::implementation {
	// 照片数值位图显示
	F_RT Microsoft::UI::Xaml::Visibility HistoryPage::GetPVBVisibility(mqui32 photo_num, mqui32 index, bool is_photo_num) const {
		if (is_photo_num) { // 照片数
			if (index == 1U) { // 若小于两位数则第一位不可见
				return photo_num < 10U ? Visibility::Collapsed : Visibility::Visible;
			}
			else if (index == 2U) { // 第二位始终可见
				return Visibility::Visible;
			}
			else { // 若小于三位数则第三位不可见
				return photo_num < 100U ? Visibility::Collapsed : Visibility::Visible;
			}
		}
		else {
			auto turnover{ photo_num * Global.cfg.Get<GlobalConfig::UNIT_PRICE>() }; // 营业额
			if (index == 1U) { // 若小于三位数则第一位不可见
				return turnover < 100U ? Visibility::Collapsed : Visibility::Visible;
			}
			else if (index == 2U) { // 第二位始终可见
				return Visibility::Visible;
			}
			else if (index == 3U) { // 若小于两位数则第三位不可见
				return turnover < 10U ? Visibility::Collapsed : Visibility::Visible;
			}
			else { // 若小于四位数则第四位不可见
				return turnover < 1000U ? Visibility::Collapsed : Visibility::Visible;
			}
		}
	}

	F_RT Microsoft::UI::Xaml::Media::Imaging::BitmapImage HistoryPage::GetPVBSource(mqui32 photo_num, mqui32 index, bool is_photo_num) const {
		if (is_photo_num) { // 照片数
			if (index == 1U) {
				if (photo_num >= 10U) {
					return photo_num < 100U ? bmp_nums[photo_num / 10U] : bmp_nums[photo_num / 100U];
				}
			}
			else if (index == 2U) {
				if (photo_num < 10U) {
					return bmp_nums[photo_num];
				}
				else {
					return photo_num < 100U ? bmp_nums[photo_num % 10U] : bmp_nums[photo_num / 10U % 10U];
				}
			}
			else {
				if (photo_num >= 100U) {
					return bmp_nums[photo_num % 10U];
				}
			}
		}
		else {
			auto turnover{ photo_num * Global.cfg.Get<GlobalConfig::UNIT_PRICE>() }; // 营业额
			if (index == 1U) {
				if (turnover >= 100U) {
					return turnover < 1000U ? bmp_nums[turnover / 100U] : bmp_nums[turnover / 1000U];
				}
			}
			else if (index == 2U) {
				if (turnover < 100U) {
					return turnover < 10U ? bmp_nums[turnover] : bmp_nums[turnover / 10U];
				}
				else {
					return turnover < 1000U ? bmp_nums[turnover / 10U % 10U] : bmp_nums[turnover / 100U % 10U];
				}
			}
			else if (index == 3U) {
				if (turnover >= 10U) {
					if (turnover < 100U) {
						return bmp_nums[turnover % 10U];
					}
					else {
						return turnover < 1000U ? bmp_nums[turnover % 10U] : bmp_nums[turnover / 10U % 10U];
					}
				}
			}
			else {
				if (turnover >= 1000U) {
					return bmp_nums[turnover % 10U];
				}
			}
		}
		return nullptr;
	}
}