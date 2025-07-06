module;
#include "MasterQian.Meta.h"
#include <string>
#define MasterQianModuleName(name) MasterQian_Media_Graph_##name
#define MasterQianModuleNameString(name) "MasterQian_Media_Graph_"#name
#ifdef _DEBUG
#define MasterQianLibString "MasterQian.Media.Graph.Debug.dll"
#else
#define MasterQianLibString "MasterQian.Media.Graph.dll"
#endif
#define MasterQianModuleVersion 20240205ULL
#pragma message("—————————— Please copy [" MasterQianLibString "] into your program folder ——————————")

export module MasterQian.Media.Graph;
export import MasterQian.Bin;
export import MasterQian.Media.GDI;

namespace MasterQian::Media::Graph {
	namespace details {
		META_IMPORT_API(mqhandle, Canvas_Create, mqsize, Color);
		META_IMPORT_API(void, Canvas_Delete, mqhandle);
		META_IMPORT_API(mqhandle, Canvas_BeginRender, mqhandle);
		META_IMPORT_API(void, Canvas_EndRender, mqhandle);
		META_IMPORT_API(void, Canvas_Render, mqhandle, mqrect const*, mqcmem, mqenum);
		META_IMPORT_API(mqhandle, Canvas_SaveToStream, mqhandle, mqui32*);
		META_IMPORT_API(void, Canvas_StreamReadRelease, mqhandle, mqbytes, mqui32);
		META_MODULE_BEGIN
			META_PROC_API(Canvas_Create);
			META_PROC_API(Canvas_Delete);
			META_PROC_API(Canvas_BeginRender);
			META_PROC_API(Canvas_EndRender);
			META_PROC_API(Canvas_Render);
			META_PROC_API(Canvas_SaveToStream);
			META_PROC_API(Canvas_StreamReadRelease);
		META_MODULE_END
	}
}

namespace MasterQian::Media::Graph {
	enum class Type : mqenum {
		Line,
		Rectangle,
		Circle,
		Text,
		Picture,
		TrilineChart,
	};

	// 对象基类
	struct ObjectBase {
		using FlatShimCall = void(__stdcall*)(mqmem, ObjectBase const*) noexcept;
		using FlatShimFreeCall = void(__stdcall*)(mqmem) noexcept;

		Type mType; // 对象类型
		mqui32 mShimSize; // 对象大小
		FlatShimCall mShimCall; // 实现回调
		FlatShimFreeCall mShimFreeCall; // 实现释放回调
	};

	// 对象
	template<Type type, typename ObjectType, bool hasFreeCall = false>
	struct Object : ObjectBase {
		Object() noexcept : ObjectBase{ type,
			static_cast<mqui32>(sizeof(typename ObjectType::Shim)),
			reinterpret_cast<FlatShimCall>(&ObjectType::ShimCall),
			nullptr } {
			if constexpr (hasFreeCall) {
				mShimFreeCall = reinterpret_cast<FlatShimFreeCall>(&ObjectType::ShimFreeCall);
			}
		}
	};

	// 文本格式
	export struct TextFormat {
		mqui32 mSize; // 字体大小
		Media::Color mColor; // 字体颜色
		std::wstring mFont; // 字体名称

		TextFormat(mqui32 size = 16U, Media::Color color = Media::Colors::Black, std::wstring_view font = L"黑体") noexcept
			: mSize{ size }, mColor{ color }, mFont{ font } { }
	};

	// 画笔格式
	export struct PenFormat {
		Media::Color mColor; // 画笔颜色
		mqui32 mSize; // 画笔大小

		PenFormat(Media::Color color = Media::Colors::Black, mqui32 size = 1U) noexcept
			: mColor{ color }, mSize{ size } { }
	};

	struct TextFormatShim {
		mqui32 mSize;
		Media::Color mColor;
		mqcstr mFont;

		void Set(TextFormat const& text) noexcept {
			mSize = text.mSize;
			mColor = text.mColor;
			mFont = text.mFont.data();
		}
	};

	struct PenFormatShim {
		Media::Color mColor;
		mqui32 mSize;

		void Set(PenFormat const& pen) noexcept {
			mColor = pen.mColor;
			mSize = pen.mSize;
		}
	};
}

export namespace MasterQian::Media::Graph {
	// 线条
	struct Line : Object<Type::Line, Line> {
		using Shim = PenFormatShim;
		static void __stdcall ShimCall(Shim* shim, Line const* obj) noexcept {
			shim->Set(obj->mFormat);
		}
		PenFormat mFormat; // 画笔格式
		Line(PenFormat format = { }) noexcept : mFormat{ format } { }
	};

	// 矩形
	struct Rectangle : Object<Type::Rectangle, Rectangle> {
		using Shim = PenFormatShim;
		static void __stdcall ShimCall(Shim* shim, Rectangle const* obj) noexcept {
			shim->Set(obj->mFormat);
		}
		PenFormat mFormat; // 画笔格式  [边界宽度(非0为空心)]
		Rectangle(PenFormat format = { Media::Colors::Black, 0U }) noexcept : mFormat{ format } { }
	};

	// 圆
	struct Circle : Object<Type::Circle, Circle> {
		using Shim = PenFormatShim;
		static void __stdcall ShimCall(Shim* shim, Circle const* obj) noexcept {
			shim->Set(obj->mFormat);
		}
		PenFormat mFormat; // 画笔格式  [边界宽度(非0为空心)]
		Circle(PenFormat format = { Media::Colors::Black, 0U }) noexcept : mFormat{ format } { }
	};

