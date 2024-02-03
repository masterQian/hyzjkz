module;
#include "MasterQian.Meta.h"
#define MasterQianModuleVersion 20240131ULL

export module MasterQian.freestanding;

/*    类型定义    */

// Windows DLL extern C 4字节 Boolean类型
export using mqbool = int;
// 8位有符号整数
export using mqi8 = signed char;
// 16位有符号整数
export using mqi16 = short;
// 32位有符号整数
export using mqi32 = int;
// 64位有符号整数
export using mqi64 = long long;
// 8位无符号整数
export using mqui8 = unsigned char;
// 16位无符号整数
export using mqui16 = unsigned short;
// 32位无符号整数
export using mqui32 = unsigned int;
// 64位无符号整数
export using mqui64 = unsigned long long;
// 32位浮点数
export using mqf32 = float;
// 64位浮点数
export using mqf64 = double;
// 枚举基类型
export using mqenum = mqui32;
// 内存地址
export using mqmem = void*;
// 只读内存地址
export using mqcmem = void const*;
// 句柄
export using mqhandle = void*;
// 字节
export using mqbyte = unsigned char;
// 字节流
export using mqbytes = unsigned char*;
// 只读字节流
export using mqcbytes = unsigned char const*;
// ASCII字符
export using mqchara = char;
// ASCII字符串
export using mqstra = char*;
// 只读ASCII字符串
export using mqcstra = char const*;
// 宽字符
export using mqchar = wchar_t;
// 宽字符串
export using mqstr = wchar_t*;
// 只读宽字符串
export using mqcstr = wchar_t const*;
// 函数调用
export using mqproc = long long(__stdcall*)() noexcept;
// GUID
export struct mqguid {
	mqui32 Data1;
	mqui16 Data2;
	mqui16 Data3;
	mqui8 Data4[8];
};
// 系统时间
export struct mqsystemtime {
	mqui16 year;
	mqui16 month;
	mqui16 week;
	mqui16 day;
	mqui16 hour;
	mqui16 minute;
	mqui16 second;
	mqui16 millisecond;
};
// 位置
export struct mqpoint {
	mqui32 x;
	mqui32 y;
};
// 尺寸
export struct mqsize {
	mqui32 width;
	mqui32 height;
};
// 矩形
export struct mqrect {
	mqui32 left;
	mqui32 top;
	mqui32 width;
	mqui32 height;
};
// 范围
export struct mqrange {
	mqui32 left;
	mqui32 top;
	mqui32 right;
	mqui32 bottom;
};



/*     freestanding concept    */

namespace MasterQian::freestanding {
	// remove_reference
	template <typename T>
	struct __remove_reference { using type = T; };

	template <typename T>
	struct __remove_reference<T&> { using type = T; };

	template <typename T>
	struct __remove_reference<T&&> { using type = T; };

	export template<typename T> using remove_reference = __remove_reference<T>::type;

	// remove_volatile
	template <typename T>
	struct __remove_volatile { using type = T; };

	template <typename T>
	struct __remove_volatile<T volatile> { using type = T; };

	export template<typename T> using remove_volatile = __remove_volatile<T>::type;

	// remove_const
	template <typename T>
	struct __remove_const { using type = T; };

	template <typename T>
	struct __remove_const<const T> { using type = T; };

	export template <typename T> using remove_const = __remove_const<T>::type;

	// remove_cv
	template <typename T>
	struct __remove_cv { using type = T; };

	template <typename T>
	struct __remove_cv<const T> { using type = T; };

	template <typename T>
	struct __remove_cv<volatile T> { using type = T; };

	template <class T>
	struct __remove_cv<const volatile T> { using type = T; };

	export template <typename T> using remove_cv = __remove_cv<T>::type;

	// remove_cvref
	export template<typename T> using remove_cvref = remove_cv<remove_reference<T>>;

	// copyable_memory
	export template<typename T> concept copyable_memory = __is_trivially_copyable(T);

	// construct_memory
	export template<typename T> concept construct_memory = __is_trivially_constructible(T);

	// trivial
	export template<typename T> concept trivial = copyable_memory<T> && construct_memory<T>;

	// same
	template <typename, typename>
	constexpr bool __same_v = false;

