#pragma once

#include "MainWindow.g.h"
#include "app.h"

namespace winrt::hyzjkz::implementation {
    struct MainWindow : MainWindowT<MainWindow> {
        UpdateFlag mUpdateFlag;

        MainWindow();

        F_EVENT void NV_Main_ItemInvoked(Controls::NavigationView const&, Controls::NavigationViewItemInvokedEventArgs const&);
        F_EVENT IAsyncAction AttachCamera_Click(IInspectable const&, RoutedEventArgs const&);
        F_EVENT void CheckUpdate_Click(IInspectable const&, RoutedEventArgs const&);

        F_RT IAsyncAction ShowTipDialog(hstring const&);
        F_RT IAsyncOperation<bool> ShowConfirmDialog(hstring const&);
        F_RT IAsyncOperation<hstring> ShowInputDialog(hstring const&);
        F_RT IAsyncAction ShowPasswordDialog();

        F_RT void NavigateToPublicPage(mqui64);
        F_RT void NavigateToPrivatePage(mqui64, IInspectable const&);
        F_RT void UpdateFlags(mqui64, mqui64);
    };
}

namespace winrt::hyzjkz::factory_implementation {
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow> {};
}
