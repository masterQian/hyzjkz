module;
#include "MasterQian.Meta.h"
#include <string>
#include <unordered_map>
#define MasterQianModuleVersion 20240131ULL

export module MasterQian.Parser.Ini;
export import MasterQian.freestanding;

namespace MasterQian::Parser {
	namespace details {
		mqcstr IniScanLine(mqcstr pData, mqcstr* p1, mqcstr* p2, mqcstr* p3, mqcstr* p4) {
			mqcstr pStart{ pData };
			while (*pData && *pData != L'\r' && *pData != L'\n') ++pData; // 寻找到行尾
			mqcstr pEnd{ pData };
			while (*pStart == L' ' || *pStart == L'\t') ++pStart; // 跳过行首的空白
			while (*pEnd == L'\r' || *pEnd == L'\n') --pEnd; // 跳过行尾的换行
			while (*pEnd == L'\0' || *pEnd == L' ' || *pEnd == L'\t') --pEnd; // 跳过行尾的空白
			std::wstring_view line{ pStart, pEnd + 1ULL };
			while (*pData == L'\r' || *pData == L'\n') ++pData; // 跳过行尾的换行
			*p1 = *p2 = *p3 = *p4 = nullptr;
			if (line.front() == L'[' && line.back() == L']') { // 节
				pStart = &line.front() + 1;
				pEnd = &line.back() - 1;
				while (*pStart == L' ' || *pStart == L'\t') ++pStart; // 跳过[后的空白
				while (*pEnd == L' ' || *pEnd == L'\t') --pEnd; // 跳过]前的空白
				// 节名(若p1 == p2是空节名)
				*p1 = pStart;
				*p2 = pEnd + 1;
			}
			else if (line.front() == L';') { } // 注释
			else if (auto pos{ line.rfind(L'=') }; pos != std::wstring_view::npos) { // 项
				auto left{ line.substr(0, pos) };
				auto right{ line.substr(pos + 1) };
				pEnd = &left.back();
				pStart = &right.front();
				while (*pEnd == L' ' || *pEnd == L'\t') --pEnd; // 跳过=前的空白
				while (*pStart == L' ' || *pStart == L'\t') ++pStart; // 跳过=后的空白
				*p1 = &left.front(); // 键
				*p2 = pEnd + 1;
				*p3 = pStart; // 值
				*p4 = &right.back() + 1;
			}
			else {
				return nullptr;
			}
			return pData;
		}
	}

	// ini解析结果
	export enum class IniError {
		OK, // 成功
		INVALID_LINE, // 非法行
		EMPTY_SECTION_NAME, // 空节名
		EMPTY_KEY, // 空键名
		ISOLATED_ITEM // 孤立项
	};

	struct IniView;

	// 节视图
	export struct SectionView : protected std::unordered_map<std::wstring_view, std::wstring_view> {
		friend struct IniView;
		using BaseT = std::unordered_map<std::wstring_view, std::wstring_view>;
		using BaseT::empty;
		using BaseT::size;
		using BaseT::contains;

		[[nodiscard]] const auto begin() const noexcept {
			return BaseT::begin();
		}

		[[nodiscard]] const auto end() const noexcept {
			return BaseT::end();
		}

		[[nodiscard]] std::wstring_view operator [] (std::wstring_view key) const noexcept {
			if (auto iter{ find(key) }; iter != cend()) {
				return iter->second;
			}
			return { };
		}
	};

	// ini视图
	export struct IniView : protected std::unordered_map<std::wstring_view, SectionView> {
	protected:
		using BaseT = std::unordered_map<std::wstring_view, SectionView>;

		std::wstring content;
		
		inline static SectionView _empty_section{ };

		void _clear() noexcept {
			clear();
			content.clear();
		}
	public:
		IniView() = default;

		IniView(std::wstring_view data) noexcept {
			Load(data);
		}

		IniView(IniView const&) = delete;
		IniView& operator = (IniView const&) = delete;

		using BaseT::empty;
		using BaseT::size;
		using BaseT::contains;

		[[nodiscard]] const auto begin() const noexcept {
			return BaseT::begin();
		}

		[[nodiscard]] const auto end() const noexcept {
			return BaseT::end();
		}

		[[nodiscard]] SectionView const& operator[](std::wstring_view sv) const noexcept {
			if (auto iter{ find(sv) }; iter != cend()) {
				return iter->second;
			}
			return _empty_section;
		}

