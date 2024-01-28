#pragma once

#include "PrintPage.g.h"

#include "app.h"

using namespace winrt::Microsoft::UI::Xaml;

namespace winrt::hyzjkz::implementation {
    struct PrintPage : PrintPageT<PrintPage> {
        hstring photo_path;

        PrintPage();
        F_EVENT void OnNavigatedTo(Navigation::NavigationEventArgs const&);
        F_EVENT void Canvas_SizeChanged(IInspectable const&, SizeChangedEventArgs const&);
        F_EVENT void LV_Templates_SelectionChanged(IInspectable const&, Controls::SelectionChangedEventArgs const&);
        F_EVENT Windows::Foundation::IAsyncAction AppBarButton_Click(IInspectable const&, RoutedEventArgs const&);
    };
}

namespace winrt::hyzjkz::factory_implementation {
    struct PrintPage : PrintPageT<PrintPage, implementation::PrintPage> {};
}
