module;
#include "MasterQian.Meta.h"
#include <string>
#include <vector>
#define MasterQianModuleName(name) MasterQian_System_##name
#define MasterQianModuleNameString(name) "MasterQian_System_"#name
#ifdef _DEBUG
#define MasterQianLibString "MasterQian.System.Debug.dll"
#else
#define MasterQianLibString "MasterQian.System.dll"
#endif
#define MasterQianModuleVersion 20240131ULL
#pragma message("—————————— Please copy [" MasterQianLibString "] into your program folder ——————————")

export module MasterQian.System;
export import MasterQian.Bin;

namespace MasterQian::System {
	// 点击模式
	export enum class MouseClickMode : mqenum {
		L, // 左键单击
		LD, // 左键双击
		M, // 中键单击
		MD, // 中键双击
		R, // 右键单击
		RD, // 右键双击
	};

	// 键代码
	export enum class KeyCode : mqenum {
		None = 0U,
		Backspace = 8U, Tab,
		Enter = 13U,
		Shift = 16U, Ctrl, Alt, Pause, CapLock,
		Esc = 27U,
		Space = 32U, Pageup, Pagedown, End, Home, Left, Up, Right, Down,
		Insert = 45U, Delete,
		N0 = 48U, N1, N2, N3, N4, N5, N6, N7, N8, N9,
		A = 65U, B, C, D, E, F, G, H, I, J, K, L, M,
		N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
		WinLeft = 91U, WinRight,
		R0 = 96U, R1, R2, R3, R4, R5, R6, R7, R8, R9,
		Multiply = 106U, Add,
		Subtract = 109U, Del, Divide,
		F1 = 112U, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
		NumLock = 144U
	};

	namespace details {
		META_IMPORT_API(mqpoint, GetMousePos);
		META_IMPORT_API(void, SetMousePos, mqpoint);
		META_IMPORT_API(void, MouseClick, mqpoint, MouseClickMode);
		META_IMPORT_API(void, MouseWheel, mqi32);
		META_IMPORT_API(void, KeyboardClick, KeyCode, KeyCode, KeyCode);

		META_IMPORT_API(void, ClearClipboard);
		META_IMPORT_API(mqhandle, GetClipboardDataSize, mqui32, mqui64*);
		META_IMPORT_API(mqbool, GetClipboardData, mqhandle, mqmem, mqui64);
		META_IMPORT_API(mqbool, SetClipboardData, mqui32, mqcmem, mqui64);
		META_IMPORT_API(mqbool, SetClipboardFiles, mqcstr, mqui64);

		META_IMPORT_API(mqui32, GetScreenWidth);
		META_IMPORT_API(mqui32, GetScreenHeight);
		META_IMPORT_API(mqui32, GetTaskBarHeight);
		META_IMPORT_API(void, GetCurrentUserName, mqstr);

		META_IMPORT_API(mqcbytes, GetResource, mqui32, mqcstr, mqui32*);
		META_IMPORT_API(mqbool, SingleProcessLock);
		META_IMPORT_API(mqui32, Execute, mqcstr, mqcstr, mqbool, mqbool);

		META_IMPORT_API(void, ShowTaskBar, mqbool);

		META_IMPORT_API(mqbool, RegOpen, mqhandle*, mqbool, mqhandle, mqcstr);
		META_IMPORT_API(mqbool, RegClose, mqhandle*);
		META_IMPORT_API(mqbool, RegCreateItem, mqhandle, mqbool, mqcstr, mqhandle*);
		META_IMPORT_API(mqbool, RegDeleteItem, mqhandle, mqbool, mqcstr);
		META_IMPORT_API(mqbool, RegEnumItem, mqhandle, mqui32, mqstr);
		META_IMPORT_API(mqbool, RegHasKey, mqhandle, mqcstr);
		META_IMPORT_API(mqbool, RegGetValueTypeAndSize, mqhandle, mqcstr, mqui32*, mqui32*);
		META_IMPORT_API(mqbool, RegGetValue, mqhandle, mqcstr, mqmem, mqui32*);
		META_IMPORT_API(mqbool, RegSetValue, mqhandle, mqcstr, mqui32, mqcmem, mqui32);
		META_IMPORT_API(mqbool, RegDeleteKey, mqhandle, mqcstr);
		META_IMPORT_API(mqbool, RegGetKeyValueMaxSize, mqhandle, mqui32*, mqui32*);
		META_IMPORT_API(mqui32, RegEnumKey, mqhandle, mqui32, mqui32, mqstr, mqui32*);
		META_IMPORT_API(void, FlushEnvironment);
		META_MODULE_BEGIN
			META_PROC_API(GetMousePos);
			META_PROC_API(SetMousePos);
			META_PROC_API(MouseClick);
			META_PROC_API(MouseWheel);
			META_PROC_API(KeyboardClick);

