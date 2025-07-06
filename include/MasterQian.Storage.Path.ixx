module;
#include "MasterQian.Meta.h"
#include <string>
#define MasterQianModuleName(name) MasterQian_Storage_Path_##name
#define MasterQianModuleNameString(name) "MasterQian_Storage_Path_"#name
#ifdef _DEBUG
#define MasterQianLibString "MasterQian.Storage.Path.Debug.dll"
#else
#define MasterQianLibString "MasterQian.Storage.Path.dll"
#endif
#define MasterQianModuleVersion 20240131ULL
#pragma message("—————————— Please copy [" MasterQianLibString "] into your program folder ——————————")

export module MasterQian.Storage.Path;
export import MasterQian.Bin;
export import MasterQian.Time;

namespace MasterQian::Storage {
	namespace details {
		using EnumUserCallback = void(__stdcall*)(mqmem arg, mqcstr path) noexcept;

		META_IMPORT_API(mqui32, Attribute, mqcstr);
		META_IMPORT_API(mqui64, Size, mqcstr);
		META_IMPORT_API(mqui64, GetTime, mqcstr, mqui32);
		META_IMPORT_API(mqbool, SetTime, mqcstr, mqui32, mqui64);
		META_IMPORT_API(void, Read, mqcstr, mqui64, mqbytes);
		META_IMPORT_API(mqbool, Write, mqcstr, mqui64, mqcbytes);
		META_IMPORT_API(mqbool, Copy, mqcstr, mqcstr, mqcstr, mqbool);
		META_IMPORT_API(mqbool, CopyRename, mqcstr, mqcstr, mqbool);
		META_IMPORT_API(mqbool, Move, mqcstr, mqcstr, mqcstr);
		META_IMPORT_API(mqbool, Rename, mqcstr, mqcstr);
		META_IMPORT_API(mqbool, Delete, mqcstr);
		META_IMPORT_API(mqbool, DeleteToRecycleBin, mqcstr);
		META_IMPORT_API(mqbool, EnumFolder, mqcstr, EnumUserCallback, mqmem, mqcstr);
		META_IMPORT_API(mqbool, EnumFolderDepth, mqcstr, EnumUserCallback, mqmem);
		META_IMPORT_API(mqbool, CreateFolder, mqcstr);
		META_IMPORT_API(mqbool, ShortCutTarget, mqcstr, mqstr);
		META_IMPORT_API(mqui32, GetDriversBitmap, mqstr);
		META_IMPORT_API(void, GetDriverSpaceInfo, mqchar, mqstr, mqf64*, mqf64*);
		META_IMPORT_API(mqui64, GetRecycleBinItemCount, mqchar);
		META_IMPORT_API(void, ClearRecycleBin, mqbool);
		META_IMPORT_API(void, GetStandardPath, mqi32, mqstr);
		META_IMPORT_API(void, AutoCurrentFolder, mqcstr);
		META_MODULE_BEGIN
			META_PROC_API(Attribute);
			META_PROC_API(Size);
			META_PROC_API(GetTime);
			META_PROC_API(SetTime);
			META_PROC_API(Read);
			META_PROC_API(Write);
			META_PROC_API(Copy);
			META_PROC_API(CopyRename);
			META_PROC_API(Move);
			META_PROC_API(Rename);
			META_PROC_API(Delete);
			META_PROC_API(DeleteToRecycleBin);
			META_PROC_API(EnumFolder);
			META_PROC_API(EnumFolderDepth);
			META_PROC_API(CreateFolder);
			META_PROC_API(ShortCutTarget);
			META_PROC_API(GetDriversBitmap);
			META_PROC_API(GetDriverSpaceInfo);
			META_PROC_API(GetRecycleBinItemCount);
			META_PROC_API(ClearRecycleBin);
			META_PROC_API(GetStandardPath);
			META_PROC_API(AutoCurrentFolder);
		META_MODULE_END
	}

	// 路径
	export struct Path {
	private:
		static constexpr auto PATH_SLASH{ L'\\' };
		static constexpr auto PATH_MAX_LENGTH{ 259ULL };
		static constexpr auto PATH_MAX{ PATH_MAX_LENGTH + 1ULL };