	// 文本
	struct Text : Object<Type::Text, Text> {
		struct Shim {
			mqcstr mContent;
			TextFormatShim mFormat;
		};
		static void __stdcall ShimCall(Shim* shim, Text const* obj) noexcept {
			shim->mContent = obj->mContent.data();
			shim->mFormat.Set(obj->mFormat);
		}
		std::wstring mContent; // 内容
		TextFormat mFormat; // 文本格式
		Text(std::wstring_view content, TextFormat const& format) noexcept : mContent{ content }, mFormat{ format } {}
	};

	// 图片
	struct Picture : Object<Type::Picture, Picture> {
		using Shim = mqhandle;
		static void __stdcall ShimCall(Shim* shim, Picture const* obj) noexcept {
			*shim = obj->mImage.unsafe_handle();
		}
		GDI::Image mImage; // 图像
		Picture(GDI::Image const& image) noexcept : mImage{ image } {}
		Picture(GDI::Image&& image) noexcept : mImage{ freestanding::move(image) } {}
	};

	// 三线图
	struct TrilineChart : Object<Type::TrilineChart, TrilineChart, true> {
		struct Shim {
			mqarray<mqcstr> mTitles;
			mqarray<mqarray<mqcstr>> mContents;
			TextFormatShim mTitleFormat;
			TextFormatShim mContentFormat;
		};
		static void __stdcall ShimCall(Shim* shim, TrilineChart const* obj) noexcept {
			auto row{ obj->mContents.size() }, col{ obj->mTitles.size() };
			new(&shim->mTitles)decltype(shim->mTitles)(col);
			for (mqui64 j{ }; j < col; ++j) {
				shim->mTitles[j] = obj->mTitles[j].data();
			}
			new(&shim->mContents)decltype(shim->mContents)(row);
			for (mqui64 i{ }; i < row; ++i) {
				auto& cur_row{ shim->mContents[i] };
				auto& cur_data_row{ obj->mContents[i] };
				cur_row.resize(col);
				for (mqui64 j{ }; j < col; ++j) {
					cur_row[j] = cur_data_row[j].data();
				}
			}
			shim->mTitleFormat.Set(obj->mTitleFormat);
			shim->mContentFormat.Set(obj->mContentFormat);
		}
		static void __stdcall ShimFreeCall(Shim* shim) noexcept {
			shim->mTitles.~decltype(shim->mTitles)();
			shim->mContents.~decltype(shim->mContents)();
		}
		mqarray<std::wstring> mTitles; // 标题
		mqlist<mqarray<std::wstring>> mContents; // 内容
		TextFormat mTitleFormat; // 标题格式
		TextFormat mContentFormat; // 内容格式
		TrilineChart(mqarray<std::wstring> const& titles, mqlist<mqarray<std::wstring>> const& contents,
			TextFormat title_format = { }, TextFormat content_format = { }) noexcept
			: mTitles{ titles }, mContents{ contents }, mTitleFormat{ title_format }, mContentFormat{ content_format } { }
	};

	// 画布
	struct Canvas {
	private:
		mqhandle context{ };
		mqlist<std::pair<mqrect, ObjectBase*>> objects; // 对象集合
	public:
		Canvas(mqsize size, Color color = Colors::Transparency) noexcept {
			context = details::MasterQian_Media_Graph_Canvas_Create(size, color);
		}
		~Canvas() noexcept {
			details::MasterQian_Media_Graph_Canvas_Delete(context);
		}
		Canvas(Canvas const&) noexcept = delete;
		Canvas& operator = (Canvas const&) noexcept = delete;
		Canvas(Canvas&& canvas) noexcept {
			freestanding::swap(context, canvas.context);
			freestanding::swap(objects, canvas.objects);
		}
		Canvas& operator = (Canvas&& canvas) noexcept {
			if (this != &canvas) {
				freestanding::swap(context, canvas.context);
				freestanding::swap(objects, canvas.objects);
			}
			return *this;
		}

		mqrect& operator [] (mqui64 index) noexcept {
			return objects[index].first;
		}

		void operator += (std::pair<mqrect, ObjectBase*> const& item) noexcept {
			objects.add(item);
		}

		Bin Render() const noexcept {
			Bin bin;
			mqui32 size{ };
			auto render{ details::MasterQian_Media_Graph_Canvas_BeginRender(context) };
			mqbuffer buffer(1024U);
			for (auto& [rect, obj] : objects) {
				buffer.reserve(static_cast<mqui64>(obj->mShimSize));
				obj->mShimCall(buffer.data(), obj);
				details::MasterQian_Media_Graph_Canvas_Render(render, &rect, buffer.data(), static_cast<mqenum>(obj->mType));
				if (obj->mShimFreeCall) {
					obj->mShimFreeCall(buffer.data());
				}
			}
			details::MasterQian_Media_Graph_Canvas_EndRender(render);
			if (auto hStream{ details::MasterQian_Media_Graph_Canvas_SaveToStream(context, &size) }) {
				bin.reserve(size);
				details::MasterQian_Media_Graph_Canvas_StreamReadRelease(hStream, bin.data(), size);
			}
			return bin;
		}
	};
}