module;
#include "MasterQian.Meta.h"
#include <string>
#include <vector>
#define MasterQianModuleName(name) MasterQian_Storage_Path_##name
#define MasterQianModuleNameString(name) "MasterQian_Storage_Path_"#name
#ifdef _DEBUG
#define MasterQianLibString "MasterQian.Storage.Path.Debug.dll"
#else
#define MasterQianLibString "MasterQian.Storage.Path.dll"
#endif
#define MasterQianModuleVersion 20240131ULL
#pragma message("�������������������� Please copy [" MasterQianLibString "] into your program folder ��������������������")

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
#undef MasterQianModuleName
#undef MasterQianModuleNameString
#undef MasterQianLibString
#undef MasterQianModuleVersion

	// ·��
	export struct Path {
	private:
		static constexpr auto PATH_SLASH{ L'\\' };
		static constexpr auto PATH_MAX_LENGTH{ 259ULL };
		static constexpr auto PATH_MAX{ PATH_MAX_LENGTH + 1ULL };

		std::wstring mData;
	public:
		// ·������
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

			// �Ƿ����
			[[nodiscard]] bool Exists() const noexcept {
				return attr != NON_EXIST;
			}

			// �Ƿ�ֻ��
			[[nodiscard]] bool ReadOnly() const noexcept {
				return Exists() && (attr & READ_ONLY) != 0U;
			}

			// �Ƿ�����
			[[nodiscard]] bool Hidden() const noexcept {
				return Exists() && (attr & HIDDEN) != 0U;
			}

			// �Ƿ�����ϵͳ
			[[nodiscard]] bool System() const noexcept {
				return Exists() && (attr & SYSTEM) != 0U;
			}

			// �Ƿ���Ŀ¼
			[[nodiscard]] bool Folder() const noexcept {
				return Exists() && (attr & FOLDER) != 0U;
			}

			// �Ƿ����ļ�
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
		/// ���ַ�������
		/// </summary>
		/// <param name="path">·��</param>
		Path(std::wstring_view path) noexcept {
			auto copy_size{ path.size() };
			if (copy_size > PATH_MAX_LENGTH) copy_size = PATH_MAX_LENGTH;
			mData.assign(path.data(), copy_size);
			RemoveBackClash();
		}

		/// <summary>
		/// ���ַ�������
		/// </summary>
		/// <param name="path">·��</param>
		Path(mqcstr path) noexcept : Path(std::wstring_view(path)) {}

		/// <summary>
		/// ת�ַ���
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
		/// �����Ӽ�����ͬ��/�����
		/// </summary>
		/// <param name="name">�Ӽ������Զ���ӷָ�������������ָ���</param>
		/// <returns>��·��</returns>
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
		/// �����Ӽ���������ͬ��/=�����
		/// </summary>
		/// <param name="name">�Ӽ������Զ���ӷָ�������������ָ���</param>
		Path& Append(std::wstring_view name) noexcept {
			return *this /= name;
		}

		/// <summary>
		/// �Ƿ��Ǿ���·��
		/// </summary>
		[[nodiscard]] bool IsAbsolutePath() const noexcept {
			return mData.size() >= 2ULL && mData[1] == L':';
		}

		/// <summary>
		/// �Ƿ������·��
		/// </summary>
		[[nodiscard]] bool IsRelativePath() const noexcept {
			return !IsAbsolutePath();
		}

		/// <summary>
		/// ȡ������·��
		/// </summary>
		/// <returns>����C:���ְ����̷�����������·��</returns>
		[[nodiscard]] Path Driver() const noexcept {
			if (IsAbsolutePath()) {
				return Path{ mData.substr(0ULL, 2ULL) };
			}
			return { };
		}

		/// <summary>
		/// �Ƿ��и�·��
		/// </summary>
		[[nodiscard]] bool HasParent() const noexcept {
			return mData.rfind(PATH_SLASH) != std::wstring::npos;
		}

		/// <summary>
		/// ȡ��·��
		/// </summary>
		[[nodiscard]] Path Parent() const noexcept {
			if (auto slash_pos{ mData.rfind(PATH_SLASH) }; slash_pos != std::wstring::npos) {
				return Path{ mData.substr(0ULL, slash_pos) };
			}
			return { };
		}

		/// <summary>
		/// ȡ�ļ���
		/// </summary>
		[[nodiscard]] std::wstring_view Name() const noexcept {
			std::wstring_view data_view{ mData };
			if (auto slash_pos{ mData.rfind(PATH_SLASH) }; slash_pos != std::wstring::npos) {
				data_view = data_view.substr(slash_pos + 1ULL);
			}
			return data_view;
		}

		/// <summary>
		/// ȡ������չ�����ļ���
		/// </summary>
		[[nodiscard]] std::wstring_view NameWithoutExt() const noexcept {
			std::wstring_view name_view{ Name() };
			if (auto dot_pos{ name_view.rfind(L'.') }; dot_pos != std::wstring_view::npos) {
				name_view = name_view.substr(0ULL, dot_pos);
			}
			return name_view;
		}

		/// <summary>
		/// ȡ��չ��
		/// </summary>
		[[nodiscard]] std::wstring_view Extension() const noexcept {
			std::wstring_view name_view{ Name() };
			if (auto dot_pos{ name_view.rfind(L'.') }; dot_pos != std::wstring_view::npos) {
				return name_view.substr(dot_pos);
			}
			return { };
		}

		/// <summary>
		/// ȡ���������չ��
		/// </summary>
		[[nodiscard]] std::wstring_view ExtensionWithoutDot() const noexcept {
			std::wstring_view ext_view{ Extension() };
			return ext_view.empty() ? ext_view : ext_view.substr(1ULL);
		}

		/// <summary>
		/// ȡ����
		/// </summary>
		[[nodiscard]] Attributes Attribute() const noexcept {
			return details::MasterQian_Storage_Path_Attribute(mData.data());
		}

		/// <summary>
		/// �Ƿ����
		/// </summary>
		[[nodiscard]] bool Exists() const noexcept {
			return Attribute().Exists();
		}

		/// <summary>
		/// �Ƿ����ļ�
		/// </summary>
		[[nodiscard]] bool IsFile() const noexcept {
			return Attribute().File();
		}

		/// <summary>
		/// �Ƿ���Ŀ¼
		/// </summary>
		[[nodiscard]] bool IsFolder() const noexcept {
			return Attribute().Folder();
		}

		/// <summary>
		/// ��С��֧���ļ���Ŀ¼����
		/// </summary>
		/// <returns>64λ���ȵĴ�С</returns>
		[[nodiscard]] mqui64 Size() const noexcept {
			return details::MasterQian_Storage_Path_Size(mData.data());
		}

		/// <summary>
		/// ȡ����ʱ�䣬֧���ļ���Ŀ¼�ĵ���
		/// </summary>
		[[nodiscard]] Timestamp CreateTime() const noexcept {
			return details::MasterQian_Storage_Path_GetTime(mData.data(), 0U);
		}

		/// <summary>
		/// �ô���ʱ�䣬֧���ļ���Ŀ¼�ĵ���
		/// </summary>
		/// <param name="ts">ʱ���</param>
		/// <returns>�Ƿ�ɹ�</returns>
		bool CreateTime(Timestamp ts) const noexcept {
			return details::MasterQian_Storage_Path_SetTime(mData.data(), 0U, ts);
		}

		/// <summary>
		/// ȡ�޸�ʱ�䣬֧���ļ���Ŀ¼�ĵ���
		/// </summary>
		[[nodiscard]] Timestamp WriteTime() const noexcept {
			return details::MasterQian_Storage_Path_GetTime(mData.data(), 1U);
		}

		/// <summary>
		/// ���޸�ʱ�䣬֧���ļ���Ŀ¼�ĵ���
		/// </summary>
		/// <param name="ts">ʱ���</param>
		/// <returns>�Ƿ�ɹ�</returns>
		bool WriteTime(Timestamp ts) const noexcept {
			return details::MasterQian_Storage_Path_SetTime(mData.data(), 1U, ts);
		}

		/// <summary>
		/// ȡ����ʱ�䣬֧���ļ���Ŀ¼�ĵ���
		/// </summary>
		[[nodiscard]] Timestamp AccessTime() const noexcept {
			return details::MasterQian_Storage_Path_GetTime(mData.data(), 2U);
		}

		/// <summary>
		/// �÷���ʱ�䣬֧���ļ���Ŀ¼�ĵ���
		/// </summary>
		/// <param name="ts">ʱ���</param>
		/// <returns>�Ƿ�ɹ�</returns>
		bool AccessTime(Timestamp ts) const noexcept {
			return details::MasterQian_Storage_Path_SetTime(mData.data(), 2U, ts);
		}

		/// <summary>
		/// ���ļ�����֧���ļ��ĵ��ã��ɳ���4G
		/// </summary>
		/// <returns>�ļ��ֽڼ�</returns>
		[[nodiscard]] Bin Read() const noexcept {
			Bin tmp(details::MasterQian_Storage_Path_Size(mData.data()));
			details::MasterQian_Storage_Path_Read(mData.data(), tmp.size(), tmp.data());
			return tmp;
		}

		/// <summary>
		/// д�ļ�����֧���ļ��ĵ���
		/// </summary>
		/// <param name="bv">�ֽڼ����ɳ���4G</param>
		/// <returns>�Ƿ�ɹ�</returns>
		bool Write(BinView bv) const noexcept {
			return details::MasterQian_Storage_Path_Write(mData.data(), bv.size(), bv.data());
		}

		/// <summary>
		/// ���ƣ�֧���ļ���Ŀ¼�ĵ��ã���·��������ͬ�����Ƶ�Ŀ��·���ڣ������޸�����ʹ��CopyRename
		/// </summary>
		/// <param name="folder_path">Ŀ��·������֧��Ŀ¼</param>
		/// <param name="overridable">������</param>
		/// <returns>�Ƿ�ɹ�</returns>
		bool Copy(Path const& folder_path, bool overridable = true) const noexcept {
			return details::MasterQian_Storage_Path_Copy(Parent().mData.data(), Name().data(), folder_path.mData.data(), overridable);
		}

		/// <summary>
		/// ���ƣ�֧���ļ���Ŀ¼�ĵ��ã����Ʋ��޸����ļ���
		/// </summary>
		/// <param name="des">Ŀ��·��</param>
		/// <param name="overridable">������</param>
		/// <returns>�Ƿ�ɹ�</returns>
		bool CopyRename(Path const& des, bool overridable = true) const noexcept {
			return details::MasterQian_Storage_Path_CopyRename(mData.data(), des.mData.data(), overridable);
		}

		/// <summary>
		/// �ƶ���֧���ļ���Ŀ¼�ĵ���
		/// </summary>
		/// <param name="folder_path">Ŀ��·������֧��Ŀ¼</param>
		/// <returns>�Ƿ�ɹ�</returns>
		bool Move(Path const& folder_path) const noexcept {
			return details::MasterQian_Storage_Path_Move(Parent().mData.data(), Name().data(), folder_path.mData.data());
		}

		/// <summary>
		/// ��������֧���ļ���Ŀ¼�ĵ���
		/// </summary>
		/// <param name="name">�����ƣ����Զ����丸·������չ�������������·������չ��</param>
		/// <returns>�Ƿ�ɹ�</returns>
		bool Rename(std::wstring_view name) const noexcept {
			Path new_path{ Parent() };
			std::wstring new_name{ name };
			new_name += Extension();
			new_path /= new_name;
			return details::MasterQian_Storage_Path_Rename(mData.data(), new_path.mData.data());
		}

		/// <summary>
		/// ɾ����֧���ļ���Ŀ¼�ĵ���
		/// </summary>
		/// <returns>�Ƿ�ɹ�</returns>
		bool Delete() const noexcept {
			return details::MasterQian_Storage_Path_Delete(mData.data());
		}

		/// <summary>
		/// ɾ��������վ��֧���ļ���Ŀ¼�ĵ���
		/// </summary>
		/// <returns>�Ƿ�ɹ�</returns>
		bool DeleteToRecycleBin() const noexcept {
			return details::MasterQian_Storage_Path_DeleteToRecycleBin(mData.data());
		}

		/// <summary>
		/// ö��Ŀ¼����֧��Ŀ¼�ĵ��ã���ö��Ŀ¼�ڵ���·��
		/// </summary>
		/// <param name="ext">��չ������*.jpg��ʾ��ö��jpg��ʽ���ļ���*��ʾƥ�������ļ���Ŀ¼</param>
		/// <returns>��·������</returns>
		[[nodiscard]] std::vector<Path> EnumFolder(std::wstring_view ext = L"*") const noexcept {
			std::vector<Path> subPaths;
			if (!details::MasterQian_Storage_Path_EnumFolder(mData.data(), [ ] (mqmem arg, mqcstr path) noexcept {
				static_cast<std::vector<Path>*>(arg)->emplace_back(path);
				}, &subPaths, ext.data())) {
				subPaths.clear();
			}
			return subPaths;
		}

		/// <summary>
		/// ���ö��Ŀ¼����֧��Ŀ¼�ĵ��ã��ݹ�ö��Ŀ¼��������Ŀ¼�������ļ���Ŀ¼
		/// </summary>
		/// <returns>��·������</returns>
		[[nodiscard]] std::vector<Path> EnumFolderDepth() const noexcept {
			std::vector<Path> subPaths;
			if (!details::MasterQian_Storage_Path_EnumFolderDepth(mData.data(), [ ] (mqmem arg, mqcstr path) noexcept {
				static_cast<std::vector<Path>*>(arg)->emplace_back(path);
				}, &subPaths)) {
				subPaths.clear();
			}
			return subPaths;
		}

		/// <summary>
		/// ȷ���ļ����ڣ���֧���ļ��ĵ��ã����ļ���������ʹ��Ĭ��ֵ�������������
		/// </summary>
		/// <param name="bv">Ĭ��ֵ</param>
		void Create(BinView bv) const noexcept {
			if (!Exists()) {
				Write(bv);
			}
		}

		/// <summary>
		/// ȷ���༶Ŀ¼���ڣ���֧��Ŀ¼�ĵ��ã���Ŀ¼�������򴴽���������ԣ�������;�༶��������·��˳�����δ���
		/// </summary>
		void Create() const noexcept {
			details::MasterQian_Storage_Path_CreateFolder(mData.data());
		}

		/// <summary>
		/// ȡ��ݷ�ʽĿ�꣬��֧���ļ��ĵ���
		/// </summary>
		/// <returns>lnk�ļ���Ӧ��Ŀ��</returns>
		[[nodiscard]] Path ShortCutTarget() const noexcept {
			mqchar buf[PATH_MAX]{ };
			details::MasterQian_Storage_Path_ShortCutTarget(mData.data(), buf);
			return std::wstring_view{ buf };
		}


		/*  ��̬����  */

		/// <summary>
		/// ȡ����·��
		/// </summary>
		[[nodiscard]] static Path Desktop() noexcept {
			mqchar buf[PATH_MAX]{ };
			details::MasterQian_Storage_Path_GetStandardPath(0x0000, buf);
			return std::wstring_view{ buf };
		}

		/// <summary>
		/// ȡ�ĵ�·��
		/// </summary>
		[[nodiscard]] static Path Documents() noexcept {
			mqchar buf[PATH_MAX]{ };
			details::MasterQian_Storage_Path_GetStandardPath(0x0005, buf);
			return std::wstring_view{ buf };
		}

		/// <summary>
		/// ȡ����·��
		/// </summary>
		[[nodiscard]] static Path Music() noexcept {
			mqchar buf[PATH_MAX]{ };
			details::MasterQian_Storage_Path_GetStandardPath(0x000D, buf);
			return std::wstring_view{ buf };
		}

		/// <summary>
		/// ȡ��Ƶ·��
		/// </summary>
		[[nodiscard]] static Path Video() noexcept {
			mqchar buf[PATH_MAX]{ };
			details::MasterQian_Storage_Path_GetStandardPath(0x000E, buf);
			return std::wstring_view{ buf };
		}

		/// <summary>
		/// ȡ����·��
		/// </summary>
		[[nodiscard]] static Path Fonts() noexcept {
			mqchar buf[PATH_MAX]{ };
			details::MasterQian_Storage_Path_GetStandardPath(0x0014, buf);
			return std::wstring_view{ buf };
		}

		/// <summary>
		/// ȡAppData·��
		/// </summary>
		[[nodiscard]] static Path AppData() noexcept {
			mqchar buf[PATH_MAX]{ };
			details::MasterQian_Storage_Path_GetStandardPath(0x001A, buf);
			return std::wstring_view{ buf };
		}

		/// <summary>
		/// ȡWindows·��
		/// </summary>
		[[nodiscard]] static Path Windows() noexcept {
			mqchar buf[PATH_MAX]{ };
			details::MasterQian_Storage_Path_GetStandardPath(0x0024, buf);
			return std::wstring_view{ buf };
		}

		/// <summary>
		/// ȡϵͳ·��
		/// </summary>
		[[nodiscard]] static Path System() noexcept {
			mqchar buf[PATH_MAX]{ };
			details::MasterQian_Storage_Path_GetStandardPath(0x0025, buf);
			return std::wstring_view{ buf };
		}

		/// <summary>
		/// ȡProgramFiles·��
		/// </summary>
		[[nodiscard]] static Path ProgramFiles() noexcept {
			mqchar buf[PATH_MAX]{ };
			details::MasterQian_Storage_Path_GetStandardPath(0x0026, buf);
			return std::wstring_view{ buf };
		}

		/// <summary>
		/// ȡ��Ƭ·��
		/// </summary>
		[[nodiscard]] static Path Picture() noexcept {
			mqchar buf[PATH_MAX]{ };
			details::MasterQian_Storage_Path_GetStandardPath(0x0027, buf);
			return std::wstring_view{ buf };
		}

		/// <summary>
		/// ȡ��ǰ���еĳ����·��
		/// </summary>
		[[nodiscard]] static Path Running() noexcept {
			mqchar buf[PATH_MAX]{ };
			details::MasterQian_Storage_Path_GetStandardPath(5201314, buf);
			return std::wstring_view{ buf };
		}

		/// <summary>
		/// ȡ��ǰ���г�����ļ���
		/// </summary>
		[[nodiscard]] static Path RunningName() noexcept {
			mqchar buf[PATH_MAX]{ };
			details::MasterQian_Storage_Path_GetStandardPath(5201315, buf);
			return std::wstring_view{ buf };
		}

		/// <summary>
		/// ȡTemp·��
		/// </summary>
		[[nodiscard]] static Path Temp() noexcept {
			mqchar buf[PATH_MAX]{ };
			details::MasterQian_Storage_Path_GetStandardPath(5201316, buf);
			return std::wstring_view{ buf };
		}

		/// <summary>
		/// ����Ӧ��ǰĿ¼�������еĳ���ǰĿ¼�޸�Ϊ�������ڵ�Ŀ¼�������Ӹ����̵ĵ���
		/// </summary>
		static void AutoCurrentFolder() noexcept {
			details::MasterQian_Storage_Path_AutoCurrentFolder(Running().mData.data());
		}
	};

	// ������
	export struct Driver {
		// ����
		Path name;
		// �ܿռ�(GB)
		mqf64 totalSpace{ };
		// ʣ��ռ�(GB)
		mqf64 freeSpace{ };
	};

	// ���������������
	export struct Drivers : private std::vector<Driver> {
		using std::vector<Driver>::empty;
		using std::vector<Driver>::size;
		using std::vector<Driver>::begin;
		using std::vector<Driver>::end;
		using std::vector<Driver>::cbegin;
		using std::vector<Driver>::cend;
		using std::vector<Driver>::rbegin;
		using std::vector<Driver>::rend;
		using std::vector<Driver>::crbegin;
		using std::vector<Driver>::crend;
		using std::vector<Driver>::operator[];

		/// <summary>
		/// ȡ��������
		/// </summary>
		/// <returns>����������</returns>
		[[nodiscard]] static Drivers Get() noexcept {
			mqchar names[27]{ }, name[3]{ };
			mqui32 count{ details::MasterQian_Storage_Path_GetDriversBitmap(names) };
			Drivers drivers;
			drivers.resize(count);
			for (mqui32 i{ }; i < count; ++i) {
				auto& driver{ drivers[i] };
				details::MasterQian_Storage_Path_GetDriverSpaceInfo(names[i], name, &driver.totalSpace, &driver.freeSpace);
				driver.name = std::wstring_view(name);
			}
			return drivers;
		}
	};

	// ����վ��������
	export namespace RecycleBin {
		/// <summary>
		/// ����վ�Ƿ�Ϊ��
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
		/// ��ջ���վ
		/// </summary>
		/// <param name="tip">�Ƿ񵯳���ʾ����</param>
		inline void Clear(bool tip = false) noexcept {
			details::MasterQian_Storage_Path_ClearRecycleBin(tip);
		}
	};
}

export [[nodiscard]] inline MasterQian::Storage::Path operator ""_path(mqcstr str, mqui64 size) noexcept {
	return MasterQian::Storage::Path{ std::wstring_view{ str, size } };
}