#include "pch.h"
#include "TemplatePage.xaml.h"
#if __has_include("TemplatePage.g.cpp")
#include "TemplatePage.g.cpp"
#endif

#include <queue>

namespace winrt::hyzjkz::implementation {
	// Ԥ��ģ��
	static void PreviewTemplate(TemplatePage* page) noexcept {
		using namespace MasterQian::Media;

		auto image{ page->CanvasImage() };
		auto canvas_width{ image.ActualWidth() };
		auto canvas_height{ image.ActualHeight() };
		auto canvas_scale{ Global.c_printCanvasSize.height / canvas_height };
		// ����
		GDI::Image canvas({ static_cast<mqui32>(canvas_width), static_cast<mqui32>(canvas_height) }, Colors::Pink);
		// ������
		auto items{ page->LV_TemplateData().Items() };
		std::vector<mqrect> rects{ items.Size() };
		for (auto item_ : items) {
			auto item{ item_.as<hyzjkz::ModelTemplateData>() };
			rects.emplace_back(
				static_cast<mqui32>(item.TLeft() / canvas_scale),
				static_cast<mqui32>(item.TTop() / canvas_scale),
				static_cast<mqui32>(item.TWidth() / canvas_scale),
				static_cast<mqui32>(item.THeight() / canvas_scale));
		}
		canvas.FillRectangles(rects, Colors::Orange, GDI::FAST_MODE);
		canvas.DrawRectangles(rects, Colors::Black, 1.0, GDI::FAST_MODE);
		image.Source(util::StreamToBMP(canvas.SaveToUnsafeStream(GDI::ImageFormat::BMP)));
	}

	// ˢ��ģ��
	static void RefreshTemplate(TemplatePage* page, std::wstring_view template_name) noexcept {
		auto& template_data{ Global.templateList[template_name] };
		page->TB_TemplateName().Text(template_data.name);
		page->Icon_Lock().Visibility(template_data.flag.canDelete ? Visibility::Collapsed : Visibility::Visible);
		page->Icon_UnLock().Visibility(!template_data.flag.canDelete ? Visibility::Collapsed : Visibility::Visible);
		page->TA_Order().Value(static_cast<mqf64>(template_data.order));
		page->TA_Rotate().IsChecked(template_data.flag.isRotate);
		page->TA_AutoCut().IsChecked(template_data.flag.isAutoCut);

		auto items{ page->LV_TemplateData().Items() };
		items.Clear();
		for (mqui32 i{ }; i < template_data.count; ++i) {
			auto& data{ template_data.data[i] };
			items.Append(hyzjkz::ModelTemplateData{ static_cast<mqf64>(data.left),
				static_cast<mqf64>(data.top), static_cast<mqf64>(data.width),
				static_cast<mqf64>(data.height) });
		}

		PreviewTemplate(page);
	}

	// ˢ��ģ���б�
	static void RefreshTemplates(TemplatePage* page) noexcept {
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
	}

	// ѡ���һ��
	static void SelectFirstTemplate(TemplatePage* page) noexcept {
		auto lv_templates{ page->LV_Templates() };
		auto items{ lv_templates.Items() };
		if (items.Size() > 0U) {
			lv_templates.SelectedItem(items.GetAt(0U));
		}
	}

	// ����ģ��
	static Windows::Foundation::IAsyncAction CreateTemplate(TemplatePage* page) noexcept {
		std::wstring name{ co_await Global.ui_window->as<hyzjkz::MainWindow>()
			.ShowInputDialog(util::RDString<hstring>(L"TemplatePage.Tip.InputNewTemplateName")) };
		if (name.empty()) {
			co_return;
		}
		if (Global.templateList.contains(name)) { // ���ģ���Ƿ��Ѵ���
			co_return co_await Global.ui_window->as<hyzjkz::MainWindow>()
				.ShowTipDialog(util::RDString<hstring>(L"TemplatePage.Tip.TemplateNameExists"));
		}
		// ����0���ģ��
		MasterQian::Bin bin(PrintTemplate::CalcSize(0U));
		auto& pt{ *reinterpret_cast<PrintTemplate*>(bin.data()) };
		pt.size = sizeof(PrintTemplate);
		pt.count = 0U;
		pt.flag.canDelete = true;
		pt.flag.isRotate = false;
		pt.flag.isAutoCut = false;
		(void)lstrcpynW(pt.name, name.data(), 31);
		pt.order = 0U;

		// �����ļ�
		Global.c_templatePath.Concat(name + L".template").Write(bin);

		// ���±���
		Global.templateList.add(name, std::move(bin));

		// ����UI
		auto lv_templates{ page->LV_Templates() };
		auto new_item{ box_value(name) };
		lv_templates.Items().Append(new_item);
		lv_templates.SelectedItem(new_item);
	}