		std::wstring mData;
	public:
		// 路径属性
		struct Attributes {
		private:
			mqui32 attr;

			static constexpr auto NON_EXIST{ static_cast<mqui32>(-1) };
			static constexpr auto READ_ONLY{ 1U };
			static constexpr auto HIDDEN{ 2U };
			static constexpr auto SYSTEM{ 4U };
			static constexpr auto FOLDER{ 16U };
		public:
			Attributes(mqui32 value = NON_EXIST) : attr{ value } {}

			// 是否存在
			[[nodiscard]] bool Exists() const noexcept {
				return attr != NON_EXIST;
			}

			// 是否只读
			[[nodiscard]] bool ReadOnly() const noexcept {
				return Exists() && (attr & READ_ONLY) != 0U;
			}

			// 是否隐藏
			[[nodiscard]] bool Hidden() const noexcept {
				return Exists() && (attr & HIDDEN) != 0U;
			}

			// 是否所属系统
			[[nodiscard]] bool System() const noexcept {
				return Exists() && (attr & SYSTEM) != 0U;
			}

			// 是否是目录
			[[nodiscard]] bool Folder() const noexcept {
				return Exists() && (attr & FOLDER) != 0U;
			}

			// 是否是文件
			[[nodiscard]] bool File() const noexcept {
				return Exists() && !Folder();
			}

			[[nodiscard]] operator mqui32 () const noexcept {
				return attr;
			}
		};
	private:
		void RemoveBackClash() noexcept {
			if (!mData.empty() && mData.back() == PATH_SLASH) {
				mData.pop_back();
			}
		}
	public:
		Path() noexcept = default;

		/// <summary>
		/// 从字符串构造
		/// </summary>
		/// <param name="path">路径</param>
		Path(std::wstring_view path) noexcept {
			auto copy_size{ path.size() };
			if (copy_size > PATH_MAX_LENGTH) copy_size = PATH_MAX_LENGTH;
			mData.assign(path.data(), copy_size);
			RemoveBackClash();
		}

		/// <summary>
		/// 从字符串构造
		/// </summary>
		/// <param name="path">路径</param>
		Path(mqcstr path) noexcept : Path(std::wstring_view(path)) {}

		/// <summary>
		/// 转字符串
		/// </summary>
		[[nodiscard]] std::wstring ToString() const noexcept {
			return mData;
		}

		[[nodiscard]] operator std::wstring_view() const noexcept {
			return mData;
		}

		template<typename T>
		Path operator + (T) const noexcept = delete;

		template<typename T>
		Path& operator += (T) const noexcept = delete;

		[[nodiscard]] Path operator / (std::wstring_view name) const noexcept {
			Path tmp{ mData };
			return tmp /= name;
		}

		/// <summary>
		/// 附加子级，等同于/运算符
		/// </summary>
		/// <param name="name">子级名，自动添加分隔符，无需包含分隔符</param>
		/// <returns>新路径</returns>
		[[nodiscard]] Path Concat(std::wstring_view name) const noexcept {
			return *this / name;
		}

		Path& operator /= (std::wstring_view name) noexcept {
			if (mData.size() < PATH_MAX_LENGTH) {
				if (!mData.empty()) {
					mData.push_back(PATH_SLASH);
				}
				auto cur_size{ mData.size() }, append_size{ name.size() };
				if (append_size + cur_size > PATH_MAX_LENGTH) {
					append_size = PATH_MAX_LENGTH - cur_size;
				}
				mData.append(name.data(), append_size);
				RemoveBackClash();
			}
			return *this;
		}

		/// <summary>
		/// 附加子级到自身，等同于/=运算符
		/// </summary>
		/// <param name="name">子级名，自动添加分隔符，无需包含分隔符</param>
		Path& Append(std::wstring_view name) noexcept {
			return *this /= name;
		}

		/// <summary>
		/// 是否是绝对路径
		/// </summary>
		[[nodiscard]] bool IsAbsolutePath() const noexcept {
			return mData.size() >= 2ULL && mData[1] == L':';
		}

		/// <summary>
		/// 是否是相对路径
		/// </summary>
		[[nodiscard]] bool IsRelativePath() const noexcept {
			return !IsAbsolutePath();
		}

