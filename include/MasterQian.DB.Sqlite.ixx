module;
#include "MasterQian.Meta.h"
#include <string>
#include <unordered_map>
#define MasterQianModuleName(name) MasterQian_DB_Sqlite_##name
#define MasterQianModuleNameString(name) "MasterQian_DB_Sqlite_"#name
#ifdef _DEBUG
#define MasterQianLibString "MasterQian.DB.Sqlite.Debug.dll"
#else
#define MasterQianLibString "MasterQian.DB.Sqlite.dll"
#endif
#define MasterQianModuleVersion 20240131ULL
#pragma message("—————————— Please copy [" MasterQianLibString "] into your program folder ——————————")

export module MasterQian.DB.Sqlite;
export import MasterQian.freestanding;

namespace MasterQian::DB {
	namespace details {
		META_IMPORT_API(mqenum, Open, mqhandle*, mqcstr);
		META_IMPORT_API(mqenum, Close, mqhandle*);
		META_IMPORT_API(mqenum, LastResult, mqhandle);
		META_IMPORT_API(mqcstr, LastResultString, mqhandle);
		META_IMPORT_API(mqenum, Execute, mqhandle, mqcstr);
		META_IMPORT_API(mqhandle, QueryPrepare, mqhandle, mqcstr);
		META_IMPORT_API(mqenum, QueryFinalize, mqhandle);
		META_IMPORT_API(mqui32, QueryColumnCount, mqhandle);
		META_IMPORT_API(mqcstr, QueryColumnName, mqhandle, mqui32);
		META_IMPORT_API(mqbool, QueryHasRow, mqhandle);
		META_IMPORT_API(mqcstr, QueryRow, mqhandle, mqui32);
		META_MODULE_BEGIN
			META_PROC_API(Open);
			META_PROC_API(Close);
			META_PROC_API(LastResult);
			META_PROC_API(LastResultString);
			META_PROC_API(Execute);
			META_PROC_API(QueryPrepare);
			META_PROC_API(QueryFinalize);
			META_PROC_API(QueryColumnCount);
			META_PROC_API(QueryColumnName);
			META_PROC_API(QueryHasRow);
			META_PROC_API(QueryRow);
		META_MODULE_END
	}

	// Sqlite3数据库对象
	export struct Sqlite {
	private:
		mqhandle handle{ };
	public:
		// Sqlite操作结果
		enum class Result : mqenum {
			OK, ERROR, INTERNAL, PERM, ABORT, BUSY, LOCKED, MOMEM, READONLY, INTERRUPT,
			IOERR, CORRUPT, NOTFOUND, FULL, CANTOPEN, PROTOCOL, EMPTY, SCHEMA, TOOBIG,
			CONSTRAINT, MISMATCH, MISUSE, NOLFS, AUTH, FORMAT, RANGE, NOTADB,
			NOTICE, WARNING,
			ROW = 100, DONE,
		};

		// 列定义
		using ColDef = std::unordered_map<std::wstring, mqui32, freestanding::isomerism_hash, freestanding::isomerism_equal>;

		// 行
		struct Row : protected mqlist<std::wstring> {
			using BaseT = mqlist<std::wstring>;
		protected:
			ColDef& colName;

			inline static std::wstring _EMPTYSTRING{ };
		public:
			Row(mqui64 count, ColDef& colDef) : BaseT{ count }, colName{ colDef } {
				BaseT::init(count);
			}

			using BaseT::begin;
			using BaseT::end;
			using BaseT::operator[];

			[[nodiscard]] std::wstring const& operator [] (std::wstring_view name) const noexcept {
				if (auto iter{ colName.find(name) }; iter != colName.cend()) {
					return BaseT::operator[](iter->second);
				}
				return _EMPTYSTRING;
			}

			[[nodiscard]] std::wstring& operator [] (std::wstring_view name) noexcept {
				if (auto iter{ colName.find(name) }; iter != colName.cend()) {
					return BaseT::operator[](iter->second);
				}
				return _EMPTYSTRING;
			}
		};

