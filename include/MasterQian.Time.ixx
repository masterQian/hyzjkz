module;
#include "MasterQian.Meta.h"
#include <string>
#define MasterQianModuleVersion 20240131ULL

export module MasterQian.Time;
export import MasterQian.freestanding;

namespace MasterQian {
	namespace details {
		constexpr mqui64 UnixDST{ 116444736000000000ULL };
		constexpr mqui64 BASE10POW[]{ 1ULL, 10ULL, 100ULL, 1000ULL, 10000ULL, 100000ULL, 1000000ULL, 10000000ULL };
	}

	// ����ʱ��
	export struct Time {
		mqui16 year;
		mqui8 month;
		mqui8 day;
		mqui8 hour;
		mqui8 minute;
		mqui8 second;
		mqui8 week;
		mqui16 millisecond;
		mqui16 microsecond;

		constexpr Time(mqui16 YY, mqui8 MM, mqui8 DD, mqui8 hh, mqui8 mm, mqui8 ss,
			mqui16 milli = 0U, mqui16 micro = 0U, mqui8 week = 0U) noexcept : year{ YY }, month{ MM }, day{ DD },
			hour{ hh }, minute{ mm }, second{ ss }, week{ week }, millisecond{ milli }, microsecond{ micro } {}

		/// <summary>���Ƚ����ڲ��Ƚ�ʱ��</summary>
		[[nodiscard]] constexpr bool compare_date(Time const& t) const noexcept {
			return year == t.year && month == t.month && day == t.day;
		}

		/// <summary>
		/// ��ʽ������2024-01-23 22:38:49.0123 2
		/// </summary>
		[[nodiscard]] std::wstring format() const noexcept {
			mqchar str[32ULL];
			api::wsprintfW(str, L"%04hu-%02u-%02u %02u:%02u:%02u.%03hu.%04hu %u",
				year, month, day, hour, minute, second, millisecond, microsecond, week);
			return str;
		}

		/// <summary>
		/// ��ʽ������20240123
		/// </summary>
		[[nodiscard]] std::wstring formatDate() const noexcept {
			mqchar str[16ULL];
			api::wsprintfW(str, L"%04hu%02u%02u", year, month, day);
			return str;
		}

		/// <summary>
		/// ��ʽ������223849
		/// </summary>
		[[nodiscard]] std::wstring formatTime() const noexcept {
			mqchar str[8ULL];
			api::wsprintfW(str, L"%02u%02u%02u", hour, minute, second);
			return str;
		}

		/// <summary>
		/// ��ʽ������20240123223849
		/// </summary>
		[[nodiscard]] std::wstring formatDateTime() const noexcept {
			mqchar str[32ULL];
			api::wsprintfW(str, L"%04hu%02u%02u%02u%02u%02u", year, month, day, hour, minute, second);
			return str;
		}
	};

	// ʱ���
	export struct Timestamp {
	private:
		mqui64 value{ };
	public:
		// ʱ�䵥λ
		enum Type : mqui64 {
			// ��
			second = 10ULL,
			// ����
			millisecond = 13ULL,
			// ΢��
			microsecond = 17ULL,
		};

		// ʱ��
		enum Zone : mqui64 {
			// ����ʱ��
			CN_BEIJING = 288000000000ULL,
		};

		/// <summary>
		/// �Ե�ǰϵͳʱ�乹��
		/// </summary>
		Timestamp() noexcept {
			api::GetSystemTimeAsFileTime(&value);
			value -= details::UnixDST;
		}

		/// <summary>
		/// ��ʱ�������
		/// </summary>
		/// <param name="v">ʱ���</param>
		/// <param name="type">ʱ�䵥λ��Ĭ��Ϊ΢�룬��17λʱ���</param>
		constexpr Timestamp(mqui64 v, Type type = Type::microsecond, bool isFileTime = false) noexcept : value{ v } {
			value *= details::BASE10POW[static_cast<mqui64>(Type::microsecond) - static_cast<mqui64>(type)];
			if (isFileTime) value -= details::UnixDST;
		}

		/// <summary>
		/// ������ʱ�乹��
		/// </summary>
		/// <param name="t">����ʱ��</param>
		/// <param name="zone">ʱ����Ĭ��Ϊ����</param>
		Timestamp(Time const& t, Zone zone = Zone::CN_BEIJING) noexcept {
			mqsystemtime st{
				.year = t.year,
				.month = t.month,
				.week = t.week,
				.day = t.day,
				.hour = t.hour,
				.minute = t.minute,
				.second = t.second,
				.millisecond = t.millisecond
			};
			api::SystemTimeToFileTime(&st, &value);
			value -= details::UnixDST + zone - t.microsecond;
		}

		/// <summary>
		/// ȡʱ�����ֵ
		/// </summary>
		template<Type type = Type::microsecond>
		[[nodiscard]] constexpr mqui64 stamp() const noexcept {
			return value / details::BASE10POW[static_cast<mqui64>(Type::microsecond) - static_cast<mqui64>(type)];
		}

		[[nodiscard]] constexpr operator mqui64 () const noexcept {
			return value;
		}

		/// <summary>
		/// ȡ����ʱ��
		/// </summary>
		/// <param name="zone">ʱ��</param>
		/// <returns>����ʱ�����Ӧʱ����ʱ��</returns>
		Time local(Zone zone = Zone::CN_BEIJING) const noexcept {
			mqui64 zone_value = value + details::UnixDST + zone;
			mqsystemtime st{ };
			api::FileTimeToSystemTime(&zone_value, &st);
			return Time{ st.year, static_cast<mqui8>(st.month), static_cast<mqui8>(st.day),
				static_cast<mqui8>(st.hour), static_cast<mqui8>(st.minute), static_cast<mqui8>(st.second),
				st.millisecond, static_cast<mqui16>(zone_value % 10000ULL), static_cast<mqui8>(st.week)
			};
		}

		/// <summary>
		/// �����
		/// </summary>
		/// <param name="count">����</param>
		constexpr void add_day(mqi64 count) noexcept {
			value += count * 24ULL * 3600ULL * 1000ULL * 10000ULL;
		}

		constexpr Timestamp& operator -= (freestanding::integral auto v) noexcept {
			value -= static_cast<mqui64>(v);
			return *this;
		}
		
		constexpr Timestamp operator - (freestanding::integral auto v) const noexcept {
			Timestamp tmp{ *this };
			tmp -= v;
			return tmp;
		}

		constexpr mqi64 operator - (Timestamp ts) const noexcept {
			return static_cast<mqi64>(value - ts.value);
		}

		constexpr Timestamp& operator += (freestanding::integral auto v) noexcept {
			value += static_cast<mqui64>(v);
			return *this;
		}

		friend inline constexpr Timestamp operator + (freestanding::integral auto v, Timestamp ts) noexcept {
			Timestamp tmp{ ts };
			tmp += v;
			return tmp;
		}
	};
}

export [[nodiscard]] inline constexpr MasterQian::Timestamp operator ""_timestamp(mqui64 value) noexcept {
	return MasterQian::Timestamp{ value };
}

export [[nodiscard]] inline MasterQian::Time operator ""_time(mqui64 value) noexcept {
	return MasterQian::Timestamp{ value }.local();
}