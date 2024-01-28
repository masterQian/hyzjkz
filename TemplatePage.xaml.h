#pragma once

#include "TemplatePage.g.h"

#include "app.h"

using namespace winrt::Microsoft::UI::Xaml;

namespace winrt::hyzjkz::implementation {
    struct TemplatePage : TemplatePageT<TemplatePage> {
        TemplatePage();
        F_EVENT void LV_Templates_SelectionChanged(IInspectable const&, Controls::SelectionChangedEventArgs const&);
        F_EVENT Windows::Foundation::IAsyncAction AppBarButton_Click(IInspectable const&, RoutedEventArgs const&);

        F_RT hstring CanvasSizeString() const;
    };
}

namespace winrt::hyzjkz::factory_implementation {
    struct TemplatePage : TemplatePageT<TemplatePage, implementation::TemplatePage> {};
}
