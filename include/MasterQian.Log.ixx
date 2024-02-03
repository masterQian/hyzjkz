module;
#include "MasterQian.Meta.h"
#include <string>
#define MasterQianModuleVersion 20240131ULL

export module MasterQian.Log;
export import MasterQian.freestanding;

export namespace MasterQian {
	// ��־����
	enum class LogType : mqenum {
		STD_CONSOLE, // ����̨��־
		DEBUG_CONSOLE, // ���Դ�����־
		FILE, // �ļ���־
	};

	// ��־��ǩ
	enum class LogTag : mqenum {
		INFO, // ��Ϣ
		WARNING, // ����
		ERR // ����
	};
}

namespace MasterQian::api {
	META_WINAPI(mqhandle, GetStdHandle, mqui32);
	META_WINAPI(mqbool, CloseHandle, mqhandle);
	META_WINAPI(mqhandle, CreateFileW, mqcstr, mqui32, mqui32, mqhandle, mqui32, mqui32, mqhandle);
	META_WINAPI(mqbool, WriteFile, mqhandle, mqcmem, mqui32, mqui32*, mqhandle);
	META_WINAPI(mqbool, WriteConsoleW, mqhandle, mqcmem, mqui32, mqui32*, mqmem);
	META_WINAPI(mqbool, SetConsoleTextAttribute, mqhandle, mqui16);
	META_WINAPI(void, OutputDebugStringW, mqcstr);

#pragma comment(linker,"/alternatename:__imp_?OutputDebugStringW@api@MasterQian@@YAXPEB_W@Z::<!MasterQian.Log>=__imp_OutputDebugStringW")
#pragma comment(linker,"/alternatename:__imp_?WriteFile@api@MasterQian@@YAHPEAXPEBXIPEAI0@Z::<!MasterQian.Log>=__imp_WriteFile")
#pragma comment(linker,"/alternatename:__imp_?WriteConsoleW@api@MasterQian@@YAHPEAXPEBXIPEAI0@Z::<!MasterQian.Log>=__imp_WriteConsoleW")
#pragma comment(linker,"/alternatename:__imp_?SetConsoleTextAttribute@api@MasterQian@@YAHPEAXG@Z::<!MasterQian.Log>=__imp_SetConsoleTextAttribute")
#pragma comment(linker,"/alternatename:__imp_?CloseHandle@api@MasterQian@@YAHPEAX@Z::<!MasterQian.Log>=__imp_CloseHandle")
#pragma comment(linker,"/alternatename:__imp_?CreateFileW@api@MasterQian@@YAPEAXPEB_WIIPEAXII1@Z::<!MasterQian.Log>=__imp_CreateFileW")
#pragma comment(linker,"/alternatename:__imp_?GetStdHandle@api@MasterQian@@YAPEAXI@Z::<!MasterQian.Log>=__imp_GetStdHandle")
}

namespace MasterQian::details {
	template<typename T>
	concept can_to_wstring = requires (freestanding::remove_cvref<T> t) {
		{ std::to_wstring(t) } -> freestanding::same<std::wstring>;
	} && !requires(freestanding::remove_cvref<T> t) {
		std::wstring(t);
	};

	template<can_to_wstring T>
	inline void LoggerLogValue(std::wstring& buf, T&& t) {
		buf += std::to_wstring(freestanding::forward<T>(t));
	}

	inline void LoggerLogValue(std::wstring& buf, std::wstring_view sv) {
		buf += sv;
	}

	template<typename T>
	concept log_userstruct_func = requires (std::wstring & buf, freestanding::remove_cvref<T> const t) {
		{ Log(buf, t) } noexcept -> freestanding::same<void>;
	};

	template<typename T>
	concept log_userstruct_member_func = requires (std::wstring & buf, freestanding::remove_cvref<T> const t) {
		{ t.Log(buf) } noexcept -> freestanding::same<void>;
	};

	/// <summary>
	/// <para>�û�����ͨ��ʵ��Log�ӿ�������Զ�������</para>
	/// <para>���������Pos��Ϊ��</para>
	/// <example>
	/// <code>
	/// struct Pos {
	///		int x, y;
	///		inline void Log(std::wstring& buf) const noexcept {
	///			buf += std::to_wstring(x) + L"," + std::wstring(y);
	///		}
	/// }
	/// ��
	/// struct Pos {
	///		int x, y;
	/// }
	/// inline void Log(std::wstring& buf, Pos pos) noexcept {
	///		buf += std::to_wstring(pos.x) + L"," + std::wstring(pos.y);
	/// }
	/// </code>
	/// </example>
	/// </summary>
	template<typename T>
	requires (log_userstruct_func<T> || log_userstruct_member_func<T>)
	inline void LoggerLogValue(std::wstring& buf, T const& t) {
		if constexpr (log_userstruct_func<T>) {
			Log(buf, t);
		}
		else {
			t.Log(buf);
		}
	}

	inline mqhandle CreateLogger(LogType type, mqcmem arg) noexcept {
		switch (type) {
		case LogType::STD_CONSOLE: {
			return api::GetStdHandle(static_cast<mqui32>(-11));
		}
		case LogType::DEBUG_CONSOLE: {
			return reinterpret_cast<mqhandle>(5201314U);
		}
		case LogType::FILE: {
			if (arg) {
				auto hFile{ api::CreateFileW(static_cast<mqcstr>(arg), 0x40000000U, 0,
					nullptr, 2U, 0x00000080U, nullptr) };
				return hFile != reinterpret_cast<mqhandle>(-1) ? hFile : nullptr;
			}
			break;
		}
		}
		return nullptr;
	}

