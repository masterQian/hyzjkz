module;
#include "MasterQian.Meta.h"
#define MasterQianModuleVersion 20240131ULL

export module MasterQian.Media.Color;
export import MasterQian.freestanding;

namespace MasterQian::Media {
	// ARGB颜色
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

	// 颜色枚举
	export namespace Colors {
#define COLOR_ENUM(name, R, G, B, A, comment) constexpr Color name{ R, G, B, A }
#define COLOR_ENUM_V(name, color, comment) constexpr Color name { color }
		COLOR_ENUM(Transparency, 0, 0, 0, 0, "透明");
		COLOR_ENUM(White, 255, 255, 255, 255, "白色");
		COLOR_ENUM(Black, 0, 0, 0, 255, "黑色");
		COLOR_ENUM(Red, 255, 0, 0, 255, "红色");
		COLOR_ENUM(Orange, 255, 165, 0, 255, "橙色");
		COLOR_ENUM(Yellow, 255, 255, 0, 255, "黄色");
		COLOR_ENUM(Green, 0, 255, 0, 255, "绿色");
		COLOR_ENUM(Cyan, 0, 255, 255, 255, "青色");
		COLOR_ENUM(Blue, 0, 0, 255, 255, "蓝色");
		COLOR_ENUM(Purple, 128, 0, 128, 255, "紫色");
		COLOR_ENUM(Gray, 128, 128, 128, 255, "灰色");
		COLOR_ENUM(Brown, 165, 42, 42, 255, "棕色");
		COLOR_ENUM(Pink, 255, 192, 203, 255, "紫色");
		COLOR_ENUM_V(AliceBlue, 0xFFF0F8FF, "爱丽丝蓝");
		COLOR_ENUM_V(AntiqueWhite, 0xFFFAEBD7, "古董白");
		COLOR_ENUM_V(DarkBlue, 0xFF00008B, "深蓝色");
		COLOR_ENUM_V(DarkCyan, 0xFF008B8B, "深青色");
		COLOR_ENUM_V(DarkGray, 0xFFA9A9A9, "深灰色");
		COLOR_ENUM_V(DarkGreen, 0xFF006400, "深绿色");
		COLOR_ENUM_V(DarkOrange, 0xFFFF8C00, "深橙色");
		COLOR_ENUM_V(DarkRed, 0xFF8B0000, "深红色");
		COLOR_ENUM_V(LightBlue, 0xFFADD8E6, "淡蓝色");
		COLOR_ENUM_V(LightCyan, 0xFFE0FFFF, "淡青色");
		COLOR_ENUM_V(LightGray, 0xFFD3D3D3, "淡灰色");
		COLOR_ENUM_V(LightGreen, 0xFF90EE90, "淡绿色");
		COLOR_ENUM_V(LightPink, 0xFFFFB5C1, "淡粉色");
		COLOR_ENUM_V(LightYellow, 0xFFFFFFE0, "淡黄色");
		COLOR_ENUM_V(SeaGreen, 0xFF2E8B57, "海绿色");
		COLOR_ENUM_V(SkyBlue, 0xFF87CEEB, "天蓝色");
		COLOR_ENUM_V(Silver, 0xFFC0C0C0, "银色");
		COLOR_ENUM_V(Snow, 0xFFFFFAFA, "雪白色");
		COLOR_ENUM_V(SteelBlue, 0xFF4682B4, "钢蓝色");
		COLOR_ENUM_V(SpringGreen, 0xFF00FF7F, "春绿色");
		COLOR_ENUM_V(YellowGreen, 0xFF9ACD32, "黄绿色");
		COLOR_ENUM_V(Violet, 0xFFEE82EE, "紫罗兰色");
	};
}

export [[nodiscard]] inline constexpr MasterQian::Media::Color operator ""_color(mqui64 value) noexcept {
	return MasterQian::Media::Color{ static_cast<mqui32>(value) };
}