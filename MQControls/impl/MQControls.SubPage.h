#pragma once
#include <winrt/windows.foundation.h>

/*  --------------------  子页面  --------------------*/

namespace winrt::MQControls {
    // 子页面
    template<std::derived_from<winrt::Windows::Foundation::IInspectable> T, typename U>
    struct SubPage : T {
        SubPage() = default;
        static U* From(winrt::Windows::Foundation::IInspectable* subPage) noexcept {
            return reinterpret_cast<U*>(subPage);
        }

        template<typename D>
        void Bind(D& d, winrt::hstring const& name) const noexcept {
            d = reinterpret_cast<U const*>(this)->FindName(name).as<D>();
        }
    };
    using SubPageFunc = void(*)(winrt::Windows::Foundation::IInspectable*) noexcept;
}