			META_PROC_API(ClearClipboard);
			META_PROC_API(GetClipboardDataSize);
			META_PROC_API(GetClipboardData);
			META_PROC_API(SetClipboardData);
			META_PROC_API(SetClipboardFiles);

			META_PROC_API(GetScreenWidth);
			META_PROC_API(GetScreenHeight);
			META_PROC_API(GetTaskBarHeight);
			META_PROC_API(GetCurrentUserName);

			META_PROC_API(GetResource);
			META_PROC_API(SingleProcessLock);
			META_PROC_API(Execute);

			META_PROC_API(ShowTaskBar);

			META_PROC_API(RegOpen);
			META_PROC_API(RegClose);
			META_PROC_API(RegCreateItem);
			META_PROC_API(RegDeleteItem);
			META_PROC_API(RegEnumItem);
			META_PROC_API(RegHasKey);
			META_PROC_API(RegGetValueTypeAndSize);
			META_PROC_API(RegGetValue);
			META_PROC_API(RegSetValue);
			META_PROC_API(RegDeleteKey);
			META_PROC_API(RegGetKeyValueMaxSize);
			META_PROC_API(RegEnumKey);
			META_PROC_API(FlushEnvironment);
		META_MODULE_END
	}

	export namespace AutoDevice {
		/// <summary>
		/// 取鼠标位置
		/// </summary>
		/// <returns>鼠标位置</returns>
		[[nodiscard]] inline mqpoint GetMousePos() noexcept {
			return details::MasterQian_System_GetMousePos();
		}

		/// <summary>
		/// 鼠标模拟移动
		/// </summary>
		/// <param name="point">鼠标位置</param>
		/// <returns></returns>
		inline void SetMousePos(mqpoint point) noexcept {
			details::MasterQian_System_SetMousePos(point);
		}

		/// <summary>
		/// 鼠标模拟点击
		/// </summary>
		/// <param name="point">鼠标位置</param>
		/// <typeparam name="mode">点击模式</typeparam>
		template<MouseClickMode mode>
		inline void MouseClick(mqpoint point) noexcept {
			details::MasterQian_System_MouseClick(point, mode);
		}

		/// <summary>
		/// 鼠标模拟滑轮
		/// </summary>
		/// <param name="len">滚动距离，负数表示反方向滚动</param>
		inline void MouseWheel(mqi32 len) noexcept {
			details::MasterQian_System_MouseWheel(len);
		}

		/// <summary>
		/// 键盘模拟按键，最多支持3个键组合
		/// </summary>
		/// <typeparam name="key1">键1</typeparam>
		/// <typeparam name="key1">键2</typeparam>
		/// <typeparam name="key1">键3</typeparam>
		template<KeyCode key1, KeyCode key2 = KeyCode::None, KeyCode key3 = KeyCode::None>
		inline void KeyboardClick() noexcept {
			details::MasterQian_System_KeyboardClick(key1, key2, key3);
		}
	}

	export namespace Clipboard {
		/// <summary>
		/// 清空剪贴板
		/// </summary>
		inline void Clear() noexcept {
			details::MasterQian_System_ClearClipboard();
		}

		/// <summary>
		/// 取剪贴板文本
		/// </summary>
		[[nodiscard]] inline std::wstring GetString() noexcept {
			mqui64 size{ };
			std::wstring str;
			if (auto handle{ details::MasterQian_System_GetClipboardDataSize(13U, &size) }) {
				str.resize((size - 1) >> 1);
				if (!details::MasterQian_System_GetClipboardData(handle, str.data(), size)) {
					str = { }; // move construct, free memory
				}
			}
			return str;
		}