	template <typename T>
	constexpr bool __same_v<T, T> = true;

	export template<typename T1, typename T2> concept same = __same_v<T1, T2>&& __same_v<T2, T1>;

	// any_of
	export template <typename T, typename... Args> concept any_of = (same<T, Args> || ...);

	// character
	export template<typename T> concept character = any_of<remove_cv<T>,
		char, signed char, unsigned char, wchar_t, char8_t, char16_t, char32_t>;

	// integral
	export template <typename T> concept integral = character<T> || any_of<remove_cv<T>,
		bool, short, unsigned short, int, unsigned int,
		long, unsigned long, long long, unsigned long long>;

	// signed_integral
	export template <class T> concept signed_integral = integral<T> && static_cast<T>(-1) < static_cast<T>(0);

	// unsigned_integral
	export template <class T> concept unsigned_integral = integral<T> && !signed_integral<T>;

	// floating_point
	export template<typename T> concept floating_point = any_of<remove_cv<T>, float, double, long double>;

	// number
	export template<typename T> concept numeric = integral<T> || floating_point<T>;

	// arg_size
	export template<typename T, typename... Args> constexpr mqui64 arg_size = arg_size<Args...> + sizeof(remove_cvref<T>);

	export template<typename T> constexpr mqui64 arg_size<T> = sizeof(remove_cvref<T>);

	// isomerism_hash   isomerism_equal
	template <typename T>
	concept hash_string_type = requires(T t) {
		t.data();
		t.size();
		typename T::value_type;
	};

	export [[nodiscard]] inline constexpr mqui64 hash_bytes(mqcbytes data, mqui64 size) noexcept {
		mqui64 value{ 14695981039346656037ULL };
		for (size_t i{ }; i < size; ++i) {
			value ^= static_cast<mqui64>(data[i]);
			value *= 1099511628211ULL;
		}
		return value;
	}

	template<hash_string_type T>
	[[nodiscard]] inline constexpr mqui64 hash_impl(T const& t) noexcept {
		return hash_bytes(static_cast<mqcbytes>(static_cast<mqcmem>(t.data())),
			static_cast<mqui64>(t.size() * sizeof(typename T::value_type)));
	}

	template<mqui64 N>
	inline constexpr mqui64 hash_impl(mqchar const(&t)[N]) noexcept {
		return hash_bytes(static_cast<mqcbytes>(static_cast<mqcmem>(t)),
			(N - 1ULL) * sizeof(mqchar));
	}

	template<typename T>
	inline constexpr mqui64 hash_impl(T const& t) noexcept {
		return hash_bytes(static_cast<mqcbytes>(static_cast<mqcmem>(&t)), sizeof(T));
	}

	export struct isomerism_hash {
		template<typename T>
		[[nodiscard]] constexpr auto operator () (T&& t) const noexcept {
			return hash_impl(static_cast<T&&>(t));
		}
		using is_transparent = void;
	};

	export struct isomerism_equal {
		template <class T1, class T2>
		[[nodiscard]] constexpr auto operator()(T1&& t1, T2&& t2) const noexcept {
			return static_cast<T1&&>(t1) == static_cast<T2&&>(t2);
		}
		using is_transparent = void;
	};
}


/*    freestanding  function    */

export namespace MasterQian::freestanding {
	// forward
	template<typename T>
	[[nodiscard]] inline constexpr T&& forward(remove_reference<T>& t) noexcept {
		return static_cast<T&&>(t);
	}

	template<typename T>
	[[nodiscard]] inline constexpr T&& forward(remove_reference<T>&& t) noexcept {
		return static_cast<T&&>(t);
	}

	// move
	template<typename T>
	[[nodiscard]] inline constexpr remove_reference<T>&& move(T&& t) noexcept {
		return static_cast<remove_reference<T>&&>(t);
	}

	// swap
	template<typename T>
	inline constexpr void swap(T& t1, T& t2) noexcept {
		T tmp{ freestanding::move(t1) };
		t1 = freestanding::move(t2);
		t2 = freestanding::move(tmp);
	}