		/// <summary>
		/// 取驱动器路径
		/// </summary>
		/// <returns>形如C:这种包含盘符的驱动器根路径</returns>
		[[nodiscard]] Path Driver() const noexcept {
			if (IsAbsolutePath()) {
				return Path{ mData.substr(0ULL, 2ULL) };
			}
			return { };
		}

		/// <summary>
		/// 是否有父路径
		/// </summary>
		[[nodiscard]] bool HasParent() const noexcept {
			return mData.rfind(PATH_SLASH) != std::wstring::npos;
		}

		/// <summary>
		/// 取父路径
		/// </summary>
		[[nodiscard]] Path Parent() const noexcept {
			if (auto slash_pos{ mData.rfind(PATH_SLASH) }; slash_pos != std::wstring::npos) {
				return Path{ mData.substr(0ULL, slash_pos) };
			}
			return { };
		}

		/// <summary>
		/// 取文件名
		/// </summary>
		[[nodiscard]] std::wstring_view Name() const noexcept {
			std::wstring_view data_view{ mData };
			if (auto slash_pos{ mData.rfind(PATH_SLASH) }; slash_pos != std::wstring::npos) {
				data_view = data_view.substr(slash_pos + 1ULL);
			}
			return data_view;
		}

		/// <summary>
		/// 取不带拓展名的文件名
		/// </summary>
		[[nodiscard]] std::wstring_view NameWithoutExt() const noexcept {
			std::wstring_view name_view{ Name() };
			if (auto dot_pos{ name_view.rfind(L'.') }; dot_pos != std::wstring_view::npos) {
				name_view = name_view.substr(0ULL, dot_pos);
			}
			return name_view;
		}

		/// <summary>
		/// 取拓展名
		/// </summary>
		[[nodiscard]] std::wstring_view Extension() const noexcept {
			std::wstring_view name_view{ Name() };
			if (auto dot_pos{ name_view.rfind(L'.') }; dot_pos != std::wstring_view::npos) {
				return name_view.substr(dot_pos);
			}
			return { };
		}

		/// <summary>
		/// 取不带点的拓展名
		/// </summary>
		[[nodiscard]] std::wstring_view ExtensionWithoutDot() const noexcept {
			std::wstring_view ext_view{ Extension() };
			return ext_view.empty() ? ext_view : ext_view.substr(1ULL);
		}

		/// <summary>
		/// 取属性
		/// </summary>
		[[nodiscard]] Attributes Attribute() const noexcept {
			return details::MasterQian_Storage_Path_Attribute(mData.data());
		}

		/// <summary>
		/// 是否存在
		/// </summary>
		[[nodiscard]] bool Exists() const noexcept {
			return Attribute().Exists();
		}

		/// <summary>
		/// 是否是文件
		/// </summary>
		[[nodiscard]] bool IsFile() const noexcept {
			return Attribute().File();
		}

		/// <summary>
		/// 是否是目录
		/// </summary>
		[[nodiscard]] bool IsFolder() const noexcept {
			return Attribute().Folder();
		}

		/// <summary>
		/// 大小，支持文件或目录调用
		/// </summary>
		/// <returns>64位长度的大小</returns>
		[[nodiscard]] mqui64 Size() const noexcept {
			return details::MasterQian_Storage_Path_Size(mData.data());
		}

		/// <summary>
		/// 取创建时间，支持文件或目录的调用
		/// </summary>
		[[nodiscard]] Timestamp CreateTime() const noexcept {
			return details::MasterQian_Storage_Path_GetTime(mData.data(), 0U);
		}

		/// <summary>
		/// 置创建时间，支持文件或目录的调用
		/// </summary>
		/// <param name="ts">时间戳</param>
		/// <returns>是否成功</returns>
		bool CreateTime(Timestamp ts) const noexcept {
			return details::MasterQian_Storage_Path_SetTime(mData.data(), 0U, ts);
		}

		/// <summary>
		/// 取修改时间，支持文件或目录的调用
		/// </summary>
		[[nodiscard]] Timestamp WriteTime() const noexcept {
			return details::MasterQian_Storage_Path_GetTime(mData.data(), 1U);
		}

