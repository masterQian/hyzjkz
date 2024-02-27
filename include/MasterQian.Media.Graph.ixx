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
#pragma message("�������������������� Please copy [" MasterQianLibString "] into your program folder ��������������������")

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

	// �������
	struct ObjectBase {
		using FlatShimCall = void(__stdcall*)(mqmem, ObjectBase const*) noexcept;
		using FlatShimFreeCall = void(__stdcall*)(mqmem) noexcept;

		Type mType; // ��������
		mqui32 mShimSize; // �����С
		FlatShimCall mShimCall; // ʵ�ֻص�
		FlatShimFreeCall mShimFreeCall; // ʵ���ͷŻص�
	};

	// ����
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

	// �ı���ʽ
	export struct TextFormat {
		mqui32 mSize; // �����С
		Media::Color mColor; // ������ɫ
		std::wstring mFont; // ��������

		TextFormat(mqui32 size = 16U, Media::Color color = Media::Colors::Black, std::wstring_view font = L"����") noexcept
			: mSize{ size }, mColor{ color }, mFont{ font } { }
	};

	// ���ʸ�ʽ
	export struct PenFormat {
		Media::Color mColor; // ������ɫ
		mqui32 mSize; // ���ʴ�С

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
	// ����
	struct Line : Object<Type::Line, Line> {
		using Shim = PenFormatShim;
		static void __stdcall ShimCall(Shim* shim, Line const* obj) noexcept {
			shim->Set(obj->mFormat);
		}
		PenFormat mFormat; // ���ʸ�ʽ
		Line(PenFormat format = { }) noexcept : mFormat{ format } { }
	};

	// ����
	struct Rectangle : Object<Type::Rectangle, Rectangle> {
		using Shim = PenFormatShim;
		static void __stdcall ShimCall(Shim* shim, Rectangle const* obj) noexcept {
			shim->Set(obj->mFormat);
		}
		PenFormat mFormat; // ���ʸ�ʽ  [�߽���(��0Ϊ����)]
		Rectangle(PenFormat format = { Media::Colors::Black, 0U }) noexcept : mFormat{ format } { }
	};

	// Բ
	struct Circle : Object<Type::Circle, Circle> {
		using Shim = PenFormatShim;
		static void __stdcall ShimCall(Shim* shim, Circle const* obj) noexcept {
			shim->Set(obj->mFormat);
		}
		PenFormat mFormat; // ���ʸ�ʽ  [�߽���(��0Ϊ����)]
		Circle(PenFormat format = { Media::Colors::Black, 0U }) noexcept : mFormat{ format } { }
	};

	// �ı�
	struct Text : Object<Type::Text, Text> {
		struct Shim {
			mqcstr mContent;
			TextFormatShim mFormat;
		};
		static void __stdcall ShimCall(Shim* shim, Text const* obj) noexcept {
			shim->mContent = obj->mContent.data();
			shim->mFormat.Set(obj->mFormat);
		}
		std::wstring mContent; // ����
		TextFormat mFormat; // �ı���ʽ
		Text(std::wstring_view content, TextFormat const& format) noexcept : mContent{ content }, mFormat{ format } {}
	};

	// ͼƬ
	struct Picture : Object<Type::Picture, Picture> {
		using Shim = mqhandle;
		static void __stdcall ShimCall(Shim* shim, Picture const* obj) noexcept {
			*shim = obj->mImage.unsafe_handle();
		}
		GDI::Image mImage; // ͼ��
		Picture(GDI::Image const& image) noexcept : mImage{ image } {}
		Picture(GDI::Image&& image) noexcept : mImage{ freestanding::move(image) } {}
	};

	// ����ͼ
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
		mqarray<std::wstring> mTitles; // ����
		mqlist<mqarray<std::wstring>> mContents; // ����
		TextFormat mTitleFormat; // �����ʽ
		TextFormat mContentFormat; // ���ݸ�ʽ
		TrilineChart(mqarray<std::wstring> const& titles, mqlist<mqarray<std::wstring>> const& contents,
			TextFormat title_format = { }, TextFormat content_format = { }) noexcept
			: mTitles{ titles }, mContents{ contents }, mTitleFormat{ title_format }, mContentFormat{ content_format } { }
	};

	// ����
	struct Canvas {
	private:
		mqhandle context{ };
		mqlist<std::pair<mqrect, ObjectBase*>> objects; // ���󼯺�
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