	// size
	template<typename T, mqui64 Len>
	[[nodiscard]] inline constexpr mqui64 size(T const(&)[Len]) noexcept {
		return Len;
	}

#if defined(__has_builtin)
#define ___memcpy___ __has_builtin(__builtin_memcpy)
#define ___memcmp___ __has_builtin(__builtin_memcmp)
#define ___memset___ __has_builtin(__builtin_memset)
#define ___bit_cast___ __has_builtin(__builtin_bit_cast)
#else
#define ___memcpy___ 0
#define ___memcmp___ 0
#define ___memset___ 0
#define ___bit_cast___ 0
#endif

	// compare
#if !___memcmp___
	extern "C" mqi32 __cdecl memcmp(mqcmem des, mqcmem src, mqui64 size);
#endif

	inline
#if ___memcmp___
		constexpr
#endif
		mqi32 compare(mqcmem des, mqcmem src, mqui64 size) noexcept {
#if ___memcmp___
		return __builtin_memcmp(des, src, size);
#else
		return memcmp(des, src, size);
#endif
	}

	// copy
#if !___memcpy___
	extern "C" mqmem __cdecl memcpy(mqmem des, mqcmem src, mqui64 size);
#endif

	inline
#if ___memcpy___
		constexpr
#endif
		mqmem copy(mqmem des, mqcmem src, mqui64 size) noexcept {
#if ___memcpy___
		return __builtin_memcpy(des, src, size);
#else
		return memcpy(des, src, size);
#endif
	}

	template<copyable_memory T>
	inline
#if ___memcpy___
		constexpr
#endif
		T* copy_n(T* des, mqcmem src, mqui64 n) noexcept {
		return static_cast<T*>(freestanding::copy(des, src, sizeof(T) * n));
	}

	// initialize
#if !___memset___
	extern "C" mqmem __cdecl memset(mqmem des, mqi32 val, mqui64 size);
#endif

	inline
#if ___memset___
		constexpr
#endif
		mqmem initialize(mqmem des, mqi32 val, mqui64 size) noexcept {
#if ___memset___
		return __builtin__memset(des, val, size);
#else
		return memset(des, val, size);
#endif
	}

	template<typename T>
	inline
#if ___memset___
		constexpr
#endif
		T* initialize_n(T* des, mqi32 val, mqui64 n) noexcept {
		return static_cast<T*>(freestanding::initialize(des, val, sizeof(T) * n));
	}

	// bit_cast
	template<typename To, typename From>
	requires (sizeof(To) == sizeof(From) && copyable_memory<To> && trivial<From>)
	[[nodiscard]] inline
#if ___memcpy___ || ___bit_cast___
		constexpr
#endif
		To bit_cast(From const& src) noexcept {
#if ___bit_cast___
		return __builtin_bit_cast(To, src);
#else
		To des;
		copy(&des, &src, sizeof(To));
		return des;
#endif
	}

	// ui64
	[[nodiscard]] inline constexpr mqui64 ui64(mqui32 low, mqui32 high) noexcept {
		return static_cast<mqui64>(low) | (static_cast<mqui64>(high) << 32ULL);
	}

	// low32
	[[nodiscard]] inline constexpr mqui32 low32(mqui64 v) noexcept {
		return static_cast<mqui32>(v & 0xFFFFFFFFULL);
	}

	// high32
	[[nodiscard]] inline constexpr mqui32 high32(mqui64 v) noexcept {
		return static_cast<mqui32>((v >> 32U) & 0xFFFFFFFFULL);
	}

	// str_i64
	[[nodiscard]] inline constexpr mqi64 str_i64(mqcstr nptr, mqcstr* endptr) noexcept {
		if (nptr) {
			bool is_neg{ };
			mqui64 n{ };
			mqcstr endp{ nptr };
			if (*nptr == L'-') {
				nptr++;
				is_neg = true;
			}
			while (*nptr >= L'0' && *nptr <= L'9') {
				n = n * 10ULL + *nptr - L'0';
				endp = ++nptr;
			}
			if (endptr) {
				*endptr = endp;
			}
			return is_neg ? -static_cast<mqi64>(n) : static_cast<mqi64>(n);
		}
		return 0LL;
	}

	// str_ui64
	[[nodiscard]] inline constexpr mqui64 str_ui64(mqcstr nptr, mqcstr* endptr) noexcept {
		mqui64 n{ };
		if (nptr) {
			mqcstr endp{ nptr };
			while (*nptr >= L'0' && *nptr <= L'9') {
				n = n * 10ULL + *nptr - L'0';
				endp = ++nptr;
			}
			if (endptr) {
				*endptr = endp;
			}
		}
		return n;
	}