		/// <summary>
		/// 置修改时间，支持文件或目录的调用
		/// </summary>
		/// <param name="ts">时间戳</param>
		/// <returns>是否成功</returns>
		bool WriteTime(Timestamp ts) const noexcept {
			return details::MasterQian_Storage_Path_SetTime(mData.data(), 1U, ts);
		}

		/// <summary>
		/// 取访问时间，支持文件或目录的调用
		/// </summary>
		[[nodiscard]] Timestamp AccessTime() const noexcept {
			return details::MasterQian_Storage_Path_GetTime(mData.data(), 2U);
		}

		/// <summary>
		/// 置访问时间，支持文件或目录的调用
		/// </summary>
		/// <param name="ts">时间戳</param>
		/// <returns>是否成功</returns>
		bool AccessTime(Timestamp ts) const noexcept {
			return details::MasterQian_Storage_Path_SetTime(mData.data(), 2U, ts);
		}

		/// <summary>
		/// 读文件，仅支持文件的调用，可超过4G
		/// </summary>
		/// <returns>文件字节集</returns>
		[[nodiscard]] Bin Read() const noexcept {
			Bin tmp(details::MasterQian_Storage_Path_Size(mData.data()));
			details::MasterQian_Storage_Path_Read(mData.data(), tmp.size(), tmp.data());
			return tmp;
		}

		/// <summary>
		/// 写文件，仅支持文件的调用
		/// </summary>
		/// <param name="bv">字节集，可超过4G</param>
		/// <returns>是否成功</returns>
		bool Write(BinView bv) const noexcept {
			return details::MasterQian_Storage_Path_Write(mData.data(), bv.size(), bv.data());
		}

		/// <summary>
		/// 复制，支持文件或目录的调用，将路径以自身同名复制到目标路径内，若需修改名称使用CopyRename
		/// </summary>
		/// <param name="folder_path">目标路径，仅支持目录</param>
		/// <param name="overridable">允许覆盖</param>
		/// <returns>是否成功</returns>
		bool Copy(Path const& folder_path, bool overridable = true) const noexcept {
			return details::MasterQian_Storage_Path_Copy(Parent().mData.data(), Name().data(), folder_path.mData.data(), overridable);
		}

		/// <summary>
		/// 复制，支持文件或目录的调用，复制并修改其文件名
		/// </summary>
		/// <param name="des">目标路径</param>
		/// <param name="overridable">允许覆盖</param>
		/// <returns>是否成功</returns>
		bool CopyRename(Path const& des, bool overridable = true) const noexcept {
			return details::MasterQian_Storage_Path_CopyRename(mData.data(), des.mData.data(), overridable);
		}

		/// <summary>
		/// 移动，支持文件或目录的调用
		/// </summary>
		/// <param name="folder_path">目标路径，仅支持目录</param>
		/// <returns>是否成功</returns>
		bool Move(Path const& folder_path) const noexcept {
			return details::MasterQian_Storage_Path_Move(Parent().mData.data(), Name().data(), folder_path.mData.data());
		}

		/// <summary>
		/// 重命名，支持文件或目录的调用
		/// </summary>
		/// <param name="name">新名称，会自动补充父路径与拓展名，无需包含父路径与拓展名</param>
		/// <returns>是否成功</returns>
		bool Rename(std::wstring_view name) const noexcept {
			Path new_path{ Parent() };
			std::wstring new_name{ name };
			new_name += Extension();
			new_path /= new_name;
			return details::MasterQian_Storage_Path_Rename(mData.data(), new_path.mData.data());
		}

		/// <summary>
		/// 删除，支持文件或目录的调用
		/// </summary>
		/// <returns>是否成功</returns>
		bool Delete() const noexcept {
			return details::MasterQian_Storage_Path_Delete(mData.data());
		}

		/// <summary>
		/// 删除到回收站，支持文件或目录的调用
		/// </summary>
		/// <returns>是否成功</returns>
		bool DeleteToRecycleBin() const noexcept {
			return details::MasterQian_Storage_Path_DeleteToRecycleBin(mData.data());
		}

