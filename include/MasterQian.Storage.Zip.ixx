module;
#include "MasterQian.Meta.h"
#include <string>
#define MasterQianModuleName(name) MasterQian_Storage_Zip_##name
#define MasterQianModuleNameString(name) "MasterQian_Storage_Zip_"#name
#ifdef _DEBUG
#define MasterQianLibString "MasterQian.Storage.Zip.Debug.dll"
#else
#define MasterQianLibString "MasterQian.Storage.Zip.dll"
#endif
#define MasterQianModuleVersion 20240131ULL
#pragma message("—————————— Please copy [" MasterQianLibString "] into your program folder ——————————")

export module MasterQian.Storage.Zip;
export import MasterQian.Bin;

namespace MasterQian::Storage {
	/// <summary>
	/// 压缩回调
	/// </summary>
	/// <param name="arg">可携带参数</param>
	/// <param name="result">解压中途是否正常</param>
	/// <param name="new_name">压缩包中文件名</param>
	/// <param name="old_name">源文件名</param>
	export using ZipCallBack = void(__stdcall*)(mqmem, bool, mqcstr, mqcstr);

	/// <summary>
	/// 解压回调
	/// </summary>
	/// <param name="arg">可携带参数</param>
	/// <param name="new_name">压缩包中文件名</param>
	/// <param name="old_name">源文件名</param>
	export using UnZipCallBack = void(__stdcall*)(mqmem, mqcstr, mqcstr);

	namespace details {
		META_IMPORT_API(mqui32, CompressBound, mqui32);
		META_IMPORT_API(mqui32, Compress, mqcbytes, mqui32, mqbytes, mqui32, mqbool);
		META_IMPORT_API(mqui32, UncompressBound, mqcbytes, mqui32);
		META_IMPORT_API(mqbool, Uncompress, mqcbytes, mqui32, mqbytes, mqui32);

		META_IMPORT_API(mqhandle, ZipStart, mqcstr, mqui32);
		META_IMPORT_API(void, ZipEnd, mqhandle);
		META_IMPORT_API(mqbool, ZipIsFile, mqcstr);
		META_IMPORT_API(mqbool, ZipIsFolder, mqcstr);
		META_IMPORT_API(mqbool, ZipAddBin, mqhandle, mqcstr, mqcbytes, mqui32);
		META_IMPORT_API(mqbool, ZipAddFile, mqhandle, mqcstr, mqcstr);
		META_IMPORT_API(mqbool, ZipAddFolder, mqhandle, mqcstr, mqcstr);
		META_IMPORT_API(mqbool, ZipAddFolderWithCallback, mqhandle, mqcstr, mqcstr, ZipCallBack, mqmem);

		META_IMPORT_API(mqhandle, UnZipStart, mqcstr);
		META_IMPORT_API(void, UnZipEnd, mqhandle);
		META_IMPORT_API(mqui64, UnZipCount, mqhandle);
		META_IMPORT_API(mqbool, UnZipSave, mqhandle, mqcstr);
		META_IMPORT_API(mqbool, UnZipSaveWithCallback, mqhandle, mqcstr, UnZipCallBack, mqmem);
		META_MODULE_BEGIN
			META_PROC_API(CompressBound);
			META_PROC_API(Compress);
			META_PROC_API(UncompressBound);
			META_PROC_API(Uncompress);

			META_PROC_API(ZipStart);
			META_PROC_API(ZipEnd);
			META_PROC_API(ZipIsFile);
			META_PROC_API(ZipIsFolder);
			META_PROC_API(ZipAddBin);
			META_PROC_API(ZipAddFile);
			META_PROC_API(ZipAddFolder);
			META_PROC_API(ZipAddFolderWithCallback);
			META_PROC_API(UnZipStart);
			META_PROC_API(UnZipEnd);
			META_PROC_API(UnZipCount);
			META_PROC_API(UnZipSave);
			META_PROC_API(UnZipSaveWithCallback);
		META_MODULE_END
	}

	// 压缩分支
	export struct ZipBranch {
	protected:
		mqhandle handle{ };
		std::wstring branch;
		ZipBranch(mqhandle h, std::wstring_view sv) noexcept : handle{ h }, branch{ sv } {}
	public:
		[[nodiscard]] ZipBranch operator [] (std::wstring_view subName) const noexcept {
			ZipBranch tmp{ *this };
			tmp.branch += subName;
			tmp.branch.push_back(L'\\');
			return tmp;
		}