	// str_hex
	[[nodiscard]] inline constexpr mqui64 str_hex(mqcstr nptr, mqcstr* endptr) noexcept {
		mqui64 n{ };
		if (nptr) {
			for (mqchar c{ *nptr }; c; c = *++nptr) {
				if (c >= L'0' && c <= L'9') {
					n = n * 16U + c - L'0';
				}
				else {
					c |= 32U;
					if (c >= L'a' && c <= L'f') n = n * 16U + c - L'a' + 10U;
					else {
						if (endptr) {
							*endptr = nptr;
						}
						break;
					}
				}
			}
		}
		return n;
	}

	// str_f64
	[[nodiscard]] inline constexpr mqf64 str_f64(mqcstr s, mqcstr* endptr) {
		mqcstr p{ s };
		mqf64 value{ };
		mqchar sign{ };
		mqf64 factor{ };
		if (*p == L'-' || *p == L'+') sign = *p++;
		while (*p - L'0' < 10U) value = value * 10U + (*p++ - L'0');
		if (*p == L'.') {
			factor = 1.0;
			p++;
			while (*p - L'0' < 10U) {
				factor *= 0.1;
				value += (*p++ - L'0') * factor;
			}
		}
		if ((*p | 32U) == L'e') {
			bool ret{ true };
			mqi32 expo{ };
			factor = 10.0;
			switch (*++p) {
			case L'-': factor = 0.1; [[fallthrough]];
			case L'+': ++p; break;
			case L'0': case L'1': case L'2':
			case L'3': case L'4': case L'5':
			case L'6': case L'7': case L'8':
			case L'9': break;
			default: value = 0.0; p = s; ret = false; break;
			}
			if (ret) {
				while (*p - L'0' < 10U) expo = 10 * expo + (*p++ - L'0');
				while (true) {
					if (expo & 1) value *= factor;
					if ((expo >>= 1) == 0) break;
					factor *= factor;
				}
			}
		}
		if (endptr) *endptr = p;
		return sign == L'-' ? -value : value;
	}
}


/*    freestanding type transfer    */

// guid
namespace MasterQian::freestanding {
	template<integral T>
	inline constexpr bool guid_parse(T& data, mqcstr str, mqui64 start) noexcept {
		T base{ 1U << (((sizeof(T) << 1U) - 1U) * 4U) };
		for (mqui64 i{ start }, end{ start + (sizeof(T) << 1ULL) }; i < end; ++i) {
			if (str[i] >= L'0' && str[i] <= L'9') {
				data += static_cast<T>((str[i] - L'0') * base);
			}
			else if (str[i] >= L'a' && str[i] <= L'f') {
				data += static_cast<T>((str[i] - L'a' + 10U) * base);
			}
			else if (str[i] >= L'A' && str[i] <= L'F') {
				data += static_cast<T>((str[i] - L'A' + 10U) * base);
			}
			else {
				return false;
			}
			base >>= 4U;
		}
		return true;
	}
}

export [[nodiscard]] inline constexpr mqguid operator ""_guid(mqcstr str, mqui64 size) noexcept {
	mqguid guid{ };
	if (size != 36ULL || str[8] != L'-' || str[13] != L'-' || str[18] != L'-' || str[23] != L'-') {
		return { };
	}
	if (!MasterQian::freestanding::guid_parse<mqui32>(guid.Data1, str, 0)) {
		return { };
	}
	if (!MasterQian::freestanding::guid_parse<mqui16>(guid.Data2, str, 9)) {
		return { };
	}
	if (!MasterQian::freestanding::guid_parse<mqui16>(guid.Data3, str, 14)) {
		return { };
	}
	if (!MasterQian::freestanding::guid_parse<mqui8>(guid.Data4[0], str, 19)) {
		return { };
	}
	if (!MasterQian::freestanding::guid_parse<mqui8>(guid.Data4[1], str, 21)) {
		return { };
	}
	if (!MasterQian::freestanding::guid_parse<mqui8>(guid.Data4[2], str, 24)) {
		return { };
	}
	if (!MasterQian::freestanding::guid_parse<mqui8>(guid.Data4[3], str, 26)) {
		return { };
	}
	if (!MasterQian::freestanding::guid_parse<mqui8>(guid.Data4[4], str, 28)) {
		return { };
	}
	if (!MasterQian::freestanding::guid_parse<mqui8>(guid.Data4[5], str, 30)) {
		return { };
	}
	if (!MasterQian::freestanding::guid_parse<mqui8>(guid.Data4[6], str, 32)) {
		return { };
	}
	if (!MasterQian::freestanding::guid_parse<mqui8>(guid.Data4[7], str, 34)) {
		return { };
	}
	return guid;
}

