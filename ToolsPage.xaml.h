#pragma once

#include "ToolsPage.g.h"

#include "app.h"

using namespace winrt::Microsoft::UI::Xaml;

namespace winrt::hyzjkz::implementation {
    struct ToolsPage : ToolsPageT<ToolsPage> {
        using HomeSubPage = SubPage<Controls::Image>;
        HomeSubPage Home;

        struct IDCardSubPage : SubPage<Controls::Grid> {
            Controls::Button button_front;
            Controls::Button button_back;
            Controls::Image image_preview;
            Controls::TextBlock preview_text;
            Controls::Button button_clear;
            Controls::Button button_work;
        }IDCard;

        ToolsPage();
        F_EVENT void NV_Main_SelectionChanged(Controls::NavigationView const&, Controls::NavigationViewSelectionChangedEventArgs const&);
    };
}

namespace winrt::hyzjkz::factory_implementation {
    struct ToolsPage : ToolsPageT<ToolsPage, implementation::ToolsPage> {};
}
