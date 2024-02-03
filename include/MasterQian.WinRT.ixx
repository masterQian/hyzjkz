module;
#include "MasterQian.Meta.h"
#include <winrt/windows.foundation.h>
#define MasterQianModuleVersion 20240131ULL

export module MasterQian.WinRT;
export import MasterQian.freestanding;

/*  --------------------  参数包  --------------------  */
/// <summary>
/// <para>Args - 参数包</para>
/// <para>将多个不同winrt运行时类型打包成一个方便参数传递, 且使用结构化绑定更方便解包</para>
/// <example>
/// <code>
/// <para>auto args { Args::box(1, 2.5, L"Something", winrt::Windows::UI::Xaml::Controls::Button{}) };</para>
/// <para>auto [arg1, _, arg3] = args.unbox&lt;int, void, winrt::hstring&gt;(); </para>
/// </code>
/// <para>
/// 在此例中将整数, 浮点数, 字符串, 按钮这多个不同类型自动包装并打包成一个运行时类型, 
/// 可以将此类型传递到诸如事件函数中的参数、控件的Tag中等等, 
/// 通过显示指定unbox的模板类型来解包, 数量并不一定需要完全相同, 只要不超过原参数包的大小,
/// 且可以通过void类型来指定某个不关心的参数忽略掉
/// </para>
/// </example>
/// </summary>

namespace MasterQian::WinRT {
    struct Args;

    namespace details {
        struct ArgsImpl;

        template<typename T>
        concept arg_pointer = std::is_pointer_v<T> || std::same_as<T, std::nullptr_t>;
    }
}

namespace winrt::impl {
    template <>
    constexpr guid guid_v<MasterQian::WinRT::Args>{ 0x2ED4939F, 0xD013, 0x56FD, { 0xAC, 0xB4, 0x87, 0xF2, 0xB1, 0x3A, 0x06, 0xDC } };

    template <>
    struct abi<MasterQian::WinRT::Args> {
        struct __declspec(novtable) type : inspectable_abi {
            virtual mqi32 __stdcall Size(mqui32*) noexcept = 0;
            virtual mqi32 __stdcall Append(Windows::Foundation::IInspectable const&) noexcept = 0;
            virtual mqi32 __stdcall GetAt(mqui32, Windows::Foundation::IInspectable*) noexcept = 0;
            virtual mqi32 __stdcall GetAll(Windows::Foundation::IInspectable**) noexcept = 0;
            virtual mqi32 __stdcall SetAt(mqui32, Windows::Foundation::IInspectable const&) noexcept = 0;
        };
    };

    template <typename D>
    struct produce<D, MasterQian::WinRT::Args> : produce_base<D, MasterQian::WinRT::Args> {
        mqi32 __stdcall Size(mqui32* result) noexcept final try {
            MasterQian::WinRT::details::ArgsImpl& args{ this->shim() };
            *result = static_cast<mqui32>(args.objects.size());
            return 0;
        }
        catch (...) { return to_hresult(); }

        mqi32 __stdcall Append(Windows::Foundation::IInspectable const& obj) noexcept final try {
            MasterQian::WinRT::details::ArgsImpl& args{ this->shim() };
            args.objects.emplace_back(obj);
            return 0;
        }
        catch (...) { return to_hresult(); }

        mqi32 __stdcall GetAt(mqui32 index, Windows::Foundation::IInspectable* result) noexcept final try {
            MasterQian::WinRT::details::ArgsImpl& args{ this->shim() };
            typename D::abi_guard guard(args);
            *result = args.objects[index];
            return 0;
        }
        catch (...) { return to_hresult(); }

        mqi32 __stdcall GetAll(Windows::Foundation::IInspectable** result) noexcept final try {
            MasterQian::WinRT::details::ArgsImpl& args{ this->shim() };
            typename D::abi_guard guard(args);
            *result = args.objects.data();
            return 0;
        }
        catch (...) { return to_hresult(); }