// mqpoint
export [[nodiscard]] bool operator == (mqpoint const& point1, mqpoint const& point2) noexcept {
	return point1.x == point2.x && point1.y == point2.y;
}

export template<MasterQian::freestanding::numeric T>
mqpoint& operator += (mqpoint& point, T value) noexcept {
	point.x = static_cast<mqui32>(point.x + value);
	point.y = static_cast<mqui32>(point.y + value);
	return point;
}

export template<MasterQian::freestanding::numeric T>
[[nodiscard]] mqpoint operator + (mqpoint const& point, T value) noexcept {
	mqpoint tmp{ point };
	tmp += value;
	return tmp;
}

export template<MasterQian::freestanding::numeric T>
mqpoint& operator -= (mqpoint& point, T value) noexcept {
	point.x = static_cast<mqui32>(point.x - value);
	point.y = static_cast<mqui32>(point.y - value);
	return point;
}

export template<MasterQian::freestanding::numeric T>
[[nodiscard]] mqpoint operator - (mqpoint const& point, T value) noexcept {
	mqpoint tmp{ point };
	tmp -= value;
	return tmp;
}

export template<MasterQian::freestanding::numeric T>
mqpoint& operator *= (mqpoint& point, T value) noexcept {
	point.x = static_cast<mqui32>(point.x * value);
	point.y = static_cast<mqui32>(point.y * value);
	return point;
}

export template<MasterQian::freestanding::numeric T>
[[nodiscard]] mqpoint operator * (mqpoint const& point, T value) noexcept {
	mqpoint tmp{ point };
	tmp *= value;
	return tmp;
}

export template<MasterQian::freestanding::numeric T>
mqpoint& operator /= (mqpoint& point, T value) noexcept {
	point.x = static_cast<mqui32>(point.x / value);
	point.y = static_cast<mqui32>(point.y / value);
	return point;
}

export template<MasterQian::freestanding::numeric T>
[[nodiscard]] mqpoint operator / (mqpoint const& point, T value) noexcept {
	mqpoint tmp{ point };
	tmp /= value;
	return tmp;
}

// mqsize
export [[nodiscard]] bool operator == (mqsize const& size1, mqsize const& size2) noexcept {
	return size1.width == size2.width && size1.height == size2.height;
}

export template<MasterQian::freestanding::numeric T>
mqsize& operator += (mqsize& size, T value) noexcept {
	size.width = static_cast<mqui32>(size.width + value);
	size.height = static_cast<mqui32>(size.height + value);
	return size;
}

export template<MasterQian::freestanding::numeric T>
[[nodiscard]] mqsize operator + (mqsize const& size, T value) noexcept {
	mqsize tmp{ size };
	tmp += value;
	return tmp;
}

export template<MasterQian::freestanding::numeric T>
mqsize& operator -= (mqsize& size, T value) noexcept {
	size.width = static_cast<mqui32>(size.width - value);
	size.height = static_cast<mqui32>(size.height - value);
	return size;
}

export template<MasterQian::freestanding::numeric T>
[[nodiscard]] mqsize operator - (mqsize const& size, T value) noexcept {
	mqsize tmp{ size };
	tmp -= value;
	return tmp;
}

export template<MasterQian::freestanding::numeric T>
mqsize& operator *= (mqsize& size, T value) noexcept {
	size.width = static_cast<mqui32>(size.width * value);
	size.height = static_cast<mqui32>(size.height * value);
	return size;
}

