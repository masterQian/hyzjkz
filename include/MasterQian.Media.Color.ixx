module;
#include "MasterQian.Meta.h"
#define MasterQianModuleVersion 20240131ULL

export module MasterQian.Media.Color;
export import MasterQian.freestanding;

namespace MasterQian::Media {
	// ARGB��ɫ
	export struct Color {
	private:
		mqui32 color;
	public:
		constexpr Color(mqbyte r, mqbyte g, mqbyte b, mqbyte alpha = 255U) noexcept {
			color = static_cast<mqui32>(b) | (static_cast<mqui32>(g) << 8U) |
				(static_cast<mqui32>(r) << 16U) | (static_cast<mqui32>(alpha) << 24U);
		}

		explicit constexpr Color(mqui32 argb) noexcept : color{ argb } {}

		[[nodiscard]] constexpr mqbyte A() const noexcept {
			return static_cast<mqbyte>(color >> 24U) & 0xFFU;
		}

		[[nodiscard]] constexpr mqbyte R() const noexcept {
			return static_cast<mqbyte>(color >> 16U) & 0xFFU;
		}

		[[nodiscard]] constexpr mqbyte G() const noexcept {
			return static_cast<mqbyte>(color >> 8U) & 0xFFU;
		}

		[[nodiscard]] constexpr mqbyte B() const noexcept {
			return static_cast<mqbyte>(color) & 0xFFU;
		}

		[[nodiscard]] constexpr operator mqui32() const noexcept {
			return color;
		}
	};

	// ��ɫö��
	export namespace Colors {
#define COLOR_ENUM(name, R, G, B, A, comment) constexpr Color name{ R, G, B, A }
#define COLOR_ENUM_V(name, color, comment) constexpr Color name { color }
		COLOR_ENUM(Transparency, 0, 0, 0, 0, "͸��");
		COLOR_ENUM(White, 255, 255, 255, 255, "��ɫ");
		COLOR_ENUM(Black, 0, 0, 0, 255, "��ɫ");
		COLOR_ENUM(Red, 255, 0, 0, 255, "��ɫ");
		COLOR_ENUM(Orange, 255, 165, 0, 255, "��ɫ");
		COLOR_ENUM(Yellow, 255, 255, 0, 255, "��ɫ");
		COLOR_ENUM(Green, 0, 255, 0, 255, "��ɫ");
		COLOR_ENUM(Cyan, 0, 255, 255, 255, "��ɫ");
		COLOR_ENUM(Blue, 0, 0, 255, 255, "��ɫ");
		COLOR_ENUM(Purple, 128, 0, 128, 255, "��ɫ");
		COLOR_ENUM(Gray, 128, 128, 128, 255, "��ɫ");
		COLOR_ENUM(Brown, 165, 42, 42, 255, "��ɫ");
		COLOR_ENUM(Pink, 255, 192, 203, 255, "��ɫ");
		COLOR_ENUM_V(AliceBlue, 0xFFF0F8FF, "����˿��");
		COLOR_ENUM_V(AntiqueWhite, 0xFFFAEBD7, "�Ŷ���");
		COLOR_ENUM_V(DarkBlue, 0xFF00008B, "����ɫ");
		COLOR_ENUM_V(DarkCyan, 0xFF008B8B, "����ɫ");
		COLOR_ENUM_V(DarkGray, 0xFFA9A9A9, "���ɫ");
		COLOR_ENUM_V(DarkGreen, 0xFF006400, "����ɫ");
		COLOR_ENUM_V(DarkOrange, 0xFFFF8C00, "���ɫ");
		COLOR_ENUM_V(DarkRed, 0xFF8B0000, "���ɫ");
		COLOR_ENUM_V(LightBlue, 0xFFADD8E6, "����ɫ");
		COLOR_ENUM_V(LightCyan, 0xFFE0FFFF, "����ɫ");
		COLOR_ENUM_V(LightGray, 0xFFD3D3D3, "����ɫ");
		COLOR_ENUM_V(LightGreen, 0xFF90EE90, "����ɫ");
		COLOR_ENUM_V(LightPink, 0xFFFFB5C1, "����ɫ");
		COLOR_ENUM_V(LightYellow, 0xFFFFFFE0, "����ɫ");
		COLOR_ENUM_V(SeaGreen, 0xFF2E8B57, "����ɫ");
		COLOR_ENUM_V(SkyBlue, 0xFF87CEEB, "����ɫ");
		COLOR_ENUM_V(Silver, 0xFFC0C0C0, "��ɫ");
		COLOR_ENUM_V(Snow, 0xFFFFFAFA, "ѩ��ɫ");
		COLOR_ENUM_V(SteelBlue, 0xFF4682B4, "����ɫ");
		COLOR_ENUM_V(SpringGreen, 0xFF00FF7F, "����ɫ");
		COLOR_ENUM_V(YellowGreen, 0xFF9ACD32, "����ɫ");
		COLOR_ENUM_V(Violet, 0xFFEE82EE, "������ɫ");
	};
}

export [[nodiscard]] inline constexpr MasterQian::Media::Color operator ""_color(mqui64 value) noexcept {
	return MasterQian::Media::Color{ static_cast<mqui32>(value) };
}