		/// <summary>
		/// 置剪贴板文本
		/// </summary>
		/// <param name="sv">文本</param>
		inline bool SetString(std::wstring_view sv) noexcept {
			return details::MasterQian_System_SetClipboardData(13U, sv.data(), (sv.size() + 1) << 1);
		}

		/// <summary>
		/// 取剪贴板位图
		/// </summary>
		/// <returns>BMP格式的字节集</returns>
		[[nodiscard]] inline Bin GetBitmap() noexcept {
			mqui64 size{ };
			Bin bin;
			if (auto handle{ details::MasterQian_System_GetClipboardDataSize(8, &size) }) {
				bin.resize(size + 2);
				auto pData{ bin.data() };
				if (details::MasterQian_System_GetClipboardData(handle, pData + 2, size)) {
					// BMP File
					// | 66 | 77 | size + 2 <4bytes> | 0 | 0 | 0 | 0 | 54 | 0 | 0 | 0 |
					// | <16bytes header> | 0 | 0 | 0 | 0 | size - 52 <4bytes> | <... content> |

					pData[0] = 66; // 'B'
					pData[1] = 77; // 'M'
					freestanding::copy(pData + 26, pData + 14, 4); // Prevent overlapping copies
					freestanding::copy(pData + 14, pData + 2, 12);
					*reinterpret_cast<mqi32*>(pData + 2) = static_cast<mqi32>(size + 2);
					freestanding::initialize(pData + 6, 0, 8ULL);
					pData[10] = 54;
					freestanding::initialize(pData + 30, 0, 4ULL);
					*reinterpret_cast<mqi32*>(pData + 34) = static_cast<mqi32>(size - 52);
				}
				else {
					bin = { }; // move construct, free memory
				}
			}
			return bin;
		}

		/// <summary>
		/// 置剪贴板文件，可用于在文件资源管理器复制粘贴等操作
		/// </summary>
		/// <param name="files">文件列表</param>
		inline bool SetFiles(std::vector<std::wstring> const& files) noexcept {
			std::wstring buf;
			for (auto& item : files) {
				buf += item;
				buf.push_back(L'\0');
			}
			return details::MasterQian_System_SetClipboardFiles(buf.data(), (buf.size() + 1) << 1);
		}

		/// <summary>
		/// 取剪贴板文件
		/// </summary>
		[[nodiscard]] inline std::vector<std::wstring> GetFiles() noexcept {
			mqui64 size{ };
			std::vector<std::wstring> files;
			if (auto handle{ details::MasterQian_System_GetClipboardDataSize(15, &size) }) {
				std::wstring buf(size >> 1, L'\0');
				if (details::MasterQian_System_GetClipboardData(handle, buf.data(), size)) {
					for (mqcstr p{ buf.data() + 20ULL / sizeof(mqchar) }; *p; p += files.back().size() + 1) {
						files.emplace_back(p);
					}
				}
			}
			return files;
		}
	}

	export namespace Info {
		/// <summary>
		///  取屏幕宽度
		/// </summary>
		[[nodiscard]] inline mqui32 GetScreenWidth() noexcept {
			return details::MasterQian_System_GetScreenWidth();
		}

		/// <summary>
		/// 取屏幕高度
		/// </summary>
		[[nodiscard]] inline mqui32 GetScreenHeight() noexcept {
			return details::MasterQian_System_GetScreenHeight();
		}

		/// <summary>
		/// 取任务栏高度
		/// </summary>
		[[nodiscard]] inline mqui32 GetTaskBarHeight() noexcept {
			return details::MasterQian_System_GetTaskBarHeight();
		}

		/// <summary>
		/// 取当前用户名
		/// </summary>
		[[nodiscard]] inline std::wstring GetCurrentUserName() noexcept {
			mqchar buf[api::PATH_MAX_SIZE]{ };
			details::MasterQian_System_GetCurrentUserName(buf);
			return buf;
		}
	}

