#pragma once

#include "RecordPage.g.h"

#include "app.h"

namespace winrt::hyzjkz::implementation {
    struct RecordPage : RecordPageT<RecordPage> {
        hstring curYear{ };
        mqui32 curMonth{ };

        RecordPage();
        F_EVENT void Year_Clicked(IInspectable const&, RoutedEventArgs const&);
        F_EVENT void Month_Clicked(IInspectable const&, RoutedEventArgs const&);
        F_EVENT void Day_Clicked(IInspectable const&, RoutedEventArgs const&);
    };
}

namespace winrt::hyzjkz::factory_implementation {
    struct RecordPage : RecordPageT<RecordPage, implementation::RecordPage> {};
}