export template<MasterQian::freestanding::numeric T>
[[nodiscard]] mqsize operator * (mqsize const& size, T value) noexcept {
	mqsize tmp{ size };
	tmp *= value;
	return tmp;
}

export template<MasterQian::freestanding::numeric T>
mqsize& operator /= (mqsize& size, T value) noexcept {
	size.width = static_cast<mqui32>(size.width / value);
	size.height = static_cast<mqui32>(size.height / value);
	return size;
}

export template<MasterQian::freestanding::numeric T>
[[nodiscard]] mqsize operator / (mqsize const& size, T value) noexcept {
	mqsize tmp{ size };
	tmp /= value;
	return tmp;
}

// mqrect
export [[nodiscard]] bool operator == (mqrect const& rect1, mqrect const& rect2) noexcept {
	return rect1.left == rect2.left && rect1.top == rect2.top && rect1.width == rect2.width && rect1.height == rect2.height;
}

export template<MasterQian::freestanding::numeric T>
mqrect& operator += (mqrect& rect, T value) noexcept {
	rect.left = static_cast<mqui32>(rect.left + value);
	rect.top = static_cast<mqui32>(rect.top + value);
	rect.width = static_cast<mqui32>(rect.width + value);
	rect.height = static_cast<mqui32>(rect.height + value);
	return rect;
}

export template<MasterQian::freestanding::numeric T>
[[nodiscard]] mqrect operator + (mqrect const& rect, T value) noexcept {
	mqrect tmp{ rect };
	tmp += value;
	return tmp;
}

export template<MasterQian::freestanding::numeric T>
mqrect& operator -= (mqrect& rect, T value) noexcept {
	rect.left = static_cast<mqui32>(rect.left - value);
	rect.top = static_cast<mqui32>(rect.top - value);
	rect.width = static_cast<mqui32>(rect.width - value);
	rect.height = static_cast<mqui32>(rect.height - value);
	return rect;
}

export template<MasterQian::freestanding::numeric T>
[[nodiscard]] mqrect operator - (mqrect const& rect, T value) noexcept {
	mqrect tmp{ rect };
	tmp -= value;
	return tmp;
}

export template<MasterQian::freestanding::numeric T>
mqrect& operator *= (mqrect& rect, T value) noexcept {
	rect.left = static_cast<mqui32>(rect.left * value);
	rect.top = static_cast<mqui32>(rect.top * value);
	rect.width = static_cast<mqui32>(rect.width * value);
	rect.height = static_cast<mqui32>(rect.height * value);
	return rect;
}

export template<MasterQian::freestanding::numeric T>
[[nodiscard]] mqrect operator * (mqrect const& rect, T value) noexcept {
	mqrect tmp{ rect };
	tmp *= value;
	return tmp;
}

export template<MasterQian::freestanding::numeric T>
mqrect& operator /= (mqrect& rect, T value) noexcept {
	rect.left = static_cast<mqui32>(rect.left / value);
	rect.top = static_cast<mqui32>(rect.top / value);
	rect.width = static_cast<mqui32>(rect.width / value);
	rect.height = static_cast<mqui32>(rect.height / value);
	return rect;
}

export template<MasterQian::freestanding::numeric T>
[[nodiscard]] mqrect operator / (mqrect const& rect, T value) noexcept {
	mqrect tmp{ rect };
	tmp /= value;
	return tmp;
}

// mqrange
export [[nodiscard]] bool operator == (mqrange const& range1, mqrange const& range2) noexcept {
	return range1.left == range2.left && range1.top == range2.top && range1.right == range2.right && range1.bottom == range2.bottom;
}

export template<MasterQian::freestanding::numeric T>
mqrange& operator += (mqrange& range, T value) noexcept {
	range.left = static_cast<mqui32>(range.left + value);
	range.top = static_cast<mqui32>(range.top + value);
	range.right = static_cast<mqui32>(range.right + value);
	range.bottom = static_cast<mqui32>(range.bottom + value);
	return range;
}

export template<MasterQian::freestanding::numeric T>
[[nodiscard]] mqrange operator + (mqrange const& range, T value) noexcept {
	mqrange tmp{ range };
	tmp += value;
	return tmp;
}