	export namespace Process {
		/// <summary>
		/// 取资源，应为rc文件中对应的资源
		/// </summary>
		/// <param name="id">资源ID</param>
		/// <param name="type">资源类型，默认为FILE</param>
		/// <returns>资源字节集</returns>
		[[nodiscard]] inline BinView GetResource(mqui32 id, std::wstring_view type = L"FILE") noexcept {
			mqui32 res_size{ };
			auto mem{ details::MasterQian_System_GetResource(id, type.data(), &res_size) };
			return { mem, static_cast<mqui64>(res_size) };
		}

		/// <summary>
		/// 单进程锁，保证应用程序仅有一个实例
		/// </summary>
		/// <returns>若运行的程序不是第一个实例则返回false</returns>
		[[nodiscard]] inline mqbool SingleProcessLock() noexcept {
			return details::MasterQian_System_SingleProcessLock();
		}

		/// <summary>
		/// 执行进程
		/// </summary>
		/// <param name="fn">进程文件名</param>
		/// <param name="arg">参数</param>
		/// <param name="isWaiting">是否等待进程结束，默认为假</param>
		/// <param name="isAdmin">是否以管理员身份运行，默认为假</param>
		/// <returns>进程返回值，仅在等待进程结束时有效</returns>
		inline mqui32 Execute(std::wstring_view fn, std::wstring_view arg = L"",
			mqbool isWaiting = false, mqbool isAdmin = false) noexcept {
			return details::MasterQian_System_Execute(fn.data(), arg.data(), isWaiting, isAdmin);
		}
	}

	export namespace Window {
		/// <summary>
		/// 显示任务栏
		/// </summary>
		/// <param name="status">是否显示任务栏</param>
		inline void ShowTaskBar(bool status = true) noexcept {
			details::MasterQian_System_ShowTaskBar(status);
		}
	}

	// 注册表操作对象
	export struct Reg {
	private:
		mqhandle handle{ };
		mqbool bWOW64{ };
	public:
		// 键类型
		enum class KeyType : mqenum {
			None = 0U, // 空
			String = 1U, // 字符串
			_ES = 2U,
			Binary = 3U, // 二进制
			Number = 4U, // 数字
			_BN = 5U, _Link = 6U,
			MultiLineString = 7U, // 多行字符串
			_RL = 8U, _FRD = 9U, _RRL = 10U, _N64 = 11U
		};

		// 根键
		enum class KeyBase : mqui64 {
			ClassesRoot = 0x80000000ULL,
			CurrentUser = 0x80000001ULL,
			LocalMachine = 0x80000002ULL,
			Users = 0x80000003ULL,
			CurrentConfig = 0x80000005ULL
		};

		Reg() = default;

		/// <summary>
		/// 打开注册表
		/// </summary>
		/// <param name="base">根键</param>
		/// <param name="route">路径</param>
		/// <param name="wow64">是否是64位映像</param>
		Reg(KeyBase base, std::wstring_view route, mqbool wow64 = true) noexcept : bWOW64{ wow64 } {
			Open(base, route);
		}

		~Reg() noexcept {
			Close();
		}

		Reg(Reg const&) = delete;
		Reg& operator = (Reg const&) = delete;

		/// <summary>
		/// 置WOW64
		/// </summary>
		/// <param name="wow64">是否是64位映像</param>
		void SetWOW64(mqbool wow64) noexcept {
			bWOW64 = wow64;
		}

		/// <summary>
		/// 是否是WOW64
		/// </summary>
		[[nodiscard]] mqbool IsWOW64() const noexcept {
			return bWOW64;
		}

		/// <summary>
		/// 是否打开
		/// </summary>
		[[nodiscard]] mqbool IsOpen() const noexcept {
			return handle != nullptr;
		}

		/// <summary>
		/// 打开
		/// </summary>
		/// <param name="base">根键</param>
		/// <param name="route">路径</param>
		/// <returns>是否成功</returns>
		mqbool Open(KeyBase base, std::wstring_view route) noexcept {
			if (handle) Close();
			return details::MasterQian_System_RegOpen(&handle, bWOW64, reinterpret_cast<mqhandle>(base), route.data());
		}