		/// <summary>
		/// 枚举目录，仅支持目录的调用，仅枚举目录内的子路径
		/// </summary>
		/// <param name="ext">拓展名，如*.jpg表示仅枚举jpg格式的文件，*表示匹配所有文件及目录</param>
		/// <returns>子路径集合</returns>
		[[nodiscard]] mqlist<Path> EnumFolder(std::wstring_view ext = L"*") const noexcept {
			mqlist<Path> subPaths;
			if (!details::MasterQian_Storage_Path_EnumFolder(mData.data(), [ ] (mqmem arg, mqcstr path) noexcept {
				static_cast<mqlist<Path>*>(arg)->add(path);
				}, &subPaths, ext.data())) {
				subPaths.clear();
			}
			return subPaths;
		}

		/// <summary>
		/// 深度枚举目录，仅支持目录的调用，递归枚举目录及所有子目录中所有文件与目录
		/// </summary>
		/// <returns>子路径集合</returns>
		[[nodiscard]] mqlist<Path> EnumFolderDepth() const noexcept {
			mqlist<Path> subPaths;
			if (!details::MasterQian_Storage_Path_EnumFolderDepth(mData.data(), [ ] (mqmem arg, mqcstr path) noexcept {
				static_cast<mqlist<Path>*>(arg)->add(path);
				}, &subPaths)) {
				subPaths.clear();
			}
			return subPaths;
		}

		/// <summary>
		/// 确保文件存在，仅支持文件的调用，若文件不存在则使用默认值创建，否则忽略
		/// </summary>
		/// <param name="bv">默认值</param>
		void Create(BinView bv) const noexcept {
			if (!Exists()) {
				Write(bv);
			}
		}

		/// <summary>
		/// 确保多级目录存在，仅支持目录的调用，若目录不存在则创建，否则忽略，允许中途多级，将沿着路径顺序依次创建
		/// </summary>
		void Create() const noexcept {
			details::MasterQian_Storage_Path_CreateFolder(mData.data());
		}

		/// <summary>
		/// 取快捷方式目标，仅支持文件的调用
		/// </summary>
		/// <returns>lnk文件对应的目标</returns>
		[[nodiscard]] Path ShortCutTarget() const noexcept {
			mqchar buf[PATH_MAX]{ };
			details::MasterQian_Storage_Path_ShortCutTarget(mData.data(), buf);
			return std::wstring_view{ buf };
		}


		/*  静态函数  */

		/// <summary>
		/// 取桌面路径
		/// </summary>
		[[nodiscard]] static Path Desktop() noexcept {
			mqchar buf[PATH_MAX]{ };
			details::MasterQian_Storage_Path_GetStandardPath(0x0000, buf);
			return std::wstring_view{ buf };
		}

		/// <summary>
		/// 取文档路径
		/// </summary>
		[[nodiscard]] static Path Documents() noexcept {
			mqchar buf[PATH_MAX]{ };
			details::MasterQian_Storage_Path_GetStandardPath(0x0005, buf);
			return std::wstring_view{ buf };
		}

		/// <summary>
		/// 取音乐路径
		/// </summary>
		[[nodiscard]] static Path Music() noexcept {
			mqchar buf[PATH_MAX]{ };
			details::MasterQian_Storage_Path_GetStandardPath(0x000D, buf);
			return std::wstring_view{ buf };
		}

		/// <summary>
		/// 取视频路径
		/// </summary>
		[[nodiscard]] static Path Video() noexcept {
			mqchar buf[PATH_MAX]{ };
			details::MasterQian_Storage_Path_GetStandardPath(0x000E, buf);
			return std::wstring_view{ buf };
		}

		/// <summary>
		/// 取字体路径
		/// </summary>
		[[nodiscard]] static Path Fonts() noexcept {
			mqchar buf[PATH_MAX]{ };
			details::MasterQian_Storage_Path_GetStandardPath(0x0014, buf);
			return std::wstring_view{ buf };
		}

		/// <summary>
		/// 取AppData路径
		/// </summary>
		[[nodiscard]] static Path AppData() noexcept {
			mqchar buf[PATH_MAX]{ };
			details::MasterQian_Storage_Path_GetStandardPath(0x001A, buf);
			return std::wstring_view{ buf };
		}

		/// <summary>
		/// 取Windows路径
		/// </summary>
		[[nodiscard]] static Path Windows() noexcept {
			mqchar buf[PATH_MAX]{ };
			details::MasterQian_Storage_Path_GetStandardPath(0x0024, buf);
			return std::wstring_view{ buf };
		}

