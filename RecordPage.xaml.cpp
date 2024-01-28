#include "pch.h"
#include "RecordPage.xaml.h"
#if __has_include("RecordPage.g.cpp")
#include "RecordPage.g.cpp"
#endif

#include <queue>

struct RecordYearStatistics { // 年统计
	mqui32 yearNum; // 年数量
	mqui32 yearValue; // 年总值
	mqui32 monthNums[12]; // 月数量
	mqui32 monthValues[12]; // 月总值
	mqui32 maxMonthValue; // 月最大总值
	mqui32 avgMonthValue; // 月均值
};

struct RecordMonthStatistics { // 月统计
	mqui32 monthNum; // 月数量
	mqui32 monthValue; // 月总值
	mqui32 dayNums[31]; // 日数量
	mqui32 dayValues[31]; // 日总值
	mqui32 maxDayValue; // 日最大值
	mqui32 avgDayValue; // 日均值
};

namespace winrt::hyzjkz::implementation {
	static void UpdateDay(RecordPage* page) noexcept;
	static void UpdateMonth(RecordPage* page) noexcept;

	// 更新年报表
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

	// 更新月报表
	static void UpdateMonth(RecordPage* page) noexcept {
		auto bin{ Global.c_dataPath.Concat(page->curYear + L".bin").Read() };
		if (bin.size() == sizeof(RecordYearData)) {
			// 更新数据
			RecordYearStatistics stat{ };
			auto& data{ *reinterpret_cast<RecordYearData const*>(bin.data()) };
			for (mqui32 month{ }; month < 12U; ++month) {
				for (mqui32 day{ }; day < 31U; ++day) {
					stat.monthNums[month] += data[month][day].num;
					stat.monthValues[month] += data[month][day].value;
				}
				stat.yearNum += stat.monthNums[month];
				stat.yearValue += stat.monthValues[month];
				if (stat.monthValues[month] > stat.maxMonthValue) {
					stat.maxMonthValue = stat.monthValues[month];
				}
			}
			stat.avgMonthValue = stat.yearValue / 12U;

			// 更新UI
			auto grid_year{ page->Grid_Year() };
			auto grid_month{ page->Grid_Month() };
			auto grid_main{ grid_year.Children() };
			grid_main.Clear();
			for (mqui32 i{ }; i < 12U; ++i) {
				Controls::Button button;
				Application::LoadComponent(button, Windows::Foundation::Uri{ L"ms-appx:///Xaml/RecordMonthItem.xaml" });
				auto panel{ button.FindName(L"Panel").as<Controls::StackPanel>() };
				auto text1{ panel.FindName(L"TB1").as<Controls::TextBlock>() };
				auto text2{ panel.FindName(L"TB2").as<Controls::TextBlock>() };
				text1.Text(to_hstring(i + 1U) + L"月");
				text2.Text(to_hstring(stat.monthNums[i]) + L"    " + to_hstring(stat.monthValues[i]));
				if (stat.monthValues[i] == stat.maxMonthValue) {
					panel.Background(page->MaxBrush());
				}
				else if (stat.monthValues[i] < stat.avgMonthValue) {
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

			mqchar totalText[64]{ };
			wsprintfW(totalText, L"%s年          %u    %u", page->curYear.data(), stat.yearNum, stat.yearValue);
			page->TotalText().Text(totalText);

			grid_year.Visibility(Visibility::Visible);
			grid_month.Visibility(Visibility::Collapsed);
		}
	}

	// 更新日报表
	static void UpdateDay(RecordPage* page) noexcept {
		auto bin{ Global.c_dataPath.Concat(page->curYear + L".bin").Read() };
		if (bin.size() == sizeof(RecordYearData)) {
			// 更新数据
			RecordMonthStatistics stat{ };
			auto& data{ (*reinterpret_cast<RecordYearData const*>(bin.data()))[page->curMonth - 1U] };
			for (mqui32 day{ }; day < 31U; ++day) {
				stat.dayNums[day] = data[day].num;
				stat.dayValues[day] = data[day].value;
				stat.monthNum += data[day].num;
				stat.monthValue += data[day].value;
				if (data[day].value > stat.maxDayValue) {
					stat.maxDayValue = data[day].value;
				}
			}
			stat.avgDayValue = stat.monthValue / 31U;

			// 更新UI
			auto grid_year{ page->Grid_Year() }, grid_month{ page->Grid_Month() };
			auto grid_main{ grid_month.Children() };
			grid_main.Clear();
			for (mqui32 i{ }; i < 31U; ++i) {
				Controls::Button button;
				Application::LoadComponent(button, Windows::Foundation::Uri{ L"ms-appx:///Xaml/RecordDayItem.xaml" });
				auto panel{ button.FindName(L"Panel").as<Controls::StackPanel>() };
				auto text1{ panel.FindName(L"TB1").as<Controls::TextBlock>() };
				auto text2{ panel.FindName(L"TB2").as<Controls::TextBlock>() };
				text1.Text(to_hstring(i + 1U) + L"日");
				text2.Text(to_hstring(stat.dayNums[i]) + L"    " + to_hstring(stat.dayValues[i]));
				if (stat.dayValues[i] == stat.maxDayValue) {
					panel.Background(page->MaxBrush());
				}
				else if (stat.dayValues[i] < stat.avgDayValue) {
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

			mqchar totalText[64]{ };
			wsprintfW(totalText, L"%s年%u月        %u    %u", page->curYear.data(), page->curMonth, stat.monthNum, stat.monthValue);
			page->TotalText().Text(totalText);

			grid_year.Visibility(Visibility::Collapsed);
			grid_month.Visibility(Visibility::Visible);
		}
	}
}

namespace winrt::hyzjkz::implementation {
	RecordPage::RecordPage() {
		InitializeComponent();

		Loaded([this] (auto, auto) -> Windows::Foundation::IAsyncAction {
			if (Global.cfg.get<GlobalType::USE_PASSWORD>(GlobalConfig::USE_PASSWORD, GlobalDefault::USE_PASSWORD)) {
				auto sp_main{ SP_Main() };
				sp_main.Visibility(Visibility::Collapsed);
				co_await Global.ui_window->as<hyzjkz::MainWindow>().ShowPasswordDialog();
				sp_main.Visibility(Visibility::Visible);
			}
			UpdateYear(this);
			});
	}

	// 点击年
	F_EVENT void RecordPage::Year_Clicked(IInspectable const& sender, RoutedEventArgs const&) {
		curYear = sender.as<Controls::MenuFlyoutItem>().Text();
		SelectYear().Content(box_value(curYear));
		UpdateMonth(this);
	}

	// 点击月
	F_EVENT void RecordPage::Month_Clicked(IInspectable const& sender, RoutedEventArgs const&) {
		curMonth = sender.as<FrameworkElement>().Tag().as<mqui32>();
		UpdateDay(this);
	}

	// 点击日
	F_EVENT void RecordPage::Day_Clicked(IInspectable const&, RoutedEventArgs const&) {
		UpdateMonth(this);
	}
}