	// ɾ��ģ��
	static Windows::Foundation::IAsyncAction DeleteTemplate(TemplatePage* page) noexcept {
		auto item{ page->LV_Templates().SelectedItem() };
		if (item == nullptr) {
			co_return;
		}
		std::wstring name{ item.as<hstring>() };

		// ���ģ������
		auto& pt{ Global.templateList[name] };

		if (!pt.flag.canDelete) { // ���ģ���Ƿ�����ɾ��
			co_return co_await Global.ui_window->as<hyzjkz::MainWindow>()
				.ShowTipDialog(util::RDString<hstring>(L"TemplatePage.Tip.CannotDeleteTemplate"));
		}

		// ����ȷ��
		if (bool result{ co_await Global.ui_window->as<hyzjkz::MainWindow>()
			.ShowConfirmDialog(util::RDString<hstring>(L"TemplatePage.Tip.ConfirmDeleteTemplate") + name) }) {
			// �����ļ�
			Global.c_templatePath.Concat(name + L".template").Delete();

			// ���±���
			Global.templateList.erase(name);

			// ����UI
			auto lv_templates{ page->LV_Templates() };
			auto items{ lv_templates.Items() };
			mqui32 index{ };
			items.IndexOf(box_value(name), index);
			items.RemoveAt(index);
			SelectFirstTemplate(page);
		}
	}

	// ������ģ��
	static Windows::Foundation::IAsyncAction RenameTemplate(TemplatePage* page) noexcept {
		auto item{ page->LV_Templates().SelectedItem() };
		if (item == nullptr) {
			co_return;
		}
		std::wstring old_name{ item.as<hstring>() };
		std::wstring new_name{ co_await Global.ui_window->as<hyzjkz::MainWindow>()
			.ShowInputDialog(util::RDString<hstring>(L"TemplatePage.Tip.InputNewTemplateName")) };
		if (new_name.empty()) {
			co_return;
		}
		if (Global.templateList.contains(new_name)) { // ���ģ���Ƿ��Ѵ���
			co_return co_await Global.ui_window->as<hyzjkz::MainWindow>()
				.ShowTipDialog(util::RDString<hstring>(L"TemplatePage.Tip.TemplateNameExists"));
		}

		// ���ģ������
		MasterQian::Bin bin{ std::move(Global.templateList.get(old_name)) };

		// �޸�����
		auto& pt{ *reinterpret_cast<PrintTemplate*>(bin.data()) };
		(void)lstrcpynW(pt.name, new_name.data(), 31);
		
		// �����ļ�
		Global.c_templatePath.Concat(old_name + L".template").Delete();
		Global.c_templatePath.Concat(new_name + L".template").Write(bin);

		// ���±���
		Global.templateList.erase(old_name);
		Global.templateList.add(new_name, std::move(bin));

		// ����UI
		auto lv_templates{ page->LV_Templates() };
		auto items{ lv_templates.Items() };
		mqui32 index{ };
		items.IndexOf(box_value(old_name), index);
		auto new_item{ box_value(new_name) };
		lv_templates.Items().SetAt(index, new_item);
		lv_templates.SelectedItem(new_item);
	}

