#pragma once

#include "App.xaml.g.h"

namespace winrt::hyzjkz::implementation {
    struct App : AppT<App> {
        Microsoft::UI::Xaml::Window window{ nullptr };

        App();
        void OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const&);
    };
}
