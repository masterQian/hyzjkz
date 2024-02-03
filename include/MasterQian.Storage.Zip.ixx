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
#pragma message("�������������������� Please copy [" MasterQianLibString "] into your program folder ��������������������")

export module MasterQian.Storage.Zip;
export import MasterQian.Bin;

namespace MasterQian::Storage {
	/// <summary>
	/// ѹ���ص�
	/// </summary>
	/// <param name="arg">��Я������</param>
	/// <param name="result">��ѹ��;�Ƿ�����</param>
	/// <param name="new_name">ѹ�������ļ���</param>
	/// <param name="old_name">Դ�ļ���</param>
	export using ZipCallBack = void(__stdcall*)(mqmem, bool, mqcstr, mqcstr);

	/// <summary>
	/// ��ѹ�ص�
	/// </summary>
	/// <param name="arg">��Я������</param>
	/// <param name="new_name">ѹ�������ļ���</param>
	/// <param name="old_name">Դ�ļ���</param>
	export using UnZipCallBack = void(__stdcall*)(mqmem, mqcstr, mqcstr);

	namespace details {
		META_IMPORT_API(mqui32, CompressBound, mqui32);
		META_IMPORT_API(mqui32, Compress, mqcbytes, mqui32, mqbytes, mqui32, mqbool);
		META_IMPORT_API(mqui32, UncompressBound, mqcbytes);
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

	// ѹ����֧
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
		/// ����ļ�
		/// </summary>
		/// <param name="name">����</param>
		/// <param name="fn">�ļ�·��</param>
		/// <returns>�Ƿ�ɹ�</returns>
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
		/// ����ֽڼ�
		/// </summary>
		/// <param name="name">����</param>
		/// <param name="bv">�ֽڼ�</param>
		/// <returns>�Ƿ�ɹ�</returns>
		bool AddBin(std::wstring_view name, BinView bv) const noexcept {
			if (handle) {
				std::wstring fullname{ branch };
				fullname += name;
				return details::MasterQian_Storage_Zip_ZipAddBin(handle, fullname.data(), bv.data(), bv.size32());
			}
			return false;
		}

		/// <summary>
		/// ����ļ���
		/// </summary>
		/// <param name="fn">����</param>
		/// <param name="createSelf">��Ϊtrue���ļ���������Ϊ��ѹ����ȥ������ֻ���ڲ��ļ�ѹ����ȥ</param>
		/// <param name="callback">ѹ���ص�</param>
		/// <param name="arg">��Я������</param>
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

	// Zipѹ��
	export struct Zip : public ZipBranch {
	public:
		// ��ģʽ
		enum class OpenMode : mqenum {
			CREATE, // �����ڴ���, ����ʧ��
			CREATE_OPEN, // �����ڴ���, ���ڴ�
			CREATE_OVERRIDE, // �����ڴ���, ���ڸ���
			OPEN_EXIST, // ������ʧ��, ���ڴ�
		};

		/// <summary>
		/// ��
		/// </summary>
		/// <param name="fn">�ļ���</param>
		/// <param name="mode">��ģʽ</param>
		Zip(std::wstring_view fn, OpenMode mode) noexcept :
			ZipBranch(details::MasterQian_Storage_Zip_ZipStart(fn.data(), static_cast<mqui32>(mode)), L"") {}

		Zip(Zip const&) = delete;
		Zip& operator = (Zip const&) = delete;

		~Zip() noexcept {
			Close();
		}

		/// <summary>
		/// �ض���
		/// </summary>
		/// <param name="fn">�ļ���</param>
		/// <param name="mode">��ģʽ</param>
		void Reset(std::wstring_view fn, OpenMode mode) noexcept {
			if (handle) {
				Close();
			}
			handle = details::MasterQian_Storage_Zip_ZipStart(fn.data(), static_cast<mqui32>(mode));
		}

		/// <summary>
		/// �ر�
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
		/// ѹ������
		/// </summary>
		/// <param name="src">ѹ��ǰ����</param>
		/// <param name="speedFirstly">��Ϊtrue�ٶ����ȣ�����ѹ��������</param>
		/// <returns>ѹ��������</returns>
		[[nodiscard]] static Bin Compress(BinView src, bool speedFirstly = true) noexcept {
			auto des_size{ details::MasterQian_Storage_Zip_CompressBound(src.size32()) };
			Bin des(des_size);
			des_size = details::MasterQian_Storage_Zip_Compress(src.data(), src.size32(), des.data(), des_size, speedFirstly);
			des.resize(des_size);
			return des;
		}
	};

	// Zip��ѹ
	export struct UnZip {
	private:
		mqhandle handle{ };
	public:
		/// <summary>
		/// ��
		/// </summary>
		/// <param name="fn">�ļ���</param>
		UnZip(std::wstring_view fn) noexcept {
			handle = details::MasterQian_Storage_Zip_UnZipStart(fn.data());
		}

		UnZip(UnZip const&) = delete;
		UnZip& operator = (UnZip const&) = delete;

		~UnZip() noexcept {
			Close();
		}

		/// <summary>
		/// �ض���
		/// </summary>
		/// <param name="fn">�ļ���</param>
		void Reset(std::wstring_view fn) noexcept {
			if (handle) {
				Close();
			}
			handle = details::MasterQian_Storage_Zip_UnZipStart(fn.data());
		}

		/// <summary>
		/// �ر�
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
		/// ��ѹ
		/// </summary>
		/// <param name="path">Ŀ��·��</param>
		/// <param name="callback">�ص�</param>
		/// <param name="arg">��Я������</param>
		bool Save(std::wstring_view path, UnZipCallBack callback = nullptr, mqmem arg = nullptr) const noexcept {
			if (callback) {
				return details::MasterQian_Storage_Zip_UnZipSaveWithCallback(handle, path.data(), callback, arg);
			}
			return details::MasterQian_Storage_Zip_UnZipSave(handle, path.data());
		}

		/// <summary>
		/// ��ѹ����
		/// </summary>
		/// <param name="src">ѹ��������</param>
		/// <returns>ѹ��ǰ����</returns>
		[[nodiscard]] static Bin Uncompress(BinView src) noexcept {
			Bin des;
			auto des_size{ details::MasterQian_Storage_Zip_UncompressBound(src.data()) };
			if (des_size) {
				des.resize(des_size);
				if (!details::MasterQian_Storage_Zip_Uncompress(src.data(), src.size32(), des.data(), des.size32())) {
					des = { };
				}
			}
			return des;
		}
	};
}