		/// <summary>
		/// 载入
		/// </summary>
		/// <param name="data">ini文本</param>
		IniError Load(std::wstring_view data) noexcept {
			clear();
			content = data;
			SectionView* curSection{ };
			mqcstr pData{ content.data() }, p1{ }, p2{ }, p3{ }, p4{ };
			while (pData && *pData) {
				pData = details::IniScanLine(pData, &p1, &p2, &p3, &p4);
				if (pData) {
					if (p3 && p4) { // 项
						if (curSection) { // 当前节存在
							std::wstring_view key{ p1, p2 }, value{ p3, p4 };
							if (key.empty()) {
								_clear();
								return IniError::EMPTY_KEY;
							}
							else {
								if (auto iter{ curSection->find(key) }; iter != curSection->cend()) {
									iter->second = value; // 更新当前项
								}
								else {
									curSection->emplace(key, value); // 创建新项
								}
							}
						}
						else {
							_clear();
							return IniError::ISOLATED_ITEM;
						}
					}
					else if (p1 && p2) { // 节
						if (std::wstring_view name{ p1, p2 }; !name.empty()) {
							if (auto iter{ find(name) }; iter != cend()) { // 更新当前节
								curSection = &iter->second;
							}
							else {
								curSection = &emplace(name, SectionView{ }).first->second; // 创建新节
							}
						}
						else {
							_clear();
							return IniError::EMPTY_SECTION_NAME;
						}
					}
					else { } // 注释
				}
				else {
					_clear();
					return IniError::INVALID_LINE;
				}
			}
			return IniError::OK;
		}
	};

	// 节
	export using Section = std::unordered_map<std::wstring, std::wstring, freestanding::isomerism_hash, freestanding::isomerism_equal>;

	// ini配置文件
	export struct Ini : public std::unordered_map<std::wstring, Section, freestanding::isomerism_hash, freestanding::isomerism_equal> {
		Ini() = default;

		Ini(std::wstring_view data) noexcept {
			Load(data);
		}

		/// <summary>
		/// 载入
		/// </summary>
		/// <param name="data">ini文本</param>
		IniError Load(std::wstring_view data) noexcept {
			clear();
			Section* curSection{ };
			mqcstr pData{ data.data() }, p1{ }, p2{ }, p3{ }, p4{ };
			while (pData && *pData) {
				pData = details::IniScanLine(pData, &p1, &p2, &p3, &p4);
				if (pData) {
					if (p3 && p4) { // 项
						if (curSection) { // 当前节存在
							std::wstring key{ p1, p2 }, value{ p3, p4 };
							if (key.empty()) {
								clear();
								return IniError::EMPTY_KEY;
							}
							else {
								if (auto iter{ curSection->find(key) }; iter != curSection->cend()) {
									iter->second = value; // 更新当前项
								}
								else {
									curSection->emplace(std::move(key), std::move(value)); // 创建新项
								}
							}
						}
						else {
							clear();
							return IniError::ISOLATED_ITEM;
						}
					}
					else if (p1 && p2) { // 节
						if (std::wstring name{ p1, p2 }; !name.empty()) {
							if (auto iter{ find(name) }; iter != cend()) { // 更新当前节
								curSection = &iter->second;
							}
							else {
								curSection = &emplace(std::move(name), Section{ }).first->second; // 创建新节
							}
						}
						else {
							clear();
							return IniError::EMPTY_SECTION_NAME;
						}
					}
					else { } // 注释
				}
				else {
					clear();
					return IniError::INVALID_LINE;
				}
			}
			return IniError::OK;
		}

		/// <summary>
		/// 保存
		/// </summary>
		/// <returns>ini文本</returns>
		std::wstring Save() const noexcept {
			mqui64 totalSize{ };
			for (auto& [name, sec] : *this) {
				totalSize += name.size();
				for (auto& [key, value] : sec) {
					totalSize += key.size() + value.size();
				}
				totalSize += sec.size() * 2ULL; // 额外的=与\n
			}
			totalSize += size() * 3ULL; // 额外的[]与\n

			std::wstring buf;
			buf.reserve(totalSize);
			for (auto& [name, sec] : *this) {
				buf.push_back(L'[');
				buf += name;
				buf += L"]\n";
				for (auto& [key, value] : sec) {
					buf += key;
					buf.push_back(L'=');
					buf += value;
					buf.push_back(L'\n');
				}
			}
			return buf;
		}
	};
}