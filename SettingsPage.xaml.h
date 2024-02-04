#pragma once

#include "SettingsPage.g.h"

#include "app.h"

namespace winrt::hyzjkz::implementation {
    struct SettingsPage : SettingsPageT<SettingsPage> {
        struct AboutSubPage : WinRT::SubPage<Controls::StackPanel, AboutSubPage> {
            Controls::Image image_icon{ nullptr };
        }About;

        struct PathSubPage : WinRT::SubPage<Controls::StackPanel, PathSubPage> {
            Controls::InfoBar infobar_eospath{ nullptr };
            Controls::TextBox input_eospath{ nullptr };
            Controls::Button button_eospath_set{ nullptr };
            Controls::Button button_eospath_clear{ nullptr };
            Controls::ListView list_tools{ nullptr };
            Controls::Button button_tools_add{ nullptr };
        }Path;

        struct BusinessSubPage : WinRT::SubPage<Controls::StackPanel, BusinessSubPage> {
            Controls::NumberBox input_unitprice{ nullptr };
            Controls::Button button_unitprice{ nullptr };
            Controls::NumberBox input_resetmonth{ nullptr };
            Controls::Button button_resetmonth{ nullptr };
            Controls::ToggleSwitch switch_showturnover{ nullptr };
            Controls::ToggleSwitch switch_password{ nullptr };
            Controls::TextBox input_password{ nullptr };
            Controls::Button button_password{ nullptr };
        }Business;

        struct PrintSubPage : WinRT::SubPage<Controls::StackPanel, PrintSubPage> {
            Controls::InfoBar infobar_printername{ nullptr };
            Controls::TextBox input_printername{ nullptr };
            Controls::Button button_printername{ nullptr };
            Controls::NumberBox input_autocuteps{ nullptr };
            Controls::Button button_autocuteps{ nullptr };
        }Print;

        SettingsPage();
        F_EVENT void NV_Main_SelectionChanged(Controls::NavigationView const&, Controls::NavigationViewSelectionChangedEventArgs const&);
    };
}

namespace winrt::hyzjkz::factory_implementation {
    struct SettingsPage : SettingsPageT<SettingsPage, implementation::SettingsPage> {};
}