		bool operator += (std::wstring_view fn) const noexcept {
			if (handle) {
				std::wstring fullname{ branch };
				fullname += fn.substr(fn.rfind(L'\\') + 1U);
				if (details::MasterQian_Storage_Zip_ZipIsFolder(fn.data())) {
					fullname.push_back(L'\\');
					return details::MasterQian_Storage_Zip_ZipAddFolder(handle, fullname.data(), fn.data());
				}
				else {
					return details::MasterQian_Storage_Zip_ZipAddFile(handle, fullname.data(), fn.data());
				}
			}
			return false;
		}

		/// <summary>
		/// 添加文件
		/// </summary>
		/// <param name="name">名称</param>
		/// <param name="fn">文件路径</param>
		/// <returns>是否成功</returns>
		bool AddFile(std::wstring_view name, std::wstring_view fn) const noexcept {
			if (handle) {
				if (details::MasterQian_Storage_Zip_ZipIsFile(fn.data())) {
					std::wstring fullname{ branch };
					fullname += name;
					return details::MasterQian_Storage_Zip_ZipAddFile(handle, fullname.data(), fn.data());
				}
			}
			return false;
		}

		/// <summary>
		/// 添加字节集
		/// </summary>
		/// <param name="name">名称</param>
		/// <param name="bv">字节集</param>
		/// <returns>是否成功</returns>
		bool AddBin(std::wstring_view name, BinView bv) const noexcept {
			if (handle) {
				std::wstring fullname{ branch };
				fullname += name;
				return details::MasterQian_Storage_Zip_ZipAddBin(handle, fullname.data(), bv.data(), bv.size32());
			}
			return false;
		}

		/// <summary>
		/// 添加文件夹
		/// </summary>
		/// <param name="fn">名称</param>
		/// <param name="createSelf">若为true则将文件夹自身作为根压缩进去，否则只将内部文件压缩进去</param>
		/// <param name="callback">压缩回调</param>
		/// <param name="arg">可携带参数</param>
		/// <returns></returns>
		bool AddFolder(std::wstring_view fn, bool createSelf = true, ZipCallBack callback = nullptr, mqmem arg = nullptr) const noexcept {
			if (details::MasterQian_Storage_Zip_ZipIsFolder(fn.data())) {
				if (createSelf) {
					std::wstring fullname{ branch };
					fullname += fn.substr(fn.rfind(L'\\') + 1U);
					fullname.push_back(L'\\');
					if (callback) {
						return details::MasterQian_Storage_Zip_ZipAddFolderWithCallback(handle,
							fullname.data(), fn.data(), callback, arg);
					}
					return details::MasterQian_Storage_Zip_ZipAddFolder(handle, fullname.data(), fn.data());
				}
				else {
					if (callback) {
						return details::MasterQian_Storage_Zip_ZipAddFolderWithCallback(handle,
							branch.data(), fn.data(), callback, arg);
					}
					return details::MasterQian_Storage_Zip_ZipAddFolder(handle, branch.data(), fn.data());
				}
			}
			return false;
		}
	};

	// Zip压缩
	export struct Zip : public ZipBranch {
	public:
		// 打开模式
		enum class OpenMode : mqenum {
			CREATE, // 不存在创建, 存在失败
			CREATE_OPEN, // 不存在创建, 存在打开
			CREATE_OVERRIDE, // 不存在创建, 存在覆盖
			OPEN_EXIST, // 不存在失败, 存在打开
		};

		/// <summary>
		/// 打开
		/// </summary>
		/// <param name="fn">文件名</param>
		/// <param name="mode">打开模式</param>
		Zip(std::wstring_view fn, OpenMode mode) noexcept :
			ZipBranch(details::MasterQian_Storage_Zip_ZipStart(fn.data(), static_cast<mqui32>(mode)), L"") {}

		Zip(Zip const&) noexcept = delete;
		Zip& operator = (Zip const&) noexcept = delete;

		Zip(Zip&& zip) noexcept : ZipBranch{ nullptr, { } } {
			freestanding::swap(handle, zip.handle);
			freestanding::swap(branch, zip.branch);
		}