		// 表
		struct Table : protected mqlist<Row> {
			using BaseT = mqlist<Row>;
			ColDef colName;

			Table(mqhandle handle) : BaseT{ } {
				if (handle) {
					auto count{ details::MasterQian_DB_Sqlite_QueryColumnCount(handle) };
					for (mqui32 i{ }; i < count; ++i) {
						colName.emplace(details::MasterQian_DB_Sqlite_QueryColumnName(handle, i), i);
					}
					while (details::MasterQian_DB_Sqlite_QueryHasRow(handle)) {
						auto& rv{ BaseT::add(count, colName) };
						for (mqui32 j{ }; j < count; ++j) {
							rv[j] = details::MasterQian_DB_Sqlite_QueryRow(handle, j);
						}
					}
					details::MasterQian_DB_Sqlite_QueryFinalize(handle);
				}
			}

			using BaseT::empty;
			using BaseT::size;
			using BaseT::begin;
			using BaseT::end;
			using BaseT::operator[];
		};

		Sqlite() noexcept = default;

		Sqlite(std::wstring_view fn) noexcept {
			details::MasterQian_DB_Sqlite_Open(&handle, fn.data());
		}

		~Sqlite() noexcept {
			details::MasterQian_DB_Sqlite_Close(&handle);
		}

		Sqlite(Sqlite const&) noexcept = delete;
		Sqlite& operator = (Sqlite const&) noexcept = delete;

		Sqlite(Sqlite&& sqlite) noexcept {
			freestanding::swap(handle, sqlite.handle);
		}

		Sqlite& operator = (Sqlite&& sqlite) noexcept {
			if (this != &sqlite) {
				freestanding::swap(handle, sqlite.handle);
			}
			return *this;
		}

		/// <summary>
		/// 打开数据库
		/// </summary>
		/// <param name="fn">Sqlite3数据库文件名</param>
		/// <returns>操作结果</returns>
		Result Open(std::wstring_view fn) noexcept {
			if (handle) {
				details::MasterQian_DB_Sqlite_Close(&handle);
			}
			return static_cast<Result>(details::MasterQian_DB_Sqlite_Open(&handle, fn.data()));
		}

		/// <summary>
		/// 关闭数据库
		/// </summary>
		/// <returns>操作结果</returns>
		Result Close() noexcept {
			return static_cast<Result>(details::MasterQian_DB_Sqlite_Close(&handle));
		}

		/// <summary>
		/// 取最后结果
		/// </summary>
		/// <returns>操作结果</returns>
		[[nodiscard]] Result LastResult() const noexcept {
			return static_cast<Result>(details::MasterQian_DB_Sqlite_LastResult(handle));
		}

		/// <summary>
		/// 取最后结果文本
		/// </summary>
		/// <returns>操作结果文本</returns>
		[[nodiscard]] std::wstring LastResultString() const noexcept {
			return details::MasterQian_DB_Sqlite_LastResultString(handle);
		}

		/// <summary>
		/// 取所有表名
		/// </summary>
		/// <returns>数据库内所有表名</returns>
		[[nodiscard]] mqlist<std::wstring> TablesName() const noexcept {
			mqlist<std::wstring> container;
			for (auto& row : Query(L"select name from sqlite_master where type = 'table'")) {
				container.add(row[0ULL]);
			}
			return container;
		}

		/// <summary>
		/// 执行SQL
		/// </summary>
		/// <param name="sql">sql语句</param>
		/// <returns>操作结果</returns>
		Result Execute(std::wstring_view sql) const noexcept {
			return static_cast<Result>(details::MasterQian_DB_Sqlite_Execute(handle, sql.data()));
		}

		/// <summary>
		/// 查询SQL
		/// </summary>
		/// <param name="sql">sql语句</param>
		/// <returns>查询表</returns>
		[[nodiscard]] Table Query(std::wstring_view sql) const noexcept {
			return Table{ details::MasterQian_DB_Sqlite_QueryPrepare(handle, sql.data()) };
		}
	};
}