	inline void CloseLogger(LogType type, mqhandle handle) noexcept {
		if (type == LogType::FILE) {
			api::CloseHandle(handle);
		}
	}

	inline void LoggerLog(LogType type, LogTag tag, mqhandle handle, mqcstr msg, mqui32 size) noexcept {
		switch (type) {
		case LogType::STD_CONSOLE: {
			switch (tag) {
			case LogTag::INFO: {
				api::SetConsoleTextAttribute(handle, 0x0002U);
				break;
			}
			case LogTag::WARNING: {
				api::SetConsoleTextAttribute(handle, 0x0001U);
				break;
			}
			case LogTag::ERR: {
				api::SetConsoleTextAttribute(handle, 0x0004U);
				break;
			}
			}
			api::WriteConsoleW(handle, msg, size, nullptr, nullptr);
			api::SetConsoleTextAttribute(handle, 0x0004U | 0x0002U | 0x0001U);
			break;
		}
		case LogType::FILE: {
			auto len{ api::WideCharToMultiByte(65001U, 0, msg, size, nullptr, 0, nullptr, nullptr) };
			if (len > 0) {
				static std::string buffer;
				if (buffer.capacity() < len) {
					buffer.reserve(len);
				}
				api::WideCharToMultiByte(65001U, 0, msg, size, buffer.data(), len, nullptr, nullptr);
				api::WriteFile(handle, buffer.data(), len, nullptr, nullptr);
			}
			break;
		}
		case LogType::DEBUG_CONSOLE: {
			api::OutputDebugStringW(msg);
			break;
		}
		}
	}
}

export namespace MasterQian {
	template<LogType type>
	struct Logger {
	private:
		mqhandle handle;

		static constexpr mqcstr LogTagString[] = { L"[Info] ", L"[Warning] ", L"[Error] " };
	public:
		/// <summary>
		/// ���ļ���־��Ҫ���ļ�����Ϊ��������������Ҫ
		/// </summary>
		/// <param name="arg">����</param>
		Logger(mqcmem arg = nullptr) noexcept {
			handle = details::CreateLogger(type, arg);
		}

		Logger(Logger const&) = delete;
		Logger& operator = (Logger const&) = delete;

		~Logger() noexcept {
			close();
		}

		/// <summary>
		/// �ض���
		/// </summary>
		/// <param name="arg">����</param>
		void reset(mqcmem arg = nullptr) noexcept {
			close();
			handle = details::CreateLogger(type, arg);
		}

		/// <summary>
		/// �ر�
		/// </summary>
		void close() noexcept {
			if (handle) {
				details::CloseLogger(type, handle);
				handle = nullptr;
			}
		}

		/// <summary>
		/// ���
		/// </summary>
		/// <typeparam name="tag">��־��ǩ</typeparam>
		/// <typeparam name="newLine">�Զ�����</typeparam>
		template<LogTag tag = LogTag::INFO, bool newLine = true, typename... Args>
		void log(Args&&... args) const noexcept {
			if (handle) {
				std::wstring buf{ LogTagString[static_cast<mqui32>(tag)] };
				(details::LoggerLogValue(buf, freestanding::forward<Args>(args)), ...);
				if (newLine) {
					buf += (type == LogType::FILE ? L"\r\n" : L"\n");
				}
				details::LoggerLog(type, tag, handle, buf.data(), static_cast<mqui32>(buf.size()));
			}
		}

		/// <summary>
		/// �����Ϣ
		/// </summary>
		template<bool newLine = true, typename... Args>
		void i(Args&&... args) const noexcept {
			log<LogTag::INFO, newLine>(freestanding::forward<Args>(args)...);
		}

		/// <summary>
		/// �������
		/// </summary>
		template<bool newLine = true, typename... Args>
		void w(Args&&... args) const noexcept {
			log<LogTag::WARNING, newLine>(freestanding::forward<Args>(args)...);
		}

		/// <summary>
		/// �������
		/// </summary>
		template<bool newLine = true, typename... Args>
		void e(Args&&... args) const noexcept {
			log<LogTag::ERR, newLine>(freestanding::forward<Args>(args)...);
		}
	};

	// ����̨��־������stdout���
	using ConsoleLogger = Logger<LogType::STD_CONSOLE>;

	// ���Դ�����־������debug�������
	using DebugLogger = Logger<LogType::DEBUG_CONSOLE>;

	// �ļ���־��ָ���ļ����
	using FileLogger = Logger<LogType::FILE>;
}


/*    �Ի����������֧��    */

// mqguid
export inline void Log(std::wstring& buf, mqguid const& guid) noexcept {
	mqchar str[37]{ };
	MasterQian::api::wsprintfW(str, L"%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
		guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2],
		guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
	buf += str;
}

// mqpoint
export inline void Log(std::wstring& buf, mqpoint point) noexcept {
	mqchar str[64]{ };
	MasterQian::api::wsprintfW(str, L"(%u, %u)", point.x, point.y);
	buf += str;
}

// mqsize
export inline void Log(std::wstring& buf, mqsize size) noexcept {
	mqchar str[64]{ };
	MasterQian::api::wsprintfW(str, L"(%u, %u)", size.width, size.height);
	buf += str;
}

// mqpoint
export inline void Log(std::wstring& buf, mqrect rect) noexcept {
	mqchar str[128]{ };
	MasterQian::api::wsprintfW(str, L"(%u, %u, %u, %u)", rect.left, rect.top, rect.width, rect.height);
	buf += str;
}

// mqpoint
export inline void Log(std::wstring& buf, mqrange range) noexcept {
	mqchar str[128]{ };
	MasterQian::api::wsprintfW(str, L"(%u, %u, %u, %u)", range.left, range.top, range.right, range.bottom);
	buf += str;
}