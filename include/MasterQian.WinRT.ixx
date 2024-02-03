module;
#include "MasterQian.Meta.h"
#include <winrt/windows.foundation.h>
#define MasterQianModuleVersion 20240131ULL

export module MasterQian.WinRT;
export import MasterQian.freestanding;

/*  --------------------  ������  --------------------  */
/// <summary>
/// <para>Args - ������</para>
/// <para>�������ͬwinrt����ʱ���ʹ����һ�������������, ��ʹ�ýṹ���󶨸�������</para>
/// <example>
/// <code>
/// <para>auto args { Args::box(1, 2.5, L"Something", winrt::Windows::UI::Xaml::Controls::Button{}) };</para>
/// <para>auto [arg1, _, arg3] = args.unbox&lt;int, void, winrt::hstring&gt;(); </para>
/// </code>
/// <para>
/// �ڴ����н�����, ������, �ַ���, ��ť������ͬ�����Զ���װ�������һ������ʱ����, 
/// ���Խ������ʹ��ݵ������¼������еĲ������ؼ���Tag�еȵ�, 
/// ͨ����ʾָ��unbox��ģ�����������, ��������һ����Ҫ��ȫ��ͬ, ֻҪ������ԭ�������Ĵ�С,
/// �ҿ���ͨ��void������ָ��ĳ�������ĵĲ������Ե�
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
    // ������
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


/*  --------------------  ��ҳ��  --------------------*/

namespace MasterQian::WinRT {
    // ��ҳ��
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