        mqi32 __stdcall SetAt(mqui32 index, Windows::Foundation::IInspectable const& obj) noexcept final try {
            MasterQian::WinRT::details::ArgsImpl& args{ this->shim() };
            typename D::abi_guard guard(args);
            args.objects[index] = obj;
            return 0;
        }
        catch (...) { return to_hresult(); }
    };
}

namespace MasterQian::WinRT {
    // 参数包
    export struct Args : winrt::Windows::Foundation::IInspectable {
    private:
        using this_abi = winrt::impl::abi_t<Args>;

        static constexpr struct _placeholder_t {} _placeholder;
        template<typename... T>
        using replace_placeholder_tuple = std::tuple<std::conditional_t<std::same_as<T, void>, _placeholder_t, T>...>;

        template<typename U>
        static winrt::Windows::Foundation::IInspectable box_arg(U&& u) {
            if constexpr (details::arg_pointer<U>) {
                return winrt::box_value(reinterpret_cast<mqui64>(u));
            }
            else {
                return winrt::box_value(std::forward<U>(u));
            }
        }

        template<typename U>
        static U unbox_arg(winrt::Windows::Foundation::IInspectable const& value) {
            if constexpr (std::same_as<U, _placeholder_t>) {
                return _placeholder;
            }
            else if constexpr (details::arg_pointer<U>) {
                return reinterpret_cast<U>(winrt::unbox_value<mqui64>(value));
            }
            else {
                return winrt::unbox_value<U>(value);
            }
        }

        template<typename D, size_t... Index>
        D unbox_args(std::index_sequence<Index...>) {
            winrt::Windows::Foundation::IInspectable* data{ };
            winrt::check_hresult((*(this_abi**)this)->GetAll(&data));
            return std::make_tuple(Args::unbox_arg<std::tuple_element_t<Index, D>>(data[Index])...);
        }
    public:
        Args(std::nullptr_t) {}
        Args(void* ptr, winrt::take_ownership_from_abi_t) noexcept : winrt::Windows::Foundation::IInspectable{ ptr, winrt::take_ownership_from_abi } {}
        Args();

        [[nodiscard]] mqui32 size() const {
            mqui32 result{ };
            winrt::check_hresult((*(this_abi**)this)->Size(&result));
            return result;
        }

        template<typename U>
        [[nodiscard]] U get(mqui32 index) const {
            winrt::Windows::Foundation::IInspectable result;
            winrt::check_hresult((*(this_abi**)this)->GetAt(index, &result));
            return Args::unbox_arg<U>(result);
        }

        template<typename U>
        void set(mqui32 index, U&& u) {
            winrt::check_hresult((*(this_abi**)this)->SetAt(index, Args::box_arg(std::forward<U>(u))));
        }

        template<typename... T>
        [[nodiscard]] static Args box(T&&... t) {
            Args args;
            auto impl{ *(this_abi**)&args };
            (winrt::check_hresult(impl->Append(Args::box_arg(std::forward<T>(t)))), ...);
            return args;
        }
        
        template<typename... T>
        [[nodiscard]] replace_placeholder_tuple<T...> unbox() {
            return unbox_args<replace_placeholder_tuple<T...>>(std::make_index_sequence<sizeof...(T)>{});
        }
    };

    namespace details {
        struct ArgsImpl : winrt::implements<ArgsImpl, Args> {
            std::vector<winrt::Windows::Foundation::IInspectable> objects;

            winrt::hstring GetRuntimeClassName() const { return L"MasterQian.WinRT.Args"; }
        };
    }

    inline Args::Args() : Args{ winrt::make<details::ArgsImpl>() } {}
}


/*  --------------------  子页面  --------------------*/

namespace MasterQian::WinRT {
    // 子页面
    export template<std::derived_from<winrt::Windows::Foundation::IInspectable> T, typename U>
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
    export using SubPageFunc = void(*)(winrt::Windows::Foundation::IInspectable*) noexcept;
}