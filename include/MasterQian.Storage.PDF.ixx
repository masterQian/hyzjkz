module;
#include "MasterQian.Meta.h"
#include <string>
#define MasterQianModuleName(name) MasterQian_Storage_PDF_##name
#define MasterQianModuleNameString(name) "MasterQian_Storage_PDF_"#name
#ifdef _DEBUG
#define MasterQianLibString "MasterQian.Storage.PDF.Debug.dll"
#else
#define MasterQianLibString "MasterQian.Storage.PDF.dll"
#endif
#define MasterQianModuleVersion 20240131ULL
#pragma message("—————————— Please copy [" MasterQianLibString "] into your program folder ——————————")

export module MasterQian.Storage.PDF;
export import MasterQian.Bin;

namespace MasterQian::Storage {
	namespace details {
		META_IMPORT_API(mqhandle, Create, mqui32, mqui32, mqui32, mqui32, mqcstr, mqcstr);
		META_IMPORT_API(void, Delete, mqhandle);
		META_IMPORT_API(mqui32, GetError, mqhandle);
		META_IMPORT_API(void, ResetError, mqhandle);
		META_IMPORT_API(mqui32, SaveToMemory, mqhandle);
		META_IMPORT_API(void, ReadFromMemory, mqhandle, mqbytes, mqui32);
		META_IMPORT_API(void, SaveToFile, mqhandle, mqcstr);
		META_IMPORT_API(mqhandle, CurrentPage, mqhandle);
		META_IMPORT_API(mqhandle, AddPage, mqhandle, mqui32, mqui32);
		META_IMPORT_API(mqhandle, InsertPage, mqhandle, mqhandle, mqui32, mqui32);
		META_IMPORT_API(mqhandle, GetSimSunFont, mqhandle);
		META_IMPORT_API(mqhandle, GetSimHeiFont, mqhandle);
		META_IMPORT_API(mqhandle, LoadFont, mqhandle, mqcstr, mqbool);
		META_IMPORT_API(mqhandle, LoadJPGImageFromFile, mqhandle, mqcstr);
		META_IMPORT_API(mqhandle, LoadPNGImageFromFile, mqhandle, mqcstr);
		META_IMPORT_API(mqhandle, LoadJPGImageFromMemory, mqhandle, mqcbytes, mqui32);
		META_IMPORT_API(mqhandle, LoadPNGImageFromMemory, mqhandle, mqcbytes, mqui32);
		META_IMPORT_API(void, BeginAddText, mqhandle, mqhandle, mqui32);
		META_IMPORT_API(void, EndAddText, mqhandle);
		META_IMPORT_API(void, AddText, mqhandle, mqcstr, mqpoint);
		META_IMPORT_API(mqsize, GetTextSize, mqhandle, mqcstr);
		META_IMPORT_API(void, AddImage, mqhandle, mqhandle, mqpoint, mqsize);
		META_MODULE_BEGIN
			META_PROC_API(Create);
			META_PROC_API(Delete);
			META_PROC_API(GetError);
			META_PROC_API(ResetError);
			META_PROC_API(SaveToMemory);
			META_PROC_API(ReadFromMemory);
			META_PROC_API(SaveToFile);
			META_PROC_API(CurrentPage);
			META_PROC_API(AddPage);
			META_PROC_API(InsertPage);
			META_PROC_API(GetSimSunFont);
			META_PROC_API(GetSimHeiFont);
			META_PROC_API(LoadFont);
			META_PROC_API(LoadJPGImageFromFile);
			META_PROC_API(LoadPNGImageFromFile);
			META_PROC_API(LoadJPGImageFromMemory);
			META_PROC_API(LoadPNGImageFromMemory);
			META_PROC_API(BeginAddText);
			META_PROC_API(EndAddText);
			META_PROC_API(AddText);
			META_PROC_API(GetTextSize);
			META_PROC_API(AddImage);
		META_MODULE_END
	}