		/// <summary>
		/// 关闭
		/// </summary>
		/// <returns>是否成功</returns>
		mqbool Close() noexcept {
			return details::MasterQian_System_RegClose(&handle);
		}

		/// <summary>
		/// 创建项
		/// </summary>
		/// <param name="route">路径</param>
		/// <param name="ret">返回的项，若为空表示忽略</param>
		/// <returns>是否成功</returns>
		mqbool CreateItem(std::wstring_view route, Reg* ret = nullptr) const noexcept {
			return handle ? details::MasterQian_System_RegCreateItem(handle, bWOW64, route.data(), ret ? &ret->handle : nullptr) : false;
		}

		/// <summary>
		/// 删除项
		/// </summary>
		/// <param name="route">路径</param>
		/// <returns>是否成功</returns>
		mqbool DeleteItem(std::wstring_view route) const noexcept {
			return handle ? details::MasterQian_System_RegDeleteItem(handle, bWOW64, route.data()) : false;
		}

		/// <summary>
		/// 枚举项
		/// </summary>
		/// <returns>路径下的所有项集合</returns>
		[[nodiscard]] std::vector<std::wstring> EnumItem() const noexcept {
			std::vector<std::wstring> container;
			if (handle) {
				mqchar buf[api::PATH_MAX_SIZE]{ };
				for (mqui32 index{ }; details::MasterQian_System_RegEnumItem(handle, index, buf); ++index) {
					container.emplace_back(buf);
				}
			}
			return container;
		}

		/// <summary>
		/// 是否有键
		/// </summary>
		/// <param name="key">键名</param>
		[[nodiscard]] mqbool HasKey(std::wstring_view key) const noexcept {
			return handle ? details::MasterQian_System_RegHasKey(handle, key.data()) : false;
		}

		/// <summary>
		/// 取键类型
		/// </summary>
		/// <param name="key">键名</param>
		/// <returns>键类型</returns>
		[[nodiscard]] KeyType GetKeyType(std::wstring_view key) const noexcept {
			KeyType type{ };
			if (handle) {
				mqui32 size;
				details::MasterQian_System_RegGetValueTypeAndSize(handle, key.data(), reinterpret_cast<mqui32*>(&type), &size);
			}
			return type;
		}

		/// <summary>
		/// 取二进制值
		/// </summary>
		/// <param name="key">键名</param>
		[[nodiscard]] Bin GetBinaryValue(std::wstring_view key) const noexcept {
			Bin bin;
			if (handle) {
				KeyType type{ };
				mqui32 size;
				if (details::MasterQian_System_RegGetValueTypeAndSize(handle, key.data(), reinterpret_cast<mqui32*>(&type), &size)) {
					if (type == KeyType::Binary) {
						bin.resize(static_cast<mqui64>(size));
						details::MasterQian_System_RegGetValue(handle, key.data(), bin.data(), &size);
					}
				}
			}
			return bin;
		}

		/// <summary>
		/// 取数字值
		/// </summary>
		/// <param name="key">键名</param>
		[[nodiscard]] mqui64 GetNumberValue(std::wstring_view key) const noexcept {
			mqui64 value{ };
			if (handle) {
				KeyType type{ };
				mqui32 size;
				if (details::MasterQian_System_RegGetValueTypeAndSize(handle, key.data(), reinterpret_cast<mqui32*>(&type), &size)) {
					if (type == KeyType::Number || type == KeyType::_N64) {
						details::MasterQian_System_RegGetValue(handle, key.data(), &value, &size);
					}
				}
			}
			return value;
		}

		/// <summary>
		/// 取字符串值
		/// </summary>
		/// <param name="key">键名</param>
		[[nodiscard]] std::wstring GetStringValue(std::wstring_view key) const noexcept {
			std::wstring str;
			if (handle) {
				KeyType type{ };
				mqui32 size;
				if (details::MasterQian_System_RegGetValueTypeAndSize(handle, key.data(), reinterpret_cast<mqui32*>(&type), &size)) {
					switch (type) {
					case KeyType::String:
					case KeyType::_ES:
					case KeyType::MultiLineString: {
						if (size < 2U) {
							size = 2U;
						}
						str.resize(static_cast<mqui64>((size - 2U) >> 1U), L'\0');
						details::MasterQian_System_RegGetValue(handle, key.data(), str.data(), &size);
						if (type == KeyType::MultiLineString) {
							for (auto& ch : str) {
								if (ch == L'\0') ch = L'\n';
							}
						}
						break;
					}
					case KeyType::Number:
					case KeyType::_N64: {
						mqui64 value{ };
						details::MasterQian_System_RegGetValue(handle, key.data(), &value, &size);
						str = std::to_wstring(value);
						break;
					}
					}
				}
			}
			return str;
		}