export template<MasterQian::freestanding::numeric T>
mqrange& operator -= (mqrange& range, T value) noexcept {
	range.left = static_cast<mqui32>(range.left - value);
	range.top = static_cast<mqui32>(range.top - value);
	range.right = static_cast<mqui32>(range.right - value);
	range.bottom = static_cast<mqui32>(range.bottom - value);
	return range;
}

export template<MasterQian::freestanding::numeric T>
[[nodiscard]] mqrange operator - (mqrange const& range, T value) noexcept {
	mqrange tmp{ range };
	tmp -= value;
	return tmp;
}

export template<MasterQian::freestanding::numeric T>
mqrange& operator *= (mqrange& range, T value) noexcept {
	range.left = static_cast<mqui32>(range.left * value);
	range.top = static_cast<mqui32>(range.top * value);
	range.right = static_cast<mqui32>(range.right * value);
	range.bottom = static_cast<mqui32>(range.bottom * value);
	return range;
}

export template<MasterQian::freestanding::numeric T>
[[nodiscard]] mqrange operator * (mqrange const& range, T value) noexcept {
	mqrange tmp{ range };
	tmp *= value;
	return tmp;
}

export template<MasterQian::freestanding::numeric T>
mqrange& operator /= (mqrange& range, T value) noexcept {
	range.left = static_cast<mqui32>(range.left / value);
	range.top = static_cast<mqui32>(range.top / value);
	range.right = static_cast<mqui32>(range.right / value);
	range.bottom = static_cast<mqui32>(range.bottom / value);
	return range;
}

export template<MasterQian::freestanding::numeric T>
[[nodiscard]] mqrange operator / (mqrange const& range, T value) noexcept {
	mqrange tmp{ range };
	tmp /= value;
	return tmp;
}

/*    freestanding struct    */

namespace MasterQian::freestanding {
	// 初始化器
	export template<copyable_memory T> struct mqinit {
		T value;
		explicit mqinit(T(*func)() noexcept) noexcept : value{ func() } {}
		template<typename U>
		mqinit(T(*func)(U&& u) noexcept, U&& u) noexcept : value{ func(forward<U>(u)) } {}
		[[nodiscard]] operator T () const noexcept {
			return value;
		}
	};
}

/*    Win32 API    */

export namespace MasterQian::api {
	constexpr mqui32 PATH_MAX_SIZE{ 260U };
}

export namespace MasterQian::api {
	META_WINAPI(void*, LoadLibraryW, mqcstr);
	META_WINAPI(mqproc, GetProcAddress, mqhandle, mqcstra);
	META_WINAPI(mqbool, FreeLibrary, mqhandle);
	META_WINAPI(mqi32, MessageBoxW, mqhandle, mqcstr, mqcstr, mqui32);
	META_WINAPI(__declspec(noreturn) void, ExitProcess, mqui32);
	META_WINAPI(mqstra, lstrcpyA, mqstra, mqcstra);
	META_WINAPI(mqstra, lstrcatA, mqstra, mqcstra);
	META_WINAPI(mqi32, lstrcmpW, mqcstr, mqcstr);
	META_WINAPI(mqi32, lstrcmpiW, mqcstr, mqcstr);
	META_WINAPI(mqstr, lstrcpynW, mqstr, mqcstr, mqui32);
	META_WINAPI(mqstr, lstrcpyW, mqstr, mqcstr);
	META_WINAPI(mqstr, lstrcatW, mqstr, mqcstr);
	META_WINAPI(mqui32, lstrlenA, mqcstra);
	META_WINAPI(mqui32, lstrlenW, mqcstr);
	META_WINAPI(mqi32, wsprintfW, mqstr, mqcstr, ...);
	META_WINAPI(mqi32, MultiByteToWideChar, mqui32, mqui32, mqcstra, mqi32, mqstr, mqi32);
	META_WINAPI(mqi32, WideCharToMultiByte, mqui32, mqui32, mqcstr, mqi32, mqstra, mqi32, mqcstra, mqbool*);
	META_WINAPI(void, GetSystemTimeAsFileTime, mqui64*);
	META_WINAPI(mqbool, FileTimeToSystemTime, mqui64 const*, mqsystemtime*);
	META_WINAPI(mqbool, SystemTimeToFileTime, mqsystemtime const*, mqui64*);
}

