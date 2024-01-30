#pragma once

#include "ToolsPage.g.h"

#include "app.h"

using namespace winrt::Microsoft::UI::Xaml;

namespace winrt::hyzjkz::implementation {
    struct ToolsPage : ToolsPageT<ToolsPage> {
        struct HomeSubPage : MasterQian::WinRT::SubPage<Controls::Image, HomeSubPage> {

        }Home;

        struct IDCardSubPage : MasterQian::WinRT::SubPage<Controls::Grid, IDCardSubPage> {
            Controls::Button button_front{ nullptr };
            Controls::Button button_back{ nullptr };
            Controls::Image image_preview{ nullptr };
            Controls::TextBlock preview_text{ nullptr };
            Controls::Button button_clear{ nullptr };
            Controls::Button button_work{ nullptr };
        }IDCard;

        struct JigsawSubPage : MasterQian::WinRT::SubPage<Controls::Grid, JigsawSubPage> {
            Controls::ToggleSwitch switch_mode{ nullptr };
            Controls::AppBarButton button_import{ nullptr };
            Controls::AppBarButton button_clear{ nullptr };
            Controls::AppBarButton button_save{ nullptr };
            Controls::Canvas canvas{ nullptr };
        }Jigsaw;

        ToolsPage();
        F_EVENT void NV_Main_SelectionChanged(Controls::NavigationView const&, Controls::NavigationViewSelectionChangedEventArgs const&);
    };
}

namespace winrt::hyzjkz::factory_implementation {
    struct ToolsPage : ToolsPageT<ToolsPage, implementation::ToolsPage> {};
}