		/// <summary>
		/// 置二进制值
		/// </summary>
		/// <param name="key">键名</param>
		/// <param name="value">键值</param>
		/// <returns>是否成功</returns>
		mqbool SetBinaryValue(std::wstring_view key, BinView value) const noexcept {
			return handle ? details::MasterQian_System_RegSetValue(handle, key.data(), static_cast<mqui32>(KeyType::Binary), value.data(), value.size32()) : false;
		}

		/// <summary>
		/// 置数字值
		/// </summary>
		/// <param name="key">键名</param>
		/// <param name="value">键值</param>
		/// <returns>是否成功</returns>
		mqbool SetNumberValue(std::wstring_view key, mqui64 value) const noexcept {
			if (handle) {
				if (value > 0xFFFFFFFFULL) {
					return details::MasterQian_System_RegSetValue(handle, key.data(), static_cast<mqui32>(KeyType::_N64), &value, sizeof(mqui64));
				}
				mqui32 value32{ static_cast<mqui32>(value) };
				return details::MasterQian_System_RegSetValue(handle, key.data(), static_cast<mqui32>(KeyType::Number), &value32, sizeof(mqui32));
			}
			return false;
		}

		/// <summary>
		/// 置字符串值
		/// </summary>
		/// <param name="key">键名</param>
		/// <param name="value">键值</param>
		/// <param name="multiline">是否多行</param>
		/// <returns>是否成功</returns>
		mqbool SetStringValue(std::wstring_view key, std::wstring_view value, mqbool multiline = false) const noexcept {
			if (handle) {
				if (multiline) {
					std::wstring tmp(value);
					for (auto& ch : tmp) {
						if (ch == L'\n') ch = L'\0';
					}
					return details::MasterQian_System_RegSetValue(handle, key.data(), static_cast<mqui32>(KeyType::MultiLineString), tmp.data(), static_cast<mqui32>(tmp.size() << 1ULL));
				}
				return details::MasterQian_System_RegSetValue(handle, key.data(), static_cast<mqui32>(KeyType::String), value.data(), static_cast<mqui32>(value.size() << 1ULL));
			}
			return false;
		}

		/// <summary>
		/// 删除键
		/// </summary>
		/// <param name="key">键名</param>
		/// <returns>是否成功</returns>
		mqbool DeleteKey(std::wstring_view key) const noexcept {
			return handle ? details::MasterQian_System_RegDeleteKey(handle, key.data()) : false;
		}

		/// <summary>
		/// 枚举键值对
		/// </summary>
		/// <returns>项下所有键值对的集合</returns>
		[[nodiscard]] std::vector<std::pair<std::wstring, KeyType>> EnumKey() const noexcept {
			std::vector<std::pair<std::wstring, KeyType>> container;
			if (handle) {
				mqui32 maxKeySize{ };
				if (details::MasterQian_System_RegGetKeyValueMaxSize(handle, &maxKeySize, nullptr)) {
					std::wstring buf(maxKeySize, L'\0');
					KeyType type{ };
					for (mqui32 keySize{ }, index{ };
						(keySize = details::MasterQian_System_RegEnumKey(
							handle, index, maxKeySize + 1U, buf.data(), 
							reinterpret_cast<mqui32*>(&type))) != 0U;
						++index) {
						container.emplace_back(buf.data(), type);
					}
				}
			}
			return container;
		}

		/// <summary>
		/// 刷新环境变量，来使得所有应用程序收到修改后注册表的更新
		/// </summary>
		static void FlushEnvironment() noexcept {
			details::MasterQian_System_FlushEnvironment();
		}
	};
}