	export struct PDF {
	private:
		mqhandle handle{ };
	public:
		// 压缩模式
		enum class CompressMode : mqenum {
			NONE = 0U, TEXT = 1U, IMAGE = 2U, METADATA = 4U, ALL = 15U
		};
		// 页面显示模式
		enum class PageShowMode : mqenum {
			NONE = 0U, OUTLINE = 1U, THUMB = 2U, FULL_SCREEN = 3U
		};
		// 页面布局模式
		enum class PageLayoutMode : mqenum {
			SINGLE = 0U, ONE_COLUMN = 1U, TWO_COLUMN_LEFT = 2U,
			TWO_COLUMN_RIGHT = 3U, TWO_PAGE_LEFT = 4U, TWO_PAGE_RIGHT = 5U,
		};
		// 权限
		enum class Permission : mqenum {
			NONE = static_cast<mqui32>(-1),
			READ = 0U, PRINT = 4U, EDIT = 8U, COPY = 16U, EDIT_COMMENT = 32U, ALL = 60U
		};

		// PDF配置
		struct PDFConfig {
			CompressMode compress_mode{ CompressMode::NONE }; // 压缩模式
			PageShowMode page_show_mode{ PageShowMode::NONE }; // 页面显示模式
			PageLayoutMode page_layout_mode{ PageLayoutMode::SINGLE }; // 页面布局模式
			Permission permission{ Permission::NONE }; // 权限
			std::wstring password_owner; // 所有者密码
			std::wstring password_user; // 用户密码
		};

		// 页面大小
		struct PageSize {
			mqui32 width, height; // 宽高(单位像素, DPI为72)

			[[nodiscard]] PageSize operator ~ () const noexcept { // 调转方向
				return { height, width };
			}
		};
		struct PageSizes {
			static constexpr PageSize LETTER{ 612U, 792U };
			static constexpr PageSize LEGAL{ 612U, 1008U };
			static constexpr PageSize A3{ 842U, 1200U };
			static constexpr PageSize A4{ 595U, 842U };
			static constexpr PageSize A5{ 420U, 595U };
			static constexpr PageSize A6{ 298U, 420U };
			static constexpr PageSize B4{ 709U, 1001U };
			static constexpr PageSize B5{ 499U, 709U };
		};

		// 页面配置
		struct PageConfig {
			PageSize page_size{ PageSizes::A4 };
		};

		// 字体
		struct Font {
			friend struct PDF;
			friend struct Page;
		private:
			mqhandle handle{ };
			Font(mqhandle h) : handle{ h } {}
		public:
			[[nodiscard]] operator bool() const noexcept {
				return handle != nullptr;
			}
		};

		// 图像
		struct Image {
			friend struct PDF;
			friend struct Page;
		private:
			mqhandle handle{ };
			Image(mqhandle h) : handle{ h } {}
		public:
			[[nodiscard]] operator bool() const noexcept {
				return handle != nullptr;
			}
		};

		// 页面
		struct Page {
			friend struct PDF;
		private:
			mqhandle handle{ };
			Page(mqhandle h) : handle{ h } {}
		public:
			[[nodiscard]] operator bool() const noexcept {
				return handle != nullptr;
			}

			void BeginAddText(Font font, mqui32 font_size) const noexcept {
				details::MasterQian_Storage_PDF_BeginAddText(handle, font.handle, font_size);
			}

			void EndAddText() const noexcept {
				details::MasterQian_Storage_PDF_EndAddText(handle);
			}

			void AddText(std::wstring_view content, mqpoint point) const noexcept {
				details::MasterQian_Storage_PDF_AddText(handle, content.data(), point);
			}

			mqsize GetTextSize(std::wstring_view content) const noexcept {
				return details::MasterQian_Storage_PDF_GetTextSize(handle, content.data());
			}

			void AddImage(Image image, mqpoint point, mqsize size = { }) const noexcept {
				return details::MasterQian_Storage_PDF_AddImage(handle, image.handle, point, size);
			}
		};