// 导出名重链接
#pragma comment(linker,"/alternatename:__imp_?LoadLibraryW@api@MasterQian@@YAPEAXPEB_W@Z::<!MasterQian.freestanding>=__imp_LoadLibraryW")
#pragma comment(linker,"/alternatename:__imp_?GetProcAddress@api@MasterQian@@YAP6A_JX_EPEAXPEBD@Z::<!MasterQian.freestanding>=__imp_GetProcAddress")
#pragma comment(linker,"/alternatename:__imp_?FreeLibrary@api@MasterQian@@YAHPEAX@Z::<!MasterQian.freestanding>=__imp_FreeLibrary")
#pragma comment(linker,"/alternatename:__imp_?MessageBoxW@api@MasterQian@@YAHPEAXPEB_W1I@Z::<!MasterQian.freestanding>=__imp_MessageBoxW")
#pragma comment(linker,"/alternatename:__imp_?ExitProcess@api@MasterQian@@YAXI@Z::<!MasterQian.freestanding>=__imp_ExitProcess")
#pragma comment(linker,"/alternatename:__imp_?lstrcpyA@api@MasterQian@@YAPEADPEADPEBD@Z::<!MasterQian.freestanding>=__imp_lstrcpyA")
#pragma comment(linker,"/alternatename:__imp_?lstrcatA@api@MasterQian@@YAPEADPEADPEBD@Z::<!MasterQian.freestanding>=__imp_lstrcatA")
#pragma comment(linker,"/alternatename:__imp_?lstrcmpW@api@MasterQian@@YAHPEB_W0@Z::<!MasterQian.freestanding>=__imp_lstrcmpW")
#pragma comment(linker,"/alternatename:__imp_?lstrcmpiW@api@MasterQian@@YAHPEB_W0@Z::<!MasterQian.freestanding>=__imp_lstrcmpiW")
#pragma comment(linker,"/alternatename:__imp_?lstrcpynW@api@MasterQian@@YAPEA_WPEA_WPEB_WI@Z::<!MasterQian.freestanding>=__imp_lstrcpynW")
#pragma comment(linker,"/alternatename:__imp_?lstrcpyW@api@MasterQian@@YAPEA_WPEA_WPEB_W@Z::<!MasterQian.freestanding>=__imp_lstrcpyW")
#pragma comment(linker,"/alternatename:__imp_?lstrcatW@api@MasterQian@@YAPEA_WPEA_WPEB_W@Z::<!MasterQian.freestanding>=__imp_lstrcatW")
#pragma comment(linker,"/alternatename:__imp_?lstrlenA@api@MasterQian@@YAIPEBD@Z::<!MasterQian.freestanding>=__imp_lstrlenA")
#pragma comment(linker,"/alternatename:__imp_?lstrlenW@api@MasterQian@@YAIPEB_W@Z::<!MasterQian.freestanding>=__imp_lstrlenW")
#pragma comment(linker,"/alternatename:__imp_?wsprintfW@api@MasterQian@@YAHPEA_WPEB_WZZ::<!MasterQian.freestanding>=__imp_wsprintfW")
#pragma comment(linker,"/alternatename:__imp_?MultiByteToWideChar@api@MasterQian@@YAHIIPEBDHPEA_WH@Z::<!MasterQian.freestanding>=__imp_MultiByteToWideChar")
#pragma comment(linker,"/alternatename:__imp_?WideCharToMultiByte@api@MasterQian@@YAHIIPEB_WHPEADHPEBDPEAH@Z::<!MasterQian.freestanding>=__imp_WideCharToMultiByte")
#pragma comment(linker,"/alternatename:__imp_?GetSystemTimeAsFileTime@api@MasterQian@@YAXPEA_K@Z::<!MasterQian.freestanding>=__imp_GetSystemTimeAsFileTime")
#pragma comment(linker,"/alternatename:__imp_?FileTimeToSystemTime@api@MasterQian@@YAHPEB_KPEAUmqsystemtime@@@Z::<!MasterQian.freestanding>=__imp_FileTimeToSystemTime")
#pragma comment(linker,"/alternatename:__imp_?SystemTimeToFileTime@api@MasterQian@@YAHPEBUmqsystemtime@@PEA_K@Z::<!MasterQian.freestanding>=__imp_SystemTimeToFileTime")