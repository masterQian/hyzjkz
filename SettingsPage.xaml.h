#pragma once

#include "SettingsPage.g.h"

#include "app.h"

using namespace winrt::Microsoft::UI::Xaml;

namespace winrt::hyzjkz::implementation {
    struct SettingsPage : SettingsPageT<SettingsPage> {
        struct AboutSubPage : SubPage<Controls::StackPanel> {
            Controls::Image image_icon;
        }About;

        struct PathSubPage : SubPage<Controls::StackPanel> {
            Controls::InfoBar infobar_eospath;
            Controls::TextBox input_eospath;
            Controls::Button button_eospath_set;
            Controls::Button button_eospath_clear;
            Controls::ListView list_tools;
            Controls::Button button_tools_add;
        }Path;

        struct BusinessSubPage : SubPage<Controls::StackPanel> {
            Controls::NumberBox input_unitprice;
            Controls::Button button_unitprice;
            Controls::NumberBox input_resetmonth;
            Controls::Button button_resetmonth;
            Controls::ToggleSwitch switch_showturnover;
            Controls::ToggleSwitch switch_password;
            Controls::TextBox input_password;
            Controls::Button button_password;
        }Business;

        struct PrintSubPage : SubPage<Controls::StackPanel> {
            Controls::InfoBar infobar_printername;
            Controls::TextBox input_printername;
            Controls::Button button_printername;
            Controls::NumberBox input_autocuteps;
            Controls::Button button_autocuteps;
        }Print;

        SettingsPage();
        F_EVENT void NV_Main_SelectionChanged(Controls::NavigationView const&, Controls::NavigationViewSelectionChangedEventArgs const&);
    };
}

namespace winrt::hyzjkz::factory_implementation {
    struct SettingsPage : SettingsPageT<SettingsPage, implementation::SettingsPage> {};
}