	// ����ģ��
	static Windows::Foundation::IAsyncAction SaveTemplate(TemplatePage* page) noexcept {
		auto lv_templates{ page->LV_Templates() };
		auto selected_item{ lv_templates.SelectedItem() };
		if (selected_item == nullptr) {
			co_return;
		}
		std::wstring name{ selected_item.as<hstring>() };

		// ���ԭģ��
		auto data_items{ page->LV_TemplateData().Items() };
		auto new_template_count{ data_items.Size() };
		auto new_template_size{ PrintTemplate::CalcSize(new_template_count) };

		// ������ģ��
		MasterQian::Bin new_template_data(new_template_size);
		auto& pt{ *reinterpret_cast<PrintTemplate*>(new_template_data.data()) };

		// ������������
		memcpy(&pt, Global.templateList.at(name), sizeof(PrintTemplate));
		pt.size = new_template_size;
		pt.count = new_template_count;

		// ����������
		pt.order = static_cast<mqui32>(page->TA_Order().Value());
		pt.flag.isRotate = page->TA_Rotate().IsChecked().as<bool>();
		pt.flag.isAutoCut = page->TA_AutoCut().IsChecked().as<bool>();

		for (mqui32 index{ }; index < new_template_count; ++index) {
			auto item{ data_items.GetAt(index).as<hyzjkz::ModelTemplateData>() };
			pt.data[index] = { static_cast<mqui32>(item.TLeft()), static_cast<mqui32>(item.TTop()),
				static_cast<mqui32>(item.TWidth()), static_cast<mqui32>(item.THeight()) };
		}

		// �����ļ�
		Global.c_templatePath.Concat(name + L".template").Write(new_template_data);

		// ���±���
		Global.templateList.set(name, std::move(new_template_data));

		// ˢ��
		RefreshTemplates(page);
		lv_templates.SelectedItem(box_value(name));

		co_await Global.ui_window->as<hyzjkz::MainWindow>()
			.ShowTipDialog(util::RDString<hstring>(L"TemplatePage.Tip.SaveSuccess"));
	}
}

namespace winrt::hyzjkz::implementation {
	TemplatePage::TemplatePage() {
		InitializeComponent();
		
		Loaded([this] (auto, auto) -> Windows::Foundation::IAsyncAction {
			if (Global.cfg.Get<GlobalConfig::USE_PASSWORD>()) {
				auto grid_main{ Grid_Main() };
				grid_main.Visibility(Visibility::Collapsed);
				co_await Global.ui_window->as<hyzjkz::MainWindow>().ShowPasswordDialog();
				grid_main.Visibility(Visibility::Visible);
			}
			RefreshTemplates(this);
			SelectFirstTemplate(this);
			});
	}

	// �л�ģ��
	F_EVENT void TemplatePage::LV_Templates_SelectionChanged(IInspectable const&, Controls::SelectionChangedEventArgs const& args) {
		if (auto items{ args.AddedItems() }; items.Size() == 1U) {
			RefreshTemplate(this, items.GetAt(0U).as<hstring>());
		}
	}

	// ��ť����
	F_EVENT Windows::Foundation::IAsyncAction TemplatePage::AppBarButton_Click(IInspectable const& sender, RoutedEventArgs const&) {
		auto label{ sender.as<Controls::AppBarButton>().Label() };
		if (label == util::RDString<hstring>(L"TemplatePage.Bar.Add")) {
			co_await CreateTemplate(this);
		}
		else if (label == util::RDString<hstring>(L"TemplatePage.Bar.Delete")) {
			co_await DeleteTemplate(this);
		}
		else if (label == util::RDString<hstring>(L"TemplatePage.Bar.Rename")) {
			co_await RenameTemplate(this);
		}
		else if (label == util::RDString<hstring>(L"TemplatePage.Bar.Preview")) {
			PreviewTemplate(this);
		}
		else if (label == util::RDString<hstring>(L"TemplatePage.Bar.Save")) {
			co_await SaveTemplate(this);
		}
		else if (label == util::RDString<hstring>(L"TemplatePage.Bar.AddItem")) {
			auto lv_template_data{ LV_TemplateData() };
			auto items{ lv_template_data.Items() };
			hyzjkz::ModelTemplateData item{ 0.0, 0.0, 1.0, 1.0 };
			items.Append(item);
			lv_template_data.SelectedItem(item);
		}
		else if (label == util::RDString<hstring>(L"TemplatePage.Bar.DeleteItem")) {
			auto lv_template_data{ LV_TemplateData() };
			if (auto index{ lv_template_data.SelectedIndex() }; index != -1) {
				lv_template_data.Items().RemoveAt(index);
			}
		}
	}
}

namespace winrt::hyzjkz::implementation {
	// ��ȡ�����ߴ�
	F_RT hstring TemplatePage::CanvasSizeString() const {
		mqchar canvas_size_string[32]{ };
		wsprintfW(canvas_size_string, L"������С: %u �� %u", Global.c_printCanvasSize.width, Global.c_printCanvasSize.height);
		return canvas_size_string;
	}
}