		/// <summary>
		/// 取系统路径
		/// </summary>
		[[nodiscard]] static Path System() noexcept {
			mqchar buf[PATH_MAX]{ };
			details::MasterQian_Storage_Path_GetStandardPath(0x0025, buf);
			return std::wstring_view{ buf };
		}

		/// <summary>
		/// 取ProgramFiles路径
		/// </summary>
		[[nodiscard]] static Path ProgramFiles() noexcept {
			mqchar buf[PATH_MAX]{ };
			details::MasterQian_Storage_Path_GetStandardPath(0x0026, buf);
			return std::wstring_view{ buf };
		}

		/// <summary>
		/// 取照片路径
		/// </summary>
		[[nodiscard]] static Path Picture() noexcept {
			mqchar buf[PATH_MAX]{ };
			details::MasterQian_Storage_Path_GetStandardPath(0x0027, buf);
			return std::wstring_view{ buf };
		}

		/// <summary>
		/// 取当前运行的程序的路径
		/// </summary>
		[[nodiscard]] static Path Running() noexcept {
			mqchar buf[PATH_MAX]{ };
			details::MasterQian_Storage_Path_GetStandardPath(5201314, buf);
			return std::wstring_view{ buf };
		}

		/// <summary>
		/// 取当前运行程序的文件名
		/// </summary>
		[[nodiscard]] static Path RunningName() noexcept {
			mqchar buf[PATH_MAX]{ };
			details::MasterQian_Storage_Path_GetStandardPath(5201315, buf);
			return std::wstring_view{ buf };
		}

		/// <summary>
		/// 取Temp路径
		/// </summary>
		[[nodiscard]] static Path Temp() noexcept {
			mqchar buf[PATH_MAX]{ };
			details::MasterQian_Storage_Path_GetStandardPath(5201316, buf);
			return std::wstring_view{ buf };
		}

		/// <summary>
		/// 自适应当前目录，将运行的程序当前目录修改为程序所在的目录，而无视父进程的调用
		/// </summary>
		static void AutoCurrentFolder() noexcept {
			details::MasterQian_Storage_Path_AutoCurrentFolder(Running().mData.data());
		}
	};

	// 驱动器
	export struct Driver {
		Path name; // 名称
		mqf64 totalSpace{ }; // 总空间(GB)
		mqf64 freeSpace{ }; // 剩余空间(GB)
	};

	export [[nodiscard]] inline mqarray<Driver> Drivers() noexcept {
		/// <summary>
		/// 取驱动器组
		/// </summary>
		/// <returns>驱动器集合</returns>
		mqchar names[27]{ }, name[3]{ };
		mqui32 count{ details::MasterQian_Storage_Path_GetDriversBitmap(names) };
		mqarray<Driver> drivers(static_cast<mqui64>(count));
		for (mqui32 i{ }; i < count; ++i) {
			auto& driver{ drivers[i] };
			details::MasterQian_Storage_Path_GetDriverSpaceInfo(names[i], name, &driver.totalSpace, &driver.freeSpace);
			driver.name = std::wstring_view{ name };
		}
		return drivers;
	}

	// 回收站操作对象
	export namespace RecycleBin {
		/// <summary>
		/// 回收站是否为空
		/// </summary>
		[[nodiscard]] inline bool Empty() noexcept {
			mqchar names[27]{ };
			mqui32 count{ details::MasterQian_Storage_Path_GetDriversBitmap(names) };
			for (mqui32 i{ }; i < count; ++i) {
				if (details::MasterQian_Storage_Path_GetRecycleBinItemCount(names[i])) {
					return false;
				}
			}
			return true;
		}

		/// <summary>
		/// 清空回收站
		/// </summary>
		/// <param name="tip">是否弹出提示窗口</param>
		inline void Clear(bool tip = false) noexcept {
			details::MasterQian_Storage_Path_ClearRecycleBin(tip);
		}
	};
}

export [[nodiscard]] inline MasterQian::Storage::Path operator ""_path(mqcstr str, mqui64 size) noexcept {
	return MasterQian::Storage::Path{ std::wstring_view{ str, size } };
}