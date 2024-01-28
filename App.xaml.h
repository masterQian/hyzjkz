#pragma once

#include "App.xaml.g.h"

using namespace winrt::Microsoft::UI::Xaml;

namespace winrt::hyzjkz::implementation {
    struct App : AppT<App> {
        Window window{ nullptr };

        App();

        void OnLaunched(LaunchActivatedEventArgs const&);
    };
}
