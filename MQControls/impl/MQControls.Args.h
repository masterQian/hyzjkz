#pragma once
#include <winrt/windows.foundation.h>

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

namespace winrt {
    namespace MQControls { struct Args; }
    namespace impl {
        template <>
        struct abi<MQControls::Args> { struct __declspec(novtable) type : inspectable_abi { }; };
        template <typename D>
        struct produce<D, MQControls::Args> : produce_base<D, MQControls::Args> { };
        template<typename I>
        struct ArgsImpl : implements<ArgsImpl<I>, I> {
            hstring GetRuntimeClassName() const final { return L"MQControls.Args"; }

            std::vector<Windows::Foundation::IInspectable> objects;
        };
    }

    namespace MQControls {
        // 参数包
        struct __declspec(uuid("2ED4939F-D013-56FD-ACB4-87F2B13A06DC"))
            Args : Windows::Foundation::IInspectable {
        private:
            friend struct Pair;
            using ArgsImpl = impl::ArgsImpl<Args>;

            static constexpr struct _placeholder_t { } _placeholder;

            template<typename U>
            static Windows::Foundation::IInspectable box_arg(U&& u) {
                if constexpr (std::is_pointer_v<U> || std::same_as<U, std::nullptr_t>) {
                    return box_value(reinterpret_cast<uint64_t>(u));
                }
                else {
                    return box_value(std::forward<U>(u));
                }
            }

            template<typename U>
            static U unbox_arg(Windows::Foundation::IInspectable const& value) {
                if constexpr (std::same_as<U, _placeholder_t>) {
                    return _placeholder;
                }
                else if constexpr (std::is_pointer_v<U> || std::same_as<U, std::nullptr_t>) {
                    return reinterpret_cast<U>(unbox_value<uint64_t>(value));
                }
                else {
                    return unbox_value<U>(value);
                }
            }

            template<typename U, size_t... Index>
            U unbox_args(std::index_sequence<Index...>) const {
                auto data{ get_self<ArgsImpl>(*this)->objects.data() };
                return std::make_tuple(Args::unbox_arg<std::tuple_element_t<Index, U>>(data[Index])...);
            }

            Args() : Args{ make<ArgsImpl>() } { }
        public:
            Args(std::nullptr_t) { }
            Args(void* ptr, take_ownership_from_abi_t) noexcept : Windows::Foundation::IInspectable{ ptr, take_ownership_from_abi } {}

            [[nodiscard]] uint64_t size() const {
                return get_self<ArgsImpl>(*this)->objects.size();
            }

            template<typename U>
            [[nodiscard]] U get(uint64_t index) const {
                return Args::unbox_arg<U>(get_self<ArgsImpl>(*this)->objects[index]);
            }

            template<typename... U>
            [[nodiscard]] static Args box(U&&... u) {
                Args args;
                auto& objects{ get_self<ArgsImpl>(args)->objects };
                objects.reserve(sizeof...(U));
                (objects.emplace_back(Args::box_arg(std::forward<U>(u))), ...);
                return args;
            }

            template<typename... U>
            [[nodiscard]] auto unbox() const {
                return unbox_args<std::tuple<std::conditional_t<std::same_as<U, void>, _placeholder_t, U>...>>(std::make_index_sequence<sizeof...(U)>{});
            }
        };
    }
}


/*  --------------------  参数对  --------------------  */
/// <summary>
/// <para>Pair - 参数对</para>
/// <para>仅含2个参数的特定版参数包</para>
/// </summary>

namespace winrt {
    namespace MQControls { struct Pair; }
    namespace impl {
        template <>
        struct abi<MQControls::Pair> { struct __declspec(novtable) type : inspectable_abi { }; };
        template <typename D>
        struct produce<D, MQControls::Pair> : produce_base<D, MQControls::Pair> { };
        template<typename I>
        struct PairImpl : implements<PairImpl<I>, I> {
            hstring GetRuntimeClassName() const final { return L"MQControls.Pair"; }

            std::pair<Windows::Foundation::IInspectable, Windows::Foundation::IInspectable> pairs;
            PairImpl(Windows::Foundation::IInspectable const& v1, Windows::Foundation::IInspectable const& v2) : pairs{ v1, v2 } { }
        };
    }

    namespace MQControls {
        // 参数包
        struct __declspec(uuid("7AD3670B-7784-4B77-A5B1-21735139667B"))
            Pair : Windows::Foundation::IInspectable {
        private:
            using PairImpl = impl::PairImpl<Pair>;
        public:
            Pair(std::nullptr_t) { }
            Pair(void* ptr, take_ownership_from_abi_t) noexcept : Windows::Foundation::IInspectable{ ptr, take_ownership_from_abi } {}
        
            template<typename U1, typename U2>
            [[nodiscard]] static Pair box(U1&& u1, U2&& u2) {
                return Pair{ make<PairImpl>(Args::box_arg(std::forward<U1>(u1)), Args::box_arg(std::forward<U2>(u2))) };
            }

            template<typename U1, typename U2>
            [[nodiscard]] auto unbox() const {
                auto& pairs{ get_self<PairImpl>(*this)->pairs };
                return std::pair{ Args::unbox_arg<U1>(pairs.first), Args::unbox_arg<U2>(pairs.second) };
            }

            template<typename U>
            [[nodiscard]] U first() const {
                return Args::unbox_arg<U>(get_self<PairImpl>(*this)->pairs.first);
            }

            template<typename U>
            [[nodiscard]] U second() const {
                return Args::unbox_arg<U>(get_self<PairImpl>(*this)->pairs.second);
            }
        };
    }
}