		Zip& operator = (Zip&& zip) noexcept {
			if (this != &zip) {
				freestanding::swap(handle, zip.handle);
				freestanding::swap(branch, zip.branch);
			}
			return *this;
		}

		~Zip() noexcept {
			Close();
		}

		/// <summary>
		/// 重定向
		/// </summary>
		/// <param name="fn">文件名</param>
		/// <param name="mode">打开模式</param>
		void Reset(std::wstring_view fn, OpenMode mode) noexcept {
			if (handle) {
				Close();
			}
			handle = details::MasterQian_Storage_Zip_ZipStart(fn.data(), static_cast<mqui32>(mode));
		}

		/// <summary>
		/// 关闭
		/// </summary>
		void Close() noexcept {
			if (handle) {
				details::MasterQian_Storage_Zip_ZipEnd(handle);
				handle = nullptr;
			}
		}

		[[nodiscard]] bool OK() const noexcept {
			return handle != nullptr;
		}

		/// <summary>
		/// 压缩数据
		/// </summary>
		/// <param name="src">压缩前数据</param>
		/// <param name="speedFirstly">若为true速度优先，否则压缩率优先</param>
		/// <returns>压缩后数据</returns>
		[[nodiscard]] static Bin Compress(BinView src, bool speedFirstly = true) noexcept {
			auto des_size{ details::MasterQian_Storage_Zip_CompressBound(src.size32()) };
			Bin des(des_size);
			des_size = details::MasterQian_Storage_Zip_Compress(src.data(), src.size32(), des.data(), des_size, speedFirstly);
			des.unsafe_shrink(des_size);
			return des;
		}
	};

	// Zip解压
	export struct UnZip {
	private:
		mqhandle handle{ };
	public:
		/// <summary>
		/// 打开
		/// </summary>
		/// <param name="fn">文件名</param>
		UnZip(std::wstring_view fn) noexcept {
			handle = details::MasterQian_Storage_Zip_UnZipStart(fn.data());
		}

		UnZip(UnZip const&) noexcept = delete;
		UnZip& operator = (UnZip const&) noexcept = delete;

		UnZip(UnZip&& unzip) noexcept {
			freestanding::swap(handle, unzip.handle);
		}

		UnZip& operator = (UnZip&& unzip) noexcept {
			if (this != &unzip) {
				freestanding::swap(handle, unzip.handle);
			}
			return *this;
		}

		~UnZip() noexcept {
			Close();
		}

		/// <summary>
		/// 重定向
		/// </summary>
		/// <param name="fn">文件名</param>
		void Reset(std::wstring_view fn) noexcept {
			if (handle) {
				Close();
			}
			handle = details::MasterQian_Storage_Zip_UnZipStart(fn.data());
		}

		/// <summary>
		/// 关闭
		/// </summary>
		/// <returns></returns>
		void Close() noexcept {
			if (handle) {
				details::MasterQian_Storage_Zip_UnZipEnd(handle);
				handle = nullptr;
			}
		}

		[[nodiscard]] bool OK() const noexcept {
			return handle != nullptr;
		}

		/// <summary>
		/// 解压
		/// </summary>
		/// <param name="path">目标路径</param>
		/// <param name="callback">回调</param>
		/// <param name="arg">可携带参数</param>
		bool Save(std::wstring_view path, UnZipCallBack callback = nullptr, mqmem arg = nullptr) const noexcept {
			if (callback) {
				return details::MasterQian_Storage_Zip_UnZipSaveWithCallback(handle, path.data(), callback, arg);
			}
			return details::MasterQian_Storage_Zip_UnZipSave(handle, path.data());
		}

		/// <summary>
		/// 解压数据
		/// </summary>
		/// <param name="src">压缩后数据</param>
		/// <returns>压缩前数据</returns>
		[[nodiscard]] static Bin Uncompress(BinView src) noexcept {
			Bin des;
			auto des_size{ details::MasterQian_Storage_Zip_UncompressBound(src.data(), src.size32()) };
			if (des_size) {
				des.reserve(des_size);
				if (!details::MasterQian_Storage_Zip_Uncompress(src.data(), src.size32(), des.data(), des.size32())) {
					des = { };
				}
			}
			return des;
		}
	};
}