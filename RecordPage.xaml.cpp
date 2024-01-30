#include "pch.h"
#include "RecordPage.xaml.h"
#if __has_include("RecordPage.g.cpp")
#include "RecordPage.g.cpp"
#endif

#include <queue>

struct RecordYearStatistics { // ��ͳ��
	mqui32 photo_year_num; // ����������
	mqui32 photo_year_value; // ��������ֵ
	mqui32 photo_month_nums[12]; // ����������
	mqui32 photo_month_values[12]; // ��������ֵ
	mqui32 copy_year_value; // ��ӡ����ֵ
	mqui32 copy_month_values[12]; // ��ӡ����ֵ
	mqui32 max_month_value; // �������ֵ
	mqui32 avg_month_value; // �¾�ֵ
};

struct RecordMonthStatistics { // ��ͳ��
	mqui32 photo_month_num; // ����������
	mqui32 photo_month_value; // ��������ֵ
	mqui32 photo_day_nums[31]; // ����������
	mqui32 photo_day_values[31]; // ��������ֵ
	mqui32 copy_month_value; // ��ӡ����ֵ
	mqui32 copy_day_values[31]; // ��ӡ����ֵ
	mqui32 max_day_value; // �����ֵ
	mqui32 avg_day_value; // �վ�ֵ
};

namespace winrt::hyzjkz::implementation {
	static void UpdateDay(RecordPage* page) noexcept;
	static void UpdateMonth(RecordPage* page) noexcept;

	// �����걨��
	static void UpdateYear(RecordPage* page) noexcept {
		auto drop_button{ page->SelectYear() };
		auto items{ page->SelectYearFlyout().Items() };
		items.Clear();

		std::priority_queue<std::wstring> years;
		for (auto& file : Global.c_dataPath.EnumFolder(L"*.bin")) {
			auto name{ file.NameWithoutExt() };
			if (name.size() == 4U) {
				years.emplace(name);
			}
		}

		if (years.empty()) {
			page->curYear = winrt::to_hstring(MasterQian::Timestamp{ }.local().year);
			drop_button.Content(box_value(util::RDString<hstring>(L"RecordPage.NoData")));
		}
		else {
			page->curYear = years.top();
			drop_button.Content(box_value(page->curYear));
			while (!years.empty()) {
				Controls::MenuFlyoutItem item;
				item.Text(years.top());
				item.Click({ page, &RecordPage::Year_Clicked });
				items.Append(item);
				years.pop();
			}
			UpdateMonth(page);
		}
	}

	// �����±���
	static void UpdateMonth(RecordPage* page) noexcept {
		auto bin{ Global.c_dataPath.Concat(page->curYear + L".bin").Read() };
		if (RecordData::CheckSize(bin)) {
			// ��������
			RecordYearStatistics stat{ };
			auto& data{ RecordData::From(bin) };
			for (mqui32 month{ }; month < 12U; ++month) {
				for (mqui32 day{ }; day < 31U; ++day) {
					stat.photo_month_nums[month] += data.photoData[month][day].num;
					stat.photo_month_values[month] += data.photoData[month][day].value;
					stat.copy_month_values[month] += data.copyData[month][day];
				}
				stat.photo_year_num += stat.photo_month_nums[month];
				stat.photo_year_value += stat.photo_month_values[month];
				stat.copy_year_value += stat.copy_month_values[month];
				if (stat.photo_month_values[month] + stat.copy_month_values[month] > stat.max_month_value) {
					stat.max_month_value = stat.photo_month_values[month] + stat.copy_month_values[month];
				}
			}
			stat.avg_month_value = (stat.photo_year_value + stat.copy_year_value) / 12U;

			// ����UI
			auto grid_year{ page->Grid_Year() };
			auto grid_month{ page->Grid_Month() };
			auto grid_main{ grid_year.Children() };
			grid_main.Clear();
			mqchar buf[256]{ };
			for (mqui32 i{ }; i < 12U; ++i) {
				Controls::Button button;
				Application::LoadComponent(button, Windows::Foundation::Uri{ L"ms-appx:///Xaml/RecordMonthItem.xaml" });
				auto panel{ button.FindName(L"Panel").as<Controls::StackPanel>() };
				auto text1{ panel.FindName(L"TB1").as<Controls::TextBlock>() };
				auto text2{ panel.FindName(L"TB2").as<Controls::TextBlock>() };
				auto text3{ panel.FindName(L"TB3").as<Controls::TextBlock>() };
				auto text4{ panel.FindName(L"TB4").as<Controls::TextBlock>() };
				wsprintfW(buf, L"%u��", i + 1U);
				text1.Text(buf);
				wsprintfW(buf, L"���� %u ��  %u Ԫ", stat.photo_month_nums[i], stat.photo_month_values[i]);
				text2.Text(buf);
				wsprintfW(buf, L"��ӡ    %u Ԫ", stat.copy_month_values[i]);
				text3.Text(buf);
				auto total_month_value{ stat.photo_month_values[i] + stat.copy_month_values[i] };
				wsprintfW(buf, L"�ܼ�    %u Ԫ", total_month_value);
				text4.Text(buf);
				if (total_month_value == stat.max_month_value) {
					panel.Background(page->MaxBrush());
				}
				else if (total_month_value < stat.avg_month_value) {
					panel.Background(page->LowBrush());
				}
				else {
					panel.Background(page->NormalBrush());
				}
				button.Tag(box_value(i + 1U));
				button.Click({ page, &RecordPage::Month_Clicked });
				Controls::Grid::SetRow(button, i / 4U);
				Controls::Grid::SetColumn(button, i % 4U);
				grid_main.Append(button);
			}
			wsprintfW(buf, L"%s��    [���� %u ��  %uԪ] [��ӡ %u Ԫ] [�ܼ� %u Ԫ]",
				page->curYear.data(), stat.photo_year_num, stat.photo_year_value, stat.copy_year_value,
				stat.photo_year_value + stat.copy_year_value);
			page->TotalText().Text(buf);

			grid_year.Visibility(Visibility::Visible);
			grid_month.Visibility(Visibility::Collapsed);
		}
	}

	// �����ձ���
	static void UpdateDay(RecordPage* page) noexcept {
		auto bin{ Global.c_dataPath.Concat(page->curYear + L".bin").Read() };
		if (RecordData::CheckSize(bin)) {
			// ��������
			RecordMonthStatistics stat{ };
			auto& data{ RecordData::From(bin) };
			auto& photo_data{ data.photoData[page->curMonth - 1U] };
			auto& copy_data{ data.copyData[page->curMonth - 1U] };
			for (mqui32 day{ }; day < 31U; ++day) {
				stat.photo_day_nums[day] = photo_data[day].num;
				stat.photo_day_values[day] = photo_data[day].value;
				stat.copy_day_values[day] = copy_data[day];
				stat.photo_month_num += stat.photo_day_nums[day];
				stat.photo_month_value += stat.photo_day_values[day];
				stat.copy_month_value += stat.copy_day_values[day];
				if (stat.photo_day_values[day] + stat.copy_day_values[day] > stat.max_day_value) {
					stat.max_day_value = stat.photo_day_values[day] + stat.copy_day_values[day];
				}
			}
			stat.avg_day_value = (stat.photo_month_value + stat.copy_month_value) / 31U;

			// ����UI
			auto grid_year{ page->Grid_Year() }, grid_month{ page->Grid_Month() };
			auto grid_main{ grid_month.Children() };
			grid_main.Clear();
			mqchar buf[256]{ };
			for (mqui32 i{ }; i < 31U; ++i) {
				Controls::Button button;
				Application::LoadComponent(button, Windows::Foundation::Uri{ L"ms-appx:///Xaml/RecordDayItem.xaml" });
				auto panel{ button.FindName(L"Panel").as<Controls::StackPanel>() };
				auto text1{ panel.FindName(L"TB1").as<Controls::TextBlock>() };
				auto text2{ panel.FindName(L"TB2").as<Controls::TextBlock>() };
				auto text3{ panel.FindName(L"TB3").as<Controls::TextBlock>() };
				auto text4{ panel.FindName(L"TB4").as<Controls::TextBlock>() };
				wsprintfW(buf, L"%u��", i + 1U);
				text1.Text(buf);
				wsprintfW(buf, L"���� %u ��  %u Ԫ", stat.photo_day_nums[i], stat.photo_day_values[i]);
				text2.Text(buf);
				wsprintfW(buf, L"��ӡ    %u Ԫ", stat.copy_day_values[i]);
				text3.Text(buf);
				auto total_day_value{ stat.photo_day_values[i] + stat.photo_day_values[i] };
				wsprintfW(buf, L"�ܼ�    %u Ԫ", total_day_value);
				text4.Text(buf);
				if (total_day_value == stat.max_day_value) {
					panel.Background(page->MaxBrush());
				}
				else if (total_day_value < stat.max_day_value) {
					panel.Background(page->LowBrush());
				}
				else {
					panel.Background(page->NormalBrush());
				}
				button.Click({ page, &RecordPage::Day_Clicked });
				Controls::Grid::SetRow(button, i / 7U);
				Controls::Grid::SetColumn(button, i % 7U);
				grid_main.Append(button);
			}
			wsprintfW(buf, L"%s��%u��    [���� %u ��  %uԪ] [��ӡ %u Ԫ] [�ܼ� %u Ԫ]",
				page->curYear.data(), page->curMonth, stat.photo_month_num, stat.photo_month_value,
				stat.copy_month_value, stat.photo_month_value + stat.copy_month_value);
			page->TotalText().Text(buf);

			grid_year.Visibility(Visibility::Collapsed);
			grid_month.Visibility(Visibility::Visible);
		}
	}
}

namespace winrt::hyzjkz::implementation {
	RecordPage::RecordPage() {
		InitializeComponent();

		Loaded([this] (auto, auto) -> Windows::Foundation::IAsyncAction {
			if (Global.cfg.Get<GlobalConfig::USE_PASSWORD>()) {
				auto sp_main{ SP_Main() };
				sp_main.Visibility(Visibility::Collapsed);
				co_await Global.ui_window->as<hyzjkz::MainWindow>().ShowPasswordDialog();
				sp_main.Visibility(Visibility::Visible);
			}
			UpdateYear(this);
			});
	}

	// �����
	F_EVENT void RecordPage::Year_Clicked(IInspectable const& sender, RoutedEventArgs const&) {
		curYear = sender.as<Controls::MenuFlyoutItem>().Text();
		SelectYear().Content(box_value(curYear));
		UpdateMonth(this);
	}

	// �����
	F_EVENT void RecordPage::Month_Clicked(IInspectable const& sender, RoutedEventArgs const&) {
		curMonth = sender.as<FrameworkElement>().Tag().as<mqui32>();
		UpdateDay(this);
	}

	// �����
	F_EVENT void RecordPage::Day_Clicked(IInspectable const&, RoutedEventArgs const&) {
		UpdateMonth(this);
	}
}