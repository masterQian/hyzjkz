#pragma once

#include "PrintPage.g.h"

#include "app.h"

namespace winrt::hyzjkz::implementation {
    struct PrintPage : PrintPageT<PrintPage> {
        hstring photo_path;

        PrintPage();
        F_EVENT void OnNavigatedTo(Microsoft::UI::Xaml::Navigation::NavigationEventArgs const&);
        F_EVENT void Canvas_SizeChanged(IInspectable const&, Microsoft::UI::Xaml::SizeChangedEventArgs const&);
        F_EVENT void LV_Templates_SelectionChanged(IInspectable const&, Controls::SelectionChangedEventArgs const&);
        F_EVENT IAsyncAction AppBarButton_Click(IInspectable const&, RoutedEventArgs const&);
    };
}

namespace winrt::hyzjkz::factory_implementation {
    struct PrintPage : PrintPageT<PrintPage, implementation::PrintPage> {};
}