	public:
		PDF(PDFConfig const& config = { }) noexcept {
			handle = details::MasterQian_Storage_PDF_Create(
				static_cast<mqui32>(config.compress_mode),
				static_cast<mqui32>(config.page_show_mode),
				static_cast<mqui32>(config.page_layout_mode),
				static_cast<mqui32>(config.permission),
				config.password_owner.data(), config.password_user.data());
		}

		PDF(PDF const&) noexcept = delete;
		PDF& operator = (PDF const&) noexcept = delete;

		~PDF() noexcept {
			details::MasterQian_Storage_PDF_Delete(handle);
		}

		[[nodiscard]] operator bool() const noexcept {
			return handle != nullptr;
		}

		mqui32 GetError() const noexcept {
			return details::MasterQian_Storage_PDF_GetError(handle);
		}

		void ResetError() const noexcept {
			return details::MasterQian_Storage_PDF_ResetError(handle);
		}

		[[nodiscard]] Bin Save() const noexcept {
			Bin bin;
			if (mqui32 size{ details::MasterQian_Storage_PDF_SaveToMemory(handle) }) {
				bin.resize(static_cast<mqui64>(size));
				details::MasterQian_Storage_PDF_ReadFromMemory(handle, bin.data(), size);
			}
			return bin;
		}

		void Save(std::wstring_view filename) const noexcept {
			details::MasterQian_Storage_PDF_SaveToFile(handle, filename.data());
		}

		[[nodiscard]] Page CurrentPage() const noexcept {
			return Page{ details::MasterQian_Storage_PDF_CurrentPage(handle) };
		}

		[[nodiscard]] Page AddPage(PageConfig const& config = { }) const noexcept {
			return Page{ details::MasterQian_Storage_PDF_AddPage(handle, config.page_size.width, config.page_size.height) };
		}

		[[nodiscard]] Page InsertPage(Page pos, PageConfig const& config = { }) const noexcept {
			return Page{ details::MasterQian_Storage_PDF_InsertPage(handle, pos.handle, config.page_size.width, config.page_size.height) };
		}

		[[nodiscard]] Font SimSunFont() const noexcept {
			return details::MasterQian_Storage_PDF_GetSimSunFont(handle);
		}

		[[nodiscard]] Font SimHeiFont() const noexcept {
			return details::MasterQian_Storage_PDF_GetSimHeiFont(handle);
		}

		[[nodiscard]] Font LoadFont(std::wstring_view filename, bool embedding = false) const noexcept {
			return details::MasterQian_Storage_PDF_LoadFont(handle, filename.data(), embedding);
		}

		[[nodiscard]] Image LoadJPGImage(std::wstring_view filename) const noexcept {
			return details::MasterQian_Storage_PDF_LoadJPGImageFromFile(handle, filename.data());
		}

		[[nodiscard]] Image LoadJPGImage(BinView bv) const noexcept {
			return details::MasterQian_Storage_PDF_LoadJPGImageFromMemory(handle, bv.data(), bv.size32());
		}

		[[nodiscard]] Image LoadPNGImage(std::wstring_view filename) const noexcept {
			return details::MasterQian_Storage_PDF_LoadPNGImageFromFile(handle, filename.data());
		}

		[[nodiscard]] Image LoadPNGImage(BinView bv) const noexcept {
			return details::MasterQian_Storage_PDF_LoadPNGImageFromMemory(handle, bv.data(), bv.size32());
		}
	};

	export inline constexpr PDF::CompressMode operator | (PDF::CompressMode v1, PDF::CompressMode v2) noexcept {
		return static_cast<PDF::CompressMode>(static_cast<mqenum>(v1) | static_cast<mqenum>(v2));
	}

	export inline constexpr PDF::Permission operator | (PDF::Permission v1, PDF::Permission v2) noexcept {
		return static_cast<PDF::Permission>(static_cast<mqenum>(v1) | static_cast<mqenum